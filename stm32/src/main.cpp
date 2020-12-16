#include <Arduino.h>
#include <arduinoFFT.h>

#define TOVOLTAGE 3.3 / 4096.0          // Para o STM32 BLUEPILL, o final de escala padrao eh 3,3V e o conversor eh de 12 bits
#define SERIAL_BAUDRATE 115200          // Esta eh a taxa de transmissao de dados usual para STM32 BLUEPILL
#define AMOSTRAS 64                     // Eh possivel varias este numero de amostras (N).

#define TOTAL_AMOSTRAS 512             // 8 ciclos com 64 amostras

#define FREQUENCIA 60                   // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_CORRENTE 0.57      // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico
#define FS 3840
#define PIN_A PA0                       // Pino do conversor analogico/digital escolhido para medir a vReal. Pode ser modificado
#define PIN_LED_INTERNO PC13            // Led interno do stm, definir como saida

unsigned long DELAY = 195;              // DELAY em us necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
unsigned long TEMPO_CONVERSAO_N_AMOSTRAS;   

String CMD_DESKTOP;                     // String que recebe os comando do desktop

arduinoFFT FFT = arduinoFFT();
double vReal[TOTAL_AMOSTRAS];
double vImag[TOTAL_AMOSTRAS];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void LerAmostras()
{
  //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE vReal
  for (int i = 0; i < TOTAL_AMOSTRAS; i++)
  {
    vReal[i] = analogRead(PIN_A);
    delayMicroseconds(DELAY);
  }
  // //FIM DA AQUISIÇÃO
}

void Escala()
{
  //CALCULO MEDIA ARITMETICA
  float media_aritmetica_vReal = 0;
  for (int i = 0; i < TOTAL_AMOSTRAS; i++)
  {
    media_aritmetica_vReal += vReal[i];     //equivale a: media_aritmetica_vReal = media_aritmetica_vReal + vReal[i];
  }
  media_aritmetica_vReal /= TOTAL_AMOSTRAS;
  //FIM CALCULO MEDIA ARITMETICA
      
  //Serial.print(media_aritmetica_vReal);
  
  for (int i = 0; i < TOTAL_AMOSTRAS; i++)
  {
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA vReal ELETRICA
    vReal[i] -= media_aritmetica_vReal;
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
    
    //PARA DEIXAR O GRAFICO NA ESCALA CORRETA
    vReal[i] = vReal[i] * GANHO_SENSOR_CORRENTE * TOVOLTAGE;
  }
}

void PrintVector(double *vData, int bufferSize, uint8_t scaleType)
{
  for (int i = 0; i < bufferSize; i++)
  {
    double abscissa;
    
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
	      break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / FS);
	      break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * FS) / AMOSTRAS);
	      break;
    }

    Serial.println(abscissa, 6);

    // if(scaleType==SCL_FREQUENCY) 
    //   Serial.print("Hz");

    // Serial.print(" ");
    Serial.println(vData[i], 4);
    }
  //Serial.println();
}

