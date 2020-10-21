//https://forum.arduino.cc/index.php?topic=558692.0
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
<<<<<<< HEAD
  delay(1000);
}
=======
 
    for (int i=0; i<12; i++){         //to request the data from 12 array addresses
        posicaoVetor = i;
        Serial.println("I send");
        Serial.println(posicaoVetor);        //Print the array adcress requested by the master

        Wire.beginTransmission (8);    //Start I2C communication with the slave
        Wire.write (posicaoVetor);                         
        Wire.endTransmission ();       

        delay(10);                  // Adding some delay to avoid error when changing from send to read...  (I tested without it, it's working, but I find safer to add it)

        Wire.requestFrom(8,5);     //request the slave to send the requested Data


        while (Wire.available()>2) {
            t[d] = Wire.read();         // Put the receving data in the array t
            d++;
            if (d>=3){d=0;}             // Return the cursor of array t to 0 when we receive the 3 wanted bytes
        }

        Serial.println("Received Data: ");
        Serial.println(t);          // Received ASII Data

        receivedData[posicaoVetor]=((((((t[0])-48)*10)+((t[1])-48))*10)+((t[2])-48));  //Convert ASII to DECIMAL to store in a int.      In ASII 0 is 48, 1 is 49, 2 is 50.... so if we sustrac 48 to the ASII number we get an DECIMAL number
        Serial.println(receivedData[posicaoVetor]);  //Print the received Data
    } // end of for loop


    Serial.print("\r\n");  //Skip one line
    Serial.println("All Received Data: ");  //Print all the received Data
    
    for (int i=0; i<12; i++){
        Serial.println(receivedData[i]);
    }   //Display all the data received
    
    while(1); //rodar uma vez
 

}   //end of void loop ()
>>>>>>> 556af56b210a5bbda2f3b43c957abbd7c35d8cc6
