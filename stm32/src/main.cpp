#include <Arduino.h>
#include <arduinoFFT.h>

#define TOVOLTAGE 3.3 / 4096.0          // Para o STM32 BLUEPILL, o final de escala padrao eh 3,3V e o conversor eh de 12 bits
#define SERIAL_BAUDRATE 115200          // Esta eh a taxa de transmissao de dados usual para STM32 BLUEPILL
#define AMOSTRAS 64                     // Eh possivel varias este numero de amostras (N).

#define TOTAL_CICLOS 512                // 8 ciclos com 64 amostras

#define FREQUENCIA 60                   // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_CORRENTE 0.57      // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico

#define PIN_A PA0                       // Pino do conversor analogico/digital escolhido para medir a corrente. Pode ser modificado
#define PIN_LED_INTERNO PC13            // Led interno do stm, definir como saida

float corrente[TOTAL_CICLOS];       // Vetor com os dados instantaneos (N AMOSTRAS) da forma de onda da corrente
float DELAY;                        // DELAY necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
float TEMPO_CONVERSAO_N_AMOSTRAS;   
float PERIODO = (1.0/FREQUENCIA);

String CMD_DESKTOP;                 // String que recebe os comando do desktop

arduinoFFT FFT = arduinoFFT();
double vReal[TOTAL_CICLOS];
double vImag[TOTAL_CICLOS];

void setup() {
  pinMode(PIN_LED_INTERNO, OUTPUT);
  pinMode(PIN_A, INPUT);

  Serial.begin(SERIAL_BAUDRATE);
  while(!Serial);

  // Serial.println("Serial initt...");
  // Serial.println("Set: ");
  // Serial.print("\tN AMOSTRAS: "); Serial.println(AMOSTRAS);
  // Serial.print("\tFrequencia do Sinal: "); Serial.print(FREQUENCIA); Serial.println(" Hz");
  // Serial.print("\tPeriodo do Sinal: "); Serial.print(PERIODO,4);Serial.println(" segundos");

  TEMPO_CONVERSAO_N_AMOSTRAS = micros();
  for (int i = 0; i < AMOSTRAS; i++)      //Primeira conversao para calibração do AD e obter o tempo necessário para converter N AMOSTRAS
  {
    corrente[i] = analogRead(PIN_A);
  }
  TEMPO_CONVERSAO_N_AMOSTRAS = micros() - TEMPO_CONVERSAO_N_AMOSTRAS;
  // O STM converte N amostras em um determinado tempo (milisegundos). Eh preciso fazer um DELAY para ter um N AMOSTRAS em um ciclo completo

  // 1000000 eh para passar o PERIODO para microsegundos
  // Observe que DELAY esta em microsegundos e que vc precisa cuidar para fazer N AMOSTRAS EM UM CICLO DE 60 OU 50Hz
  DELAY = (((1000000*PERIODO) - TEMPO_CONVERSAO_N_AMOSTRAS) / AMOSTRAS);    

  //DELAY nao pode ser negativo
  //Serial.print("\tDELAY NAO PODE SER NEGATIVO =  "); Serial.print(DELAY, 4); Serial.println("  microsegundos para 64 amostras");
}

void loop() {
  delay(5000); //O arduino framework para stm é meio bugado, precisa de delay para iniciar corretamente a serial

  while (Serial.available())      // This will be skipped if no data present, leading to                                  
  {                               // The code sitting in the delay function below
    delay(30);                    // Delay to allow buffer to fill 
    if (Serial.available() >0)
    {
      char c = Serial.read();     // Gets one byte from serial buffer
      CMD_DESKTOP += c;           // Makes the string CMD_DESKTOP
    }
  }

  if (CMD_DESKTOP == "1")
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE 10 CLICLOS = 640 AMOSTRAS DO SENSOR DE CORRENTE
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO
    
    //CALCULO MEDIA ARITMETICA
    float media_aritmetica_corrente = 0;
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      media_aritmetica_corrente += corrente[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
    }
    media_aritmetica_corrente /= TOTAL_CICLOS;
    //FIM CALCULO MEDIA ARITMETICA
    
    /*
    Serial.print(media_aritmetica_corrente);
    */
    
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
      corrente[i] -= media_aritmetica_corrente;
      //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA

      //PARA DEIXAR O GRAFICO NA ESCALA CORRETA
      corrente[i] = corrente[i] * GANHO_SENSOR_CORRENTE * TOVOLTAGE;
      Serial.println(corrente[i]);
    }
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "2")
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE 10 CLICLOS = 640 AMOSTRAS DO SENSOR DE CORRENTE
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO
    
    //CALCULO MEDIA ARITMETICA
    float media_aritmetica_corrente = 0;
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      media_aritmetica_corrente += corrente[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
    }
    media_aritmetica_corrente /= TOTAL_CICLOS;
    //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    /*
    Serial.println(media_aritmetica_corrente);
    */
    
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] -= media_aritmetica_corrente;
    }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
    
    //CALULO DO VALOR EFICAZ CORRENTE(RMS)
    //valor_eficaz = sqrt(sum(amostra[i]^2) / numero de amostras)
    
    float soma_quadrados_corrente = 0;
    int i = 0;
    int aux = 0;

    //calcula o valor da corrente rms para cada ciclo
    for (int j = 0; j < 10; j++)
    {
      for (i; i < (aux + 64); i++)
      {
        soma_quadrados_corrente += corrente[i] * corrente[i];
      }
      float rms_corrente = sqrt(soma_quadrados_corrente / AMOSTRAS) * TOVOLTAGE;     // ajuste para ficar na escala de (0 a 5.0 ou a 3.3)Volts
      Serial.println(rms_corrente * GANHO_SENSOR_CORRENTE, 3);
      
      aux = i;
      soma_quadrados_corrente = 0;
    }
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "3")
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE CORRENTE
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      vReal[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO

    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      vImag[i] = 0.0;
    }
          
    //CALCULO MEDIA ARITMETICA
    float media_aritmetica_corrente = 0;
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      media_aritmetica_corrente += vReal[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
    }
    media_aritmetica_corrente /= TOTAL_CICLOS;
    //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      vReal[i] -= media_aritmetica_corrente;
    }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA

    FFT.Windowing(vReal, TOTAL_CICLOS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, TOTAL_CICLOS, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, TOTAL_CICLOS);

    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      Serial.println(vReal[i]);
      Serial.println(vImag[i]);
    }
    CMD_DESKTOP = "0";
  }
}