#include <Arduino.h>
#include <arduinoFFT.h>

#define TOVOLTAGE 3.3 / 4096.0          // Para o STM32 BLUEPILL, o final de escala padrao eh 3,3V e o conversor eh de 12 bits
#define SERIAL_BAUDRATE 115200          // Esta eh a taxa de transmissao de dados usual para STM32 BLUEPILL
#define AMOSTRAS 64                     // Eh possivel varias este numero de amostras (N).

#define TOTAL_CICLOS 512                // 8 ciclos com 64 amostras

#define FREQUENCIA 60                   // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_CORRENTE 0.57      // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico
#define FS 5120 
#define PIN_A PA0                       // Pino do conversor analogico/digital escolhido para medir a corrente. Pode ser modificado
#define PIN_LED_INTERNO PC13            // Led interno do stm, definir como saida

double corrente[TOTAL_CICLOS];        // Vetor com os dados instantaneos (N AMOSTRAS) da forma de onda da corrente
int DELAY = 195;                          // DELAY necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
float TEMPO_CONVERSAO_N_AMOSTRAS;   
float PERIODO = (1.0/FREQUENCIA);

String CMD_DESKTOP;                 // String que recebe os comando do desktop

arduinoFFT FFT = arduinoFFT();
double vReal[TOTAL_CICLOS];         //amostras para teste = {1451,1458,1468,1476,1487,1497,1509,1519,1529,1540,1550,1560,1572,1584,1598,1610,1622,1637,1651,1666,1679,1694,1710,1726,1742,1758,1775,1789,1806,1824,1842,1860,1877,1896,1914,1927,1949,1968,1987,2004,2023,2040,2060,2078,2094,2111,2126,2140,2155,2166,2178,2186,2201,2222,2243,2261,2276,2291,2308,2324,2341,2357,2372,2386,2402,2418,2431,2449,2464,2478,2494,2507,2520,2532,2543,2552,2560,2569,2577,2582,2591,2598,2606,2614,2623,2631,2639,2647,2654,2661,2670,2676,2683,2692,2697,2699,2703,2706,2711,2712,2714,2715,2718,2719,2719,2720,2722,2722,2723,2724,2724,2724,2725,2726,2725,2727,2727,2726,2727,2726,2724,2721,2714,2707,2700,2690,2678,2667,2656,2645,2635,2624,2615,2604,2594,2582,2569,2558,2541,2531,2517,2502,2487,2472,2458,2441,2426,2410,2393,2379,2362,2344,2322,2309,2291,2272,2254,2237,2219,2201,2181,2164,2145,2128,2109,2091,2074,2056,2041,2025,2012,1997,1982,1970,1956,1947,1936,1915,1895,1875,1858,1843,1828,1814,1797,1783,1768,1755,1740,1724,1709,1693,1680,1665,1649,1638,1633,1617,1607,1600,1592,1584,1577,1568,1562,1553,1546,1539,1528,1522,1514,1505,1499,1492,1484,1477,1471,1466,1461,1458,1453,1452,1450,1447,1446,1445,1443,1442,1440,1439,1441,1438,1439,1438,1438,1436,1437,1435,1436,1435,1433,1437,1438,1440,1442,1449,1457,1467,1476,1486,1496,1509,1519,1530,1539,1549};
double vImag[TOTAL_CICLOS];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void PrintVector(double *vData, int bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
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
        abscissa = ((i * 1.0 * FS) / TOTAL_CICLOS);
	break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}

void setup() {
  pinMode(PIN_LED_INTERNO, OUTPUT);
  pinMode(PIN_A, INPUT);

  Serial.begin(SERIAL_BAUDRATE);
  while(!Serial);
}

void loop() {
  delay(4000); //delay pra n buga a serial

  //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE CORRENTE
  for (int i = 0; i < TOTAL_CICLOS; i++)
  {
    vReal[i] = analogRead(PIN_A);
    delayMicroseconds(DELAY);
  }
  //FIM DA AQUISIÇÃO


  // PARA VISUZALIZAR A FORMA DE ONDA NO PLOTTER SERIAL DEIXE SOMENTE O PRINT DESETE FOR
  // for (int i = 0; i < TOTAL_CICLOS; i++) 
  // {
  //   Serial.println(vReal[i]);
  // }

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
  
  //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA e deixar na escala
  for (int i = 0; i < TOTAL_CICLOS; i++)
  {
    vReal[i] -= media_aritmetica_corrente;
    vReal[i] = vReal[i] * TOVOLTAGE * GANHO_SENSOR_CORRENTE;
  }

  //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA


  //AQUI COMECA A FFT

  //FFT.Windowing(vReal, TOTAL_CICLOS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  // Serial.println("Weighed data:");
  //PrintVector(vReal, TOTAL_CICLOS, SCL_TIME);

  FFT.Compute(vReal, vImag, TOTAL_CICLOS, FFT_FORWARD);
  //Serial.println("Computed Real values:");
  //PrintVector(vReal, TOTAL_CICLOS, SCL_INDEX);
  // Serial.println("Computed Imaginary values:");
  // PrintVector(vImag, TOTAL_CICLOS, SCL_INDEX);

  FFT.ComplexToMagnitude(vReal, vImag, TOTAL_CICLOS);
  PrintVector(vReal, (TOTAL_CICLOS >> 1), SCL_FREQUENCY);

  double x = FFT.MajorPeak(vReal, TOTAL_CICLOS, FS); //retorna a freq com maior pico
  Serial.println(x, 6);

  while(1);
}
