// Wire Slave Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
#define myAdress 0x08

float variavel_float = 46689.8750;
byte byte1, byte2, byte3, byte4;
unsigned int aux;

void setup() {
  Serial.begin(9600);
  Wire.begin(myAdress);         // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  // Para variáveis float positivas
  // Ajustando o número antes da vírgula
  aux = (unsigned int) variavel_float;  // aux = 46689, Pega somente a parte inteira da variável float (0 - 65536)
  byte2 = aux;                          // byte2 = 0B01100001, pega apenas os primeros 8 bits
  byte1 = (aux>>8);                     // byte1 = 0B10110110, pega os 8 ultimos bits
  
  // Ajustando o número depois da vírgula
  variavel_float -= aux;                // Deixa apenas o número depois da vírgula
  variavel_float *= 10000;              // Multiplica por 10k para pegar 4 dígitos após a vírgula
  aux = (unsigned int) variavel_float;  // Pega somente o valor antes da vírgula
  byte4 = aux;                          // byte2 = 0B00101110, pega apenas os primeros 8 bits
  byte3 = (aux>>8);                     // byte1 = 0B00100010, pega os 8 ultimos bits

  //Wire.beginTransmission(myAdress);     // Começa transmissão para o escravo 0x2C
  Wire.write(byte1);                    // Envia os bytes do número antes da vírgua e depois da vírgula
  //Wire.write(byte2); 
  //Wire.write(byte3);
  //Wire.write(byte4);  
  //Wire.endTransmission();               // Termina a transmissão 
}