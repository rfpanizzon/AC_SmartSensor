#define TOVOLTAGE 5 / 1023.0        // Para o Arduino, o final de escala padrao eh 5,0V e o conversor eh de 10 bits
#define SERIAL_BAUDRATE 9600        // esta eh a taxa de transmissao de dados usual para Arduino
#define AMOSTRAS 64                 // eh possivel varias este numero de amostras (N). Para Arduino UNO, o maximo eh 147
                                    // quando selecionar o numero de amostras, cuide para a variavel DELAY deve se manter positiva 
#define FREQUENCIA 60               // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_CORRENTE 0.57  // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico
#define PIN_A A0                    // Pino do conversor analogico/digital escolhido para medir a corrente. Pode ser modificado
#define PRINT_VETOR 0               // "0"(zero) mostra o valor eficaz na serial e "1" mostra os dados na serial e a forma de onda na plotter


float corrente[AMOSTRAS];           // vetor com os dados instantaneos (N AMOSTRAS)da forma de onda da corrente
float DELAY;                        // DELAY necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
float TEMPO_CONVERSAO_N_AMOSTRAS;   
float PERIODO = (1.0/FREQUENCIA);
String readString;

#include <arduinoFFT.h>
arduinoFFT FFT = arduinoFFT();
double vReal[AMOSTRAS];
double vImag[AMOSTRAS];

void setup()
{
  pinMode(PIN_A, INPUT);
  Serial.begin(SERIAL_BAUDRATE);
  //Serial.println("Serial initt...");
  //Serial.println("Set: ");
  //Serial.print("\tN AMOSTRAS: "); Serial.println(AMOSTRAS);
  //Serial.print("\tFrequencia do Sinal: "); Serial.print(FREQUENCIA); Serial.println(" Hz");
  //Serial.print("\tPeriodo do Sinal: "); Serial.print(PERIODO,4);Serial.println(" segundos");
  
  TEMPO_CONVERSAO_N_AMOSTRAS = micros();
  for (int i = 0; i < AMOSTRAS; i++) //primeira conversao para calibração do AD e obter o tempo necessário para converter N AMOSTRAS
  {
    corrente[i] = analogRead(PIN_A);
  }
  TEMPO_CONVERSAO_N_AMOSTRAS = micros() - TEMPO_CONVERSAO_N_AMOSTRAS;
  // O Arduino converte N amostras em um determinado tempo (milisegundos). Eh preciso fazer um DELAY para ter um N AMOSTRAS em um ciclo completo
  DELAY = (((1000000*PERIODO) - TEMPO_CONVERSAO_N_AMOSTRAS) / AMOSTRAS);    // 1000000 eh para passar o PERIODO para microsegundos
  // Observe que DELAY esta em microsegundos e que vc precisa cuidar para fazer N AMOSTRAS EM UM CICLO DE 60 OU 50Hz
  // DELAY nao pode ser negativo
  //Serial.print("\tDELAY NAO PODE SER NEGATIVO =  ");Serial.print(DELAY, 4);Serial.println("  microsegundos para 64 amostras");
}

void loop()
{
  while (Serial.available()) // this will be skipped if no data present, leading to
                             // the code sitting in the delay function below
  {
    delay(30);  //delay to allow buffer to fill 
    if (Serial.available() >0)
    {
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    }
  }
  }
  if (readString == "3")
  {
    for(int j = 0; j < 10; j++)
    {
        //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE CORRENTE
        for (int i = 0; i < AMOSTRAS; i++)
        {
        vReal[i] = analogRead(PIN_A);
        vImag[i] = 0;
        delayMicroseconds(DELAY);
        }
        //FIM DA AQUISIÇÃO

        float media_aritmetica_corrente = 0;
        for (int i = 0; i < AMOSTRAS; i++)
        {
            media_aritmetica_corrente += vReal[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + vReal[i];
        }
        media_aritmetica_corrente /= AMOSTRAS;
        //FIM CÁLCULO MÉDIA ARITMÉTICA
        //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
        for (int i = 0; i < AMOSTRAS; i++)
        {
            vReal[i] -= media_aritmetica_corrente;
        }
        //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA

        /*FFT*/
        FFT.Windowing(vReal, AMOSTRAS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
        FFT.Compute(vReal, vImag, AMOSTRAS, FFT_FORWARD);
        FFT.ComplexToMagnitude(vReal, vImag, AMOSTRAS);
        double peak = FFT.MajorPeak(vReal, AMOSTRAS, FREQUENCIA);
        
        /*PRINT RESULTS*/
        //Serial.println(peak);     //Print out what frequency is the most dominant.

        for(int i=0; i<(AMOSTRAS/2); i++)
        {
            /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/
            
            //EIXO X DO GRAF PARA O PYTHON
            Serial.println((i * 1.0 * AMOSTRAS/2));
            //Serial.print(" ");
            Serial.println(vReal[i]);    //View only this line in serial plotter to visualize the bins
        }

        //delay(1000);  //Repeat the process every second OR:
    }
    readString = "0";
  }
}