void setup() {
  pinMode(PIN_LED_INTERNO, OUTPUT);
  pinMode(PIN_A, INPUT);

  Serial.begin(SERIAL_BAUDRATE);
  analogReadResolution(12);
  while(!Serial);
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

  if (CMD_DESKTOP == "1") //Forma de Onda Dados Brutos
  {
    LerAmostras();

    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      Serial.println(vReal[i]);
    }

    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "2") //Forma de Onda Ajustada
  {
    LerAmostras();
    Escala();

    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      Serial.println(vReal[i], 6);
    }

    CMD_DESKTOP = "0";
  }
  
  if (CMD_DESKTOP == "3") //Valor RMS da vReal em cada ciclo
  {
    LerAmostras();

    //CALCULO MEDIA ARITMETICA
    long double media_aritmetica_vReal = 0;
    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      media_aritmetica_vReal += vReal[i];     //equivale a: media_aritmetica_vReal = media_aritmetica_vReal + vReal[i];
    }
    media_aritmetica_vReal /= TOTAL_AMOSTRAS;
    //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    //Serial.println(media_aritmetica_vReal);
    
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA vReal ELETRICA
    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      vReal[i] -= media_aritmetica_vReal;
    }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
    
    //CALULO DO VALOR EFICAZ vReal(RMS)
    //valor_eficaz = sqrt(sum(amostra[i]^2) / numero de amostras)
    
    double soma_quadrados_vReal = 0;
    int i = 0;
    int aux = 0;

    //calcula o valor da vReal rms para cada ciclo
    for (int j = 0; j < 8; j++)
    {
      for (i; i < (aux + 64); i++)
      {
        soma_quadrados_vReal += vReal[i] * vReal[i];
      }
      double rms_vReal = sqrt(soma_quadrados_vReal / AMOSTRAS) * TOVOLTAGE;     // ajuste para ficar na escala de (0 a 5.0 ou a 3.3)Volts
      Serial.println(rms_vReal * GANHO_SENSOR_CORRENTE, 6);
      
      aux = i;
      soma_quadrados_vReal = 0;
    }
    
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "4") //Valor RMS de todos os ciclos
  {
    LerAmostras();
    
    //CALCULO MEDIA ARITMETICA
    double media_aritmetica_vReal = 0;
    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      media_aritmetica_vReal += vReal[i];     //equivale a: media_aritmetica_vReal = media_aritmetica_vReal + vReal[i];
    }
    media_aritmetica_vReal /= TOTAL_AMOSTRAS;
    //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    //Serial.println(media_aritmetica_vReal);
    
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA vReal ELETRICA
    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      vReal[i] -= media_aritmetica_vReal;
    }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
    
    //CALULO DO VALOR EFICAZ vReal(RMS)
    //valor_eficaz = sqrt(sum(amostra[i]^2) / numero de amostras)
    
    long double soma_quadrados_vReal = 0;

    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      soma_quadrados_vReal += vReal[i] * vReal[i];
    }
    double rms_vReal = sqrt(soma_quadrados_vReal / TOTAL_AMOSTRAS) * TOVOLTAGE;     // ajuste para ficar na escala de (0 a 5.0 ou a 3.3)Volts
    Serial.println(rms_vReal * GANHO_SENSOR_CORRENTE, 6);
  
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "5")
  {
    LerAmostras();

    for (int i = 0; i < AMOSTRAS; i++)
    {
      vImag[i] = 0.0;
    }
          
    //CALCULO MEDIA ARITMETICA
    float media_aritmetica_vReal = 0;
    for (int i = 0; i < AMOSTRAS; i++)
    {
      media_aritmetica_vReal += vReal[i];     //equivale a: media_aritmetica_vReal = media_aritmetica_vReal + vReal[i];
    }
    media_aritmetica_vReal /= AMOSTRAS;
    //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA vReal ELETRICA
    for (int i = 0; i < AMOSTRAS; i++)
    {
      vReal[i] -= media_aritmetica_vReal;
      vReal[i] = vReal[i] * TOVOLTAGE * GANHO_SENSOR_CORRENTE;
    }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA

    //Serial.println("Data:");
    //PrintVector(vReal, TOTAL_AMOSTRAS, SCL_TIME);

    FFT.Windowing(vReal, AMOSTRAS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    //Serial.println("Weighed data:");
    // PrintVector(vReal, TOTAL_AMOSTRAS, SCL_TIME);

    FFT.Compute(vReal, vImag, AMOSTRAS, FFT_FORWARD);
    //Serial.println("Computed Real values:");
    //PrintVector(vReal, TOTAL_AMOSTRAS, SCL_INDEX);
    // Serial.println("Computed Imaginary values:");
    // PrintVector(vImag, TOTAL_AMOSTRAS, SCL_INDEX);

    FFT.ComplexToMagnitude(vReal, vImag, AMOSTRAS);
    PrintVector(vReal, (AMOSTRAS >> 1), SCL_FREQUENCY);

    CMD_DESKTOP = "0";
  }
}
