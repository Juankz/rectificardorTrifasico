#include <Arduino.h>

const int pinLed = 13;
const int pinSalida = 3;
const int pinCrucex0 = 2;
const int f = 60;

float T = (1/(2*(float)f))*100000; //Un solo semiciclo. us
int estado = 0;
int alpha = 10; //Valor entre 0 y 180
float alphaMicroseconds = T*alpha/180; //valor en us
float anchoPulso = 0.1*T; // Valor en us


void cruce(){
  delay(alphaMicroseconds);
  digitalWrite(pinSalida,HIGH);
  delayMicroseconds(anchoPulso);
  digitalWrite(pinSalida,LOW);
}

void setup(){
  Serial.begin(9600);
  pinMode(pinSalida,OUTPUT);
  pinMode(pinCrucex0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinCrucex0), cruce, RISING);
}

void loop(){
  Serial.print("f: ");
  Serial.println(f);
  Serial.print("T: ");
  Serial.println(T);
  Serial.print("alpha: ");
  Serial.println(alpha);
  Serial.print("alphaMicroseconds: ");
  Serial.println(alphaMicroseconds);
  Serial.print("anchoPulso: ");
  Serial.println(anchoPulso);
  delay(1000);
}
