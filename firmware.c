// Editado por Rafael Panizzon em 07Out2020: Captura de 10 ciclos de 60hz mostrando os valores medio e rms
// Editado por prof. Spalding em 09Set2020:
// Para usar com Arduino UNO, ESP32 e outros: Programa adaptado por Carlos Re Signor em Setembro de 2018 para capturar as formas
// de onda do sensor de corrente Elotod.
// O programa calcula o valor eficaz da corrente e mostra o valor na saida serial e a forma de onda na saide serial modo plotter
// MUITO CUIDADO AO USAR 220 V. Não faça este experimento sem um adulto por perto, além de você. Ensine ele a desligar a energia eletrica.
// Este firmware eh usado nos videos 2 e 3 da pagina da Elotod no facebook: https://www.facebook.com/elotod

#if defined(ESP8266) || defined(ESP31B) || defined(ESP32)
#define TOVOLTAGE 3.3 / 1023.0      // o projetista pode escolher o valor de final de escala e o valor de bits do conversor
#define SERIAL_BAUDRATE 115200      // esta eh a taxa de transmissao de dados usual para ESP32
#else

#define TOVOLTAGE 5 / 1023.0        // Para o Arduino, o final de escala padrao eh 5,0V e o conversor eh de 10 bits
#define SERIAL_BAUDRATE 9600        // esta eh a taxa de transmissao de dados usual para Arduino
#endif
#include <arduinoFFT.h>
#define AMOSTRAS 64                 // eh possivel varias este numero de amostras (N). Para Arduino UNO, o maximo eh 147
                                    // quando selecionar o numero de amostras, cuide para a variavel DELAY deve se manter positiva 
#define FREQUENCIA 60               // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_CORRENTE 0.57  // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico
#define PIN_A A5                    // Pino do conversor analogico/digital escolhido para medir a corrente. Pode ser modificado
#define PRINT_VETOR 0               // "0"(zero) mostra o valor eficaz na serial e "1" mostra os dados na serial e a forma de onda na plotter

arduinoFFT FFT = arduinoFFT();
float corrente[AMOSTRAS];           // vetor com os dados instantaneos (N AMOSTRAS)da forma de onda da corrente
float DELAY;                        // DELAY necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
float TEMPO_CONVERSAO_N_AMOSTRAS;   
float PERIODO = (1.0/FREQUENCIA);

double correnteReal[AMOSTRAS];
double correnteImag[AMOSTRAS];

// vetores com tamanho 10, para guardar os dados de 10 ciclos
float VALORES_CORRENTE_RMS[10];
float VALORES_MEDIA_ARITMETICA[10];
int cont_valores = 0;

void setup()
{
  pinMode(PIN_A, INPUT);
  Serial.begin(SERIAL_BAUDRATE);
  Serial.println("Serial initt...");
  Serial.println("Set: ");
  Serial.print("\tN AMOSTRAS: "); Serial.println(AMOSTRAS);
  Serial.print("\tFrequencia do Sinal: "); Serial.print(FREQUENCIA); Serial.println(" Hz");
  Serial.print("\tPeriodo do Sinal: "); Serial.print(PERIODO,4);Serial.println(" segundos");
  
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
 Serial.print("\tDELAY NAO PODE SER NEGATIVO =  ");Serial.print(DELAY, 4);Serial.println("  microsegundos");
}
void loop()
{
  //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE CORRENTE
  for (int i = 0; i < AMOSTRAS; i++)
  {
    correnteReal[i] = corrente[i] = analogRead(PIN_A);
    correnteImag[i] = 0;
    delayMicroseconds(DELAY);
  }
  //FIM DA AQUISIÇÃO

  // "1" eh a opcao para ver a forma de onda, sugiro comentar todos os "Serial.print" que estao fora do #if PRINT_VETOR == 1 abaixo.
#if PRINT_VETOR == 1
  for (int i = 0; i < AMOSTRAS; i++)
  {
    Serial.println(corrente[i]);
    Serial.print(" ");
  }
 while (1);               // voce pode eliminar esta linha ou comentar para que a visualização da forma de onda seja constante                       
#endif
  //CÁLCULO DA MÉDIA ARITMÉTICA DA TENSÃO E CORRENTE. Eh importante calcular o valor medio das amostras para usar na sequencia e eliminar
  // o nivel DC da forma de onda, caso sua corrente seja simetrica. A variação de temperatura no sensor pode fazer o nivel DC variar
  float media_aritmetica_corrente = 0;
  for (int i = 0; i < AMOSTRAS; i++)
  {
    media_aritmetica_corrente += corrente[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
  }
  media_aritmetica_corrente /= AMOSTRAS;
  //FIM CÁLCULO MÉDIA ARITMÉTICA
  //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
  for (int i = 0; i < AMOSTRAS; i++)
  {
    corrente[i] -= media_aritmetica_corrente;
  }
  //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
  
  //CALULO DO VALOR EFICAZ CORRENTE(RMS)
  //valor_eficaz = sqrt(sum(amostra[i]^2) / numero de amostras)
  float soma_quadrados_corrente = 0;
  for (int i = 0; i < AMOSTRAS; i++)
  {
    soma_quadrados_corrente += corrente[i] * corrente[i];
  }
  float rms_corrente = sqrt(soma_quadrados_corrente / AMOSTRAS) * TOVOLTAGE; // ajuste para ficar na escala de (0 a 5.0 ou a 3.3)Volts
  // FIM CÁLCULO VALOR RMS
  
  // aplicar o ganho para obter o valor correto e calibrado da corrente eletrica. 
  // O valor do ganho esta impresso na etiqueta do sensor de corrente Elotod
  // variável VALORES_CORRENTE_RMS para salvar 10 ciclos do valor da corrente eficaz (RMS)
  VALORES_MEDIA_ARITMETICA[cont_valores] = media_aritmetica_corrente;
  VALORES_CORRENTE_RMS[cont_valores] = rms_corrente * GANHO_SENSOR_CORRENTE;
  cont_valores++;

  /*FFT*/
  FFT.Windowing(correnteReal, AMOSTRAS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(correnteReal, correnteImag, AMOSTRAS, FFT_FORWARD);
  FFT.ComplexToMagnitude(correnteReal, correnteImag, AMOSTRAS);
  double peak = FFT.MajorPeak(correnteReal, AMOSTRAS, SAMPLING_FREQUENCY);

  
  // laço for para printar os valores dos 10 ciclos
  if(cont_valores == 9)
  {
    Serial.print("\tValor medio 10 ciclos: ");
    for (int i = 0; i < 10; i++)
    {
      Serial.print(VALORES_MEDIA_ARITMETICA[i], 3);
      Serial.print(" | ");
    }

    Serial.print("\tCorrente Eficaz 10 ciclos: ");
    for (int i = 0; i < 10; i++)
    {
      Serial.print(VALORES_CORRENTE_RMS[i], 3);
      Serial.print(" (Arms) | ");
    }
    cont_valores = 0;

    // soh pra ver se funcionou deixar travado pra ver os valores no monitor serial
    while (1);
  }

  delay(1000);
}