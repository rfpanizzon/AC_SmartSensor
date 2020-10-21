#include <Wire.h>

float variavel_float = 46689.8750;

byte byte1, byte2, byte3, byte4;
unsigned int aux;

void setup() {
  delay(500);                           // Espera o escravo se estabilizar
  Serial.begin(9600);
  Wire.begin();

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

  Wire.beginTransmission(44);           // Começa transmissão para o escravo 0x2C
  Wire.write(byte1);                    // Envia os bytes do número antes da vírgua e depois da vírgula
  Wire.write(byte2); 
  Wire.write(byte3);
  Wire.write(byte4);  
  Wire.endTransmission();               // Termina a transmissão 
}

void loop() {
  delay(1000);
}