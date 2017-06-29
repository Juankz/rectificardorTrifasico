#include <Arduino.h>

#define DEBUG 0

const int S1_S2 = 4;
const int S3_S4 = 5;
const int S5_S6 = 6;

const int pinLed = 13;
const int pinSalida = 3;
const int pinCrucex0 = 2;
const int pinAlfa = A0; //Entrada del potenciómetro que medirá el ángulo alfa
const int f = 60;
const float DaG = float(180)/float(1023); //Digital a Grados
const float DaMicros = float(1)/float(1023); //Digital a microsegundos
const float DIF_PULSOS_GRADOS=60;
const float CAMBIO_FASE_GRADOS=30;

/* Estados */
const short ESPERAR_ALFA = 0;
const short DISPARANDO = 1;
const short ESPERANDO_SIG_PULSO = 2;
const short APAGAR = 3;
short estado = ESPERAR_ALFA;

/* Timers microsegundos*/
long ultimo_t0;
long ultimo_t1;
long actual_t0;
long actual_t1;
long ultimo_t2_cruce;
long ultimo_t3_cruce;

/* Trifasica */
int senales[6]; // Secuencia: ACN - CNB - ANB - ANC - CBN - ABN
int pines_senales[] = {7,8,9,10,11,12};
int senal_actual = 6;

boolean cruce_detectado = false;
short contador_cruces = 0;
float T = (1/(2*(float)f))*1000000; //Un solo semiciclo. us
int valorAlfaSensado; //Alamcena el valor del divisor de voltaje por potenciómetro
int alfa = 10; //Valor entre 0 y 180
float alfaMicroseconds; //valor en us
float anchoPulso = 0.07*T; // Valor en us
float dif_pulsos_us=(DIF_PULSOS_GRADOS/float(360*60))*1000000;
float cambio_fase_us = (CAMBIO_FASE_GRADOS/float(f*360))*1000000;

void apagar_todo(){
  for(int i = 0; i < 6;i++){
    if(senales [i] != LOW){
      senales[i]=LOW;
      digitalWrite(pines_senales[i], LOW);
    }
  }
}

int obtener_prox_senal(int s){
  if(s >= 5){
    return 0;
  }else{
    return s+1;
  }
}

void cruce(){
  actual_t1 = micros();
  /*Aseguramos que solo tome el cruce de un semiciclo*/
  if(actual_t1-ultimo_t1>T*1.5){
    ultimo_t1 = micros();
    cruce_detectado = true;
    // ultimo_t0=micros();
  }
}

void setup(){
  #if DEBUG
    Serial.begin(9600);
  #endif
  pinMode(pinSalida,OUTPUT);
  pinMode(pinCrucex0, INPUT_PULLUP);
  pinMode(S1_S2,OUTPUT);
  pinMode(S3_S4,OUTPUT);
  pinMode(S5_S6,OUTPUT);
  for(int i = 0; i < 6; i++){
    pinMode(pines_senales[i],OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(pinCrucex0), cruce, RISING);
  ultimo_t1 = micros();
}

void loop(){
  noInterrupts();
  valorAlfaSensado = analogRead(pinAlfa);
  interrupts();
  #if DEBUG
    /*Solo se usa para comprobar ciertos valores en comunicación serial*/
    alfa = valorAlfaSensado*DaG;
    alfaMicroseconds = T*alfa/180;
  #else
    /*Las dos lineas anteriores se pueden reducir a la siguiente*/
    alfaMicroseconds = T*valorAlfaSensado*DaMicros;
  #endif

  if(alfaMicroseconds>T-500){alfaMicroseconds = T - 500;}

  /* Calcular*/
  noInterrupts();
  boolean cruce_detectado_copia = cruce_detectado;
  interrupts();

  float alfa_real_us = cambio_fase_us+alfaMicroseconds;

  if(cruce_detectado_copia){
    actual_t0 = micros();
    int prox_senal = obtener_prox_senal(senal_actual);

    switch (estado) {

      /*Una vez que llegue al alfa, escribir las dos señales en alto*/
      case ESPERAR_ALFA:
        if(actual_t0 - ultimo_t1 > alfa_real_us){
          senales [senal_actual] = HIGH;
          senales [prox_senal] = HIGH;
          digitalWrite(pines_senales[senal_actual],HIGH);
          digitalWrite(pines_senales[prox_senal],HIGH);
          ultimo_t0 = micros();
          estado=DISPARANDO;
        }
        break;

      /* Cuando el tiempo ha sobrepasado el ancho del pulso, apaga las señales y continúa con la
         siguiente señal. Si ya ha disparado todas las señales, salga y espera al siguiente cruce por cero
      */
      case DISPARANDO:
        if(actual_t0 - ultimo_t0 > anchoPulso){
          senales [senal_actual] = LOW;
          senales [prox_senal] = LOW;
          digitalWrite(pines_senales[senal_actual],LOW);
          digitalWrite(pines_senales[prox_senal],LOW);
          senal_actual=prox_senal;
          if(senal_actual<5){
            estado = ESPERANDO_SIG_PULSO;
          }else{
            estado = APAGAR;
          }
        }
        break;

      case ESPERANDO_SIG_PULSO:
        if(actual_t0 - ultimo_t0 > dif_pulsos_us){
          senales [senal_actual] = HIGH;
          senales [prox_senal] = HIGH;
          digitalWrite (pines_senales[senal_actual],HIGH);
          digitalWrite (pines_senales[prox_senal],HIGH);
          ultimo_t0 = micros ();
          estado = DISPARANDO;
        }
        break;

      case APAGAR:
        /* Salir */
        if(actual_t0-ultimo_t1<T*1.5){
          estado = ESPERAR_ALFA;
        }else{
          cruce_detectado=false;
        }
        break;

      default:
        // cruce_detectado=false;
        break;
    }
  }

  int valor_S1_S2 = senales[0] | senales[3];
  int valor_S3_S4 = senales[2] | senales[5];
  int valor_S5_S6 = senales[1] | senales[4];

  digitalWrite(S1_S2, valor_S1_S2);
  digitalWrite(S3_S4, valor_S3_S4);
  digitalWrite(S5_S6, valor_S5_S6);


  //====================================================
  #if DEBUG
    Serial.println("=================================");
    // Serial.print("f(Hz): ");
    // Serial.println(f);
    // Serial.print("T(us): ");
    // Serial.println(T);
    // Serial.print("DaG: ");
    // Serial.println(DaG);
    // Serial.print("valor de alfa sensado: ");
    // Serial.println(valorAlfaSensado);
    Serial.print("alfa(°): ");
    Serial.println(alfa);
    Serial.print("alfa(us)): ");
    Serial.println(alfaMicroseconds);
    // Serial.print("anchoPulso(us): ");
    // Serial.println(anchoPulso);
  #endif
}
