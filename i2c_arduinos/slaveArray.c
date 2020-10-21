#include <Wire.h>

#define sizeArray = 3;
float array_float[sizeArray];
byte byte1, byte2, byte3, byte4;
unsigned int aux[sizeArray];
int quantidade_bytes_esperados = 4;

void setup() 
{
  Serial.begin(9600);
  Wire.begin(44);                                   // Inicia como escravo endereço 44
  Wire.onReceive(receiveEvent);                     // Interrupção ativada quando  mestre está enviado os bytes 
}

void loop() 
{
  delay(1000);
}

void receiveEvent(int quantidade_bytes_esperados) 
{ // Está código é executado quando "quantidade_bytes_esperados" foi recebido via I2C
  
    for (int i=0; i<sizeArray; i++)
    {
        // Lê os 4 bytes enviados pelo mestre
        byte1 = Wire.read();
        byte2 = Wire.read(); 
        byte3 = Wire.read(); 
        byte4 = Wire.read(); 

        // Ajustando os bytes recebidos para obetr a array_float
        aux[i] = (byte3<<8) | byte4;                       // Ajusta a parte fracionáia (depois da vírgula)
        array_float[i] = (float) (aux[i]*0.0001);          // Atribui a parte fracionária, depois da vírgula 
        aux[i] = (byte1<<8) | byte2;                       // Ajusta a parte inteira (antes da vírgula)
        array_float[i] += aux[i];                          // Atribui a parte iteira
        Serial.println("Variavel recebida pelo escravo:");
        Serial.println(array_float[i], DEC);  
    }
}