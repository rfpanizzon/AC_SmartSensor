// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

float variavel_float;
byte byte1, byte2, byte3, byte4;
unsigned int aux;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  Wire.requestFrom(8, 4);   // request 4 bytes from slave device #8

  while (Wire.available()) 
  {                         // slave may send less than requested
    byte1 = Wire.read();    // Lê os 4 bytes enviados pelo mestre 
    //byte2 = Wire.read(); 
    //byte3 = Wire.read(); 
    //byte4 = Wire.read(); 

    // Ajustando os bytes recebidos para obetr a variavel_float
    aux = (byte3<<8) | byte4;                     // Ajusta a parte fracionáia (depois da vírgula)
    variavel_float = (float) (aux*0.0001);        // Atribui a parte fracionária, depois da vírgula 
    aux = (byte1<<8) | byte2;                     // Ajusta a parte inteira (antes da vírgula)
    variavel_float += aux;                        // Atribui a parte iteira
    Serial.println("Variavel recebida pelo escravo:");
    Serial.println(variavel_float, DEC);  
  }
  delay(3000);
}
