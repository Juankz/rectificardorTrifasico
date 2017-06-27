#include <Arduino.h>

#define DEBUG 0

const int pinLed = 13;
const int pinSalida = 3;
const int pinCrucex0 = 2;
const int pinAlfa = A0; //Entrada del potenciómetro que medirá el ángulo alfa
const int f = 60;
const float DaG = float(180)/float(1023); //Digital a Grados
const float DaMicros = float(1)/float(1023); //Digital a microsegundos

float T = (1/(2*(float)f))*1000000; //Un solo semiciclo. us
int valorAlfaSensado; //Alamcena el valor del divisor de voltaje por potenciómetro
int estado = 0;
int alfa = 10; //Valor entre 0 y 180
float alfaMicroseconds; //valor en us
float anchoPulso = 0.1*T; // Valor en us


void cruce(){
  delayMicroseconds(alfaMicroseconds);
  digitalWrite(pinSalida,HIGH);
  delayMicroseconds(anchoPulso);
  digitalWrite(pinSalida,LOW);
}

void setup(){
  #if DEBUG
    Serial.begin(9600);
  #endif
  pinMode(pinSalida,OUTPUT);
  pinMode(pinCrucex0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinCrucex0), cruce, RISING);
}

void loop(){
  valorAlfaSensado = analogRead(pinAlfa);
  #if DEBUG
    alfa = valorAlfaSensado*DaG;
    alfaMicroseconds = T*alfa/180;
  #else
    alfaMicroseconds = T*valorAlfaSensado*DaMicros;
  #endif

  //====================================================
  #if DEBUG
    Serial.println("=================================");
    // Serial.print("f(Hz): ");
    // Serial.println(f);
    // Serial.print("T(us): ");
    // Serial.println(T);
    // Serial.print("DaG: ");
    // Serial.println(DaG);
    Serial.print("valor de alfa sensado: ");
    Serial.println(valorAlfaSensado);
    Serial.print("alfa(°): ");
    Serial.println(alfa);
    Serial.print("alfa(us)): ");
    Serial.println(alfaMicroseconds);
    // Serial.print("anchoPulso(us): ");
    // Serial.println(anchoPulso);
    delay(1000);
  #endif
}
