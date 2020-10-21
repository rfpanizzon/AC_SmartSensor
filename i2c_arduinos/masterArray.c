#include <Wire.h>

#define sizeArray = 3;
float array_float[sizeArray] = {46689.8750, 423122.8430, 12344.5555};

byte byte1, byte2, byte3, byte4;
unsigned int aux[sizeArray];

void setup() 
{
  Serial.begin(9600);
}

void loop() {
  i2cArrayFloat();
  delay(1000);
  while(1);
}

void i2cArrayFloat()
{
  Wire.begin();

  // Para variáveis float positivas
  // Ajustando o número antes da vírgula
  for (int i=0; i<sizeArray; i++)
  {
    aux[i] = (unsigned int) array_float[i];  // aux = 46689, Pega somente a parte inteira da variável float (0 - 65536)
    byte2 = aux[i];                          // byte2 = 0B01100001, pega apenas os primeros 8 bits
    byte1 = (aux[i]>>8);                     // byte1 = 0B10110110, pega os 8 ultimos bits

    // Ajustando o número depois da vírgula
    array_float[i] -= aux[i];                // Deixa apenas o número depois da vírgula
    array_float[i] *= 10000;                 // Multiplica por 10k para pegar 4 dígitos após a vírgula
    aux[i] = (unsigned int) array_float[i];  // Pega somente o valor antes da vírgula
    byte4 = aux[i];                          // byte2 = 0B00101110, pega apenas os primeros 8 bits
    byte3 = (aux[i]>>8);                     // byte1 = 0B00100010, pega os 8 ultimos bits

    Wire.beginTransmission(44);           // Começa transmissão para o escravo 0x2C
    Wire.write(byte1);                    // Envia os bytes do número antes da vírgua e depois da vírgula
    Wire.write(byte2); 
    Wire.write(byte3);
    Wire.write(byte4);  
  }
  Wire.endTransmission();               // Termina a transmissão 
}