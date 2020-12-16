#include <Arduino.h>
#include <arduinoFFT.h>

#define TOVOLTAGE 3.3 / 4096.0          // Para o STM32 BLUEPILL, o final de escala padrao eh 3,3V e o conversor eh de 12 bits
#define SERIAL_BAUDRATE 115200          // Esta eh a taxa de transmissao de dados usual para STM32 BLUEPILL
#define AMOSTRAS 64                     // Eh possivel varias este numero de amostras (N).

#define TOTAL_CICLOS 512                // 8 ciclos com 64 amostras

#define FREQUENCIA 60                   // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_CORRENTE 0.57      // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico
#define FS 512 * 60
#define PIN_A PA0                       // Pino do conversor analogico/digital escolhido para medir a corrente. Pode ser modificado
#define PIN_LED_INTERNO PC13            // Led interno do stm, definir como saida

double corrente[TOTAL_CICLOS];        // Vetor com os dados instantaneos (N AMOSTRAS) da forma de onda da corrente
float DELAY;                          // DELAY necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
float TEMPO_CONVERSAO_N_AMOSTRAS;   
float PERIODO = (1.0/FREQUENCIA);

String CMD_DESKTOP;                 // String que recebe os comando do desktop

arduinoFFT FFT = arduinoFFT();
double vReal[TOTAL_CICLOS] = {2105,
2105,
2113,
2127,
2127,
2125,
2112,
2113,
2130,
2130,
2123,
2112,
2112,
2128,
2128,
2123,
2112,
2113,
2136,
2136,
2374,
2551,
2671,
2869,
2887,
3036,
2964,
2964,
2964,
3145,
3145,
3206,
3209,
3298,
3298,
3264,
3261,
3208,
3239,
3239,
3197,
3197,
3113,
3032,
3065,
3065,
3000,
2901,
2943,
2943,
2900,
2852,
2844,
2863,
2863,
2838,
2764,
2791,
2724,
2724,
2668,
2710,
2636,
2658,
2658,
2606,
2527,
2443,
2434,
2434,
2258,
2066,
2061,
2080,
2080,
2063,
2055,
2055,
2070,
2070,
2064,
2051,
2054,
2069,
2069,
2062,
2054,
2058,
2066,
2057,
2062,
2052,
2054,
2067,
2067,
2063,
2051,
2049,
2068,
2068,
2060,
2053,
2050,
2067,
2066,
2069,
2051,
2053,
2070,
2070,
2064,
2050,
2053,
2070,
2070,
2062,
2052,
2055,
2068,
2068,
2065,
2052,
2057,
2072,
2072,
2064,
2054,
2054,
2072,
2072,
2064,
2053,
2052,
2069,
2069,
2064,
2052,
2053,
2064,
2064,
2062,
2049,
2051,
2067,
2067,
2059,
2051,
2050,
2068,
2068,
2058,
2046,
2050,
2068,
2068,
2061,
2052,
2058,
2068,
2068,
2059,
2052,
2053,
2067,
2067,
1744,
1526,
1471,
1289,
1289,
1137,
1142,
1055,
1120,
1120,
997,
967,
894,
941,
941,
928,
887,
1001,
949,
949,
1028,
1105,
1079,
1191,
1191,
1223,
1211,
1223,
1267,
1267,
1325,
1267,
1356,
1327,
1327,
1369,
1439,
1381,
1498,
1498,
1503,
1503,
1527,
1545,
1545,
1635,
1650,
1778,
1792,
1792,
1971,
2108,
2109,
2126,
2126,
2124,
2117,
2118,
2131,
2131,
2126,
2118,
2118,
2138,
2138,
2134,
2124,
2114,
2135,
2135,
2129,
2121,
2120,
2136,
2136,
2128,
2122,
2122,
2143,
2120,
2130,
2120,
2121,
2139,
2139,
2130,
2119,
2121,
2136,
2136,
2129};         //amostras para teste = {1451,1458,1468,1476,1487,1497,1509,1519,1529,1540,1550,1560,1572,1584,1598,1610,1622,1637,1651,1666,1679,1694,1710,1726,1742,1758,1775,1789,1806,1824,1842,1860,1877,1896,1914,1927,1949,1968,1987,2004,2023,2040,2060,2078,2094,2111,2126,2140,2155,2166,2178,2186,2201,2222,2243,2261,2276,2291,2308,2324,2341,2357,2372,2386,2402,2418,2431,2449,2464,2478,2494,2507,2520,2532,2543,2552,2560,2569,2577,2582,2591,2598,2606,2614,2623,2631,2639,2647,2654,2661,2670,2676,2683,2692,2697,2699,2703,2706,2711,2712,2714,2715,2718,2719,2719,2720,2722,2722,2723,2724,2724,2724,2725,2726,2725,2727,2727,2726,2727,2726,2724,2721,2714,2707,2700,2690,2678,2667,2656,2645,2635,2624,2615,2604,2594,2582,2569,2558,2541,2531,2517,2502,2487,2472,2458,2441,2426,2410,2393,2379,2362,2344,2322,2309,2291,2272,2254,2237,2219,2201,2181,2164,2145,2128,2109,2091,2074,2056,2041,2025,2012,1997,1982,1970,1956,1947,1936,1915,1895,1875,1858,1843,1828,1814,1797,1783,1768,1755,1740,1724,1709,1693,1680,1665,1649,1638,1633,1617,1607,1600,1592,1584,1577,1568,1562,1553,1546,1539,1528,1522,1514,1505,1499,1492,1484,1477,1471,1466,1461,1458,1453,1452,1450,1447,1446,1445,1443,1442,1440,1439,1441,1438,1439,1438,1438,1436,1437,1435,1436,1435,1433,1437,1438,1440,1442,1449,1457,1467,1476,1486,1496,1509,1519,1530,1539,1549};
double vImag[TOTAL_CICLOS];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

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
        abscissa = ((i * 1.0 * FS) / TOTAL_CICLOS);
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
  while(!Serial);
  //delay(4000);
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
  // Serial.print("\tDELAY NAO PODE SER NEGATIVO =  "); Serial.print(DELAY, 4); Serial.println("  microsegundos para 64 amostras");
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
    //AQUISIÇÃO E ARMAZENAMENTO DE 8 CLICLOS = 512 AMOSTRAS DO SENSOR DE CORRENTE
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO

    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      Serial.println(corrente[i]);
    }

    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "2") //Forma de Onda Ajustada
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE 8 CLICLOS = 512 AMOSTRAS DO SENSOR DE CORRENTE
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
        
    //Serial.print(media_aritmetica_corrente);
    
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
      corrente[i] -= media_aritmetica_corrente;
      //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
      
      //PARA DEIXAR O GRAFICO NA ESCALA CORRETA
      corrente[i] = corrente[i] * GANHO_SENSOR_CORRENTE * TOVOLTAGE;
      Serial.println(corrente[i], 6);
    }

    CMD_DESKTOP = "0";
  }
  
  if (CMD_DESKTOP == "3") //Valor RMS da corrente em cada ciclo
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE 8 CLICLOS = 512 AMOSTRAS DO SENSOR DE CORRENTE
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO
    
    //CALCULO MEDIA ARITMETICA
    double media_aritmetica_corrente = 0;
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      media_aritmetica_corrente += corrente[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
    }
    media_aritmetica_corrente /= TOTAL_CICLOS;
    //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    //Serial.println(media_aritmetica_corrente);
    
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] -= media_aritmetica_corrente;
    }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
    
    //CALULO DO VALOR EFICAZ CORRENTE(RMS)
    //valor_eficaz = sqrt(sum(amostra[i]^2) / numero de amostras)
    
    double soma_quadrados_corrente = 0;
    int i = 0;
    int aux = 0;

    //calcula o valor da corrente rms para cada ciclo
    for (int j = 0; j < 8; j++)
    {
      for (i; i < (aux + 64); i++)
      {
        soma_quadrados_corrente += corrente[i] * corrente[i];
      }
      double rms_corrente = sqrt(soma_quadrados_corrente / AMOSTRAS) * TOVOLTAGE;     // ajuste para ficar na escala de (0 a 5.0 ou a 3.3)Volts
      Serial.println(rms_corrente * GANHO_SENSOR_CORRENTE, 6);
      
      aux = i;
      soma_quadrados_corrente = 0;
    }
    
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "4") //Valor RMS de todos os ciclos
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE 8 CLICLOS = 512 AMOSTRAS DO SENSOR DE CORRENTE
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO
    
    //CALCULO MEDIA ARITMETICA
    double media_aritmetica_corrente = 0;
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      media_aritmetica_corrente += corrente[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
    }
    media_aritmetica_corrente /= TOTAL_CICLOS;
    //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    //Serial.println(media_aritmetica_corrente);
    
    //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      corrente[i] -= media_aritmetica_corrente;
    }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA
    
    //CALULO DO VALOR EFICAZ CORRENTE(RMS)
    //valor_eficaz = sqrt(sum(amostra[i]^2) / numero de amostras)
    
    long double soma_quadrados_corrente = 0;

    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      soma_quadrados_corrente += corrente[i] * corrente[i];
    }
    double rms_corrente = sqrt(soma_quadrados_corrente / TOTAL_CICLOS) * TOVOLTAGE;     // ajuste para ficar na escala de (0 a 5.0 ou a 3.3)Volts
    Serial.println(rms_corrente * GANHO_SENSOR_CORRENTE, 6);
  
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "5")
  {
    // //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE CORRENTE
    // for (int i = 0; i < TOTAL_CICLOS; i++)
    // {
    //   vReal[i] = analogRead(PIN_A);
    //   delayMicroseconds(DELAY);
    // }
    // //FIM DA AQUISIÇÃO

    for (int i = 0; i < TOTAL_CICLOS; i++)
    {
      vImag[i] = 0.0;
    }
          
    // //CALCULO MEDIA ARITMETICA
    // float media_aritmetica_corrente = 0;
    // for (int i = 0; i < TOTAL_CICLOS; i++)
    // {
    //   media_aritmetica_corrente += vReal[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
    // }
    // media_aritmetica_corrente /= TOTAL_CICLOS;
    // //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    // //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
    // for (int i = 0; i < TOTAL_CICLOS; i++)
    // {
    //   vReal[i] -= media_aritmetica_corrente;
    //   vReal[i] = vReal[i] * TOVOLTAGE * GANHO_SENSOR_CORRENTE;
    // }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA

    //Serial.println("Data:");
    //PrintVector(vReal, TOTAL_CICLOS, SCL_TIME);

    //FFT.Windowing(vReal, TOTAL_CICLOS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    //Serial.println("Weighed data:");
    // PrintVector(vReal, TOTAL_CICLOS, SCL_TIME);

    FFT.Compute(vReal, vImag, TOTAL_CICLOS, FFT_FORWARD);
    //Serial.println("Computed Real values:");
    //PrintVector(vReal, TOTAL_CICLOS, SCL_INDEX);
    // Serial.println("Computed Imaginary values:");
    // PrintVector(vImag, TOTAL_CICLOS, SCL_INDEX);

    FFT.ComplexToMagnitude(vReal, vImag, TOTAL_CICLOS);
    PrintVector(vReal, (TOTAL_CICLOS >> 1), SCL_FREQUENCY);
    // for (int i = 0; i < TOTAL_CICLOS; i++)
    // {
    //   Serial.println(vReal[i]);
    // }
    CMD_DESKTOP = "0";


  }
}


































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
float DELAY;                          // DELAY necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
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
  for (int i = 0; i < TOTAL_CICLOS; i++) 
  {
    Serial.println(vReal[i]);
  }

  for (int i = 0; i < TOTAL_CICLOS; i++)
  {
    vImag[i] = 0.0;
  }
        
  // CALCULO MEDIA ARITMETICA
  float media_aritmetica_corrente = 0;
  for (int i = 0; i < TOTAL_CICLOS; i++)
  {
    media_aritmetica_corrente += vReal[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
  }
  media_aritmetica_corrente /= TOTAL_CICLOS;
  // FIM CÁLCULO MÉDIA ARITMÉTICA
  
  //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
  for (int i = 0; i < TOTAL_CICLOS; i++)
  {
    vReal[i] -= media_aritmetica_corrente;
    vReal[i] = vReal[i] * TOVOLTAGE;// * GANHO_SENSOR_CORRENTE;
  }

  //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA


  //AQUI COMECA A FFT

  FFT.Windowing(vReal, TOTAL_CICLOS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  // Serial.println("Weighed data:");
  //PrintVector(vReal, TOTAL_CICLOS, SCL_TIME);

  FFT.Compute(vReal, vImag, TOTAL_CICLOS, FFT_FORWARD);
  //Serial.println("Computed Real values:");
  //PrintVector(vReal, TOTAL_CICLOS, SCL_INDEX);
  // Serial.println("Computed Imaginary values:");
  // PrintVector(vImag, TOTAL_CICLOS, SCL_INDEX);

  FFT.ComplexToMagnitude(vReal, vImag, TOTAL_CICLOS);
  // PrintVector(vReal, (TOTAL_CICLOS >> 1), SCL_FREQUENCY);

  double x = FFT.MajorPeak(vReal, TOTAL_CICLOS, FS); //retorna a freq com maior pico
  // Serial.println(x, 6);

  while(1);
}



























#include <Arduino.h>
#include <arduinoFFT.h>

#define TOVOLTAGE 3.3 / 4096.0          // Para o STM32 BLUEPILL, o final de escala padrao eh 3,3V e o conversor eh de 12 bits
#define SERIAL_BAUDRATE 115200          // Esta eh a taxa de transmissao de dados usual para STM32 BLUEPILL
#define AMOSTRAS 64                     // Eh possivel varias este numero de amostras (N).

#define TOTAL_CICLOS 1024                // 8 ciclos com 64 amostras

#define FREQUENCIA 60                   // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_CORRENTE 0.57      // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico
#define FS 3840 
#define PIN_A PA0                       // Pino do conversor analogico/digital escolhido para medir a corrente. Pode ser modificado
#define PIN_LED_INTERNO PC13            // Led interno do stm, definir como saida

// double corrente[TOTAL_CICLOS];        // Vetor com os dados instantaneos (N AMOSTRAS) da forma de onda da corrente
unsigned long DELAY = 193;                          // DELAY em us necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
unsigned long TEMPO_CONVERSAO_N_AMOSTRAS;   
float PERIODO = (1.0/FREQUENCIA);

String CMD_DESKTOP;                 // String que recebe os comando do desktop

arduinoFFT FFT = arduinoFFT();
double vReal[TOTAL_CICLOS];         //amostras para teste = {1451,1458,1468,1476,1487,1497,1509,1519,1529,1540,1550,1560,1572,1584,1598,1610,1622,1637,1651,1666,1679,1694,1710,1726,1742,1758,1775,1789,1806,1824,1842,1860,1877,1896,1914,1927,1949,1968,1987,2004,2023,2040,2060,2078,2094,2111,2126,2140,2155,2166,2178,2186,2201,2222,2243,2261,2276,2291,2308,2324,2341,2357,2372,2386,2402,2418,2431,2449,2464,2478,2494,2507,2520,2532,2543,2552,2560,2569,2577,2582,2591,2598,2606,2614,2623,2631,2639,2647,2654,2661,2670,2676,2683,2692,2697,2699,2703,2706,2711,2712,2714,2715,2718,2719,2719,2720,2722,2722,2723,2724,2724,2724,2725,2726,2725,2727,2727,2726,2727,2726,2724,2721,2714,2707,2700,2690,2678,2667,2656,2645,2635,2624,2615,2604,2594,2582,2569,2558,2541,2531,2517,2502,2487,2472,2458,2441,2426,2410,2393,2379,2362,2344,2322,2309,2291,2272,2254,2237,2219,2201,2181,2164,2145,2128,2109,2091,2074,2056,2041,2025,2012,1997,1982,1970,1956,1947,1936,1915,1895,1875,1858,1843,1828,1814,1797,1783,1768,1755,1740,1724,1709,1693,1680,1665,1649,1638,1633,1617,1607,1600,1592,1584,1577,1568,1562,1553,1546,1539,1528,1522,1514,1505,1499,1492,1484,1477,1471,1466,1461,1458,1453,1452,1450,1447,1446,1445,1443,1442,1440,1439,1441,1438,1439,1438,1438,1436,1437,1435,1436,1435,1433,1437,1438,1440,1442,1449,1457,1467,1476,1486,1496,1509,1519,1530,1539,1549};
double vImag[TOTAL_CICLOS];

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void LerAmostras()
{
  //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE CORRENTE
  for (int i = 0; i < TOTAL_CICLOS; i++)
  {
    vReal[i] = analogRead(PIN_A);
    delayMicroseconds(DELAY);
  }
  // //FIM DA AQUISIÇÃO
}

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
        abscissa = (((i * 1.0 * FS) / TOTAL_CICLOS)) ;
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
  LerAmostras();

  for (int i = 0; i < TOTAL_CICLOS; i++)
  {
    vImag[i] = 0.0;
  }
        
  // // CALCULO MEDIA ARITMETICA
  // float media_aritmetica_corrente = 0;
  // for (int i = 0; i < TOTAL_CICLOS; i++)
  // {
  //   media_aritmetica_corrente += vReal[i];     //equivale a: media_aritmetica_corrente = media_aritmetica_corrente + corrente[i];
  // }
  // media_aritmetica_corrente /= TOTAL_CICLOS;
  // // FIM CÁLCULO MÉDIA ARITMÉTICA
  
  // //REMOVER O NIVEL DC DA FORMA DE ONDA DA CORRENTE ELETRICA
  // for (int i = 0; i < TOTAL_CICLOS; i++)
  // {
  //   vReal[i] -= media_aritmetica_corrente;
  //   vReal[i] = vReal[i] * TOVOLTAGE;// * GANHO_SENSOR_CORRENTE;
  // }

  // //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA


  // //AQUI COMECA A FFT

  FFT.Windowing(vReal, TOTAL_CICLOS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  // // Serial.println("Weighed data:");
  // //PrintVector(vReal, TOTAL_CICLOS, SCL_TIME);

  FFT.Compute(vReal, vImag, TOTAL_CICLOS, FFT_FORWARD);
  // //Serial.println("Computed Real values:");
  // //PrintVector(vReal, TOTAL_CICLOS, SCL_INDEX);
  // // Serial.println("Computed Imaginary values:");
  // // PrintVector(vImag, TOTAL_CICLOS, SCL_INDEX);

  FFT.ComplexToMagnitude(vReal, vImag, TOTAL_CICLOS);
  PrintVector(vReal, (TOTAL_CICLOS >> 1), SCL_FREQUENCY);

  double x = FFT.MajorPeak(vReal, TOTAL_CICLOS, FS); //retorna a freq com maior pico
  Serial.println(x, 6);
  while(1);
}






























#include <Arduino.h>
#include <arduinoFFT.h>

#define TOVOLTAGE 3.3 / 4096.0          // Para o STM32 BLUEPILL, o final de escala padrao eh 3,3V e o conversor eh de 12 bits
#define SERIAL_BAUDRATE 115200          // Esta eh a taxa de transmissao de dados usual para STM32 BLUEPILL
#define AMOSTRAS 64                     // Eh possivel varias este numero de amostras (N).

#define TOTAL_AMOSTRAS 1024             // 16 ciclos com 64 amostras

#define FREQUENCIA 60                   // Normalmente, eh 60 ou 50 Hz
#define GANHO_SENSOR_vReal 0.57      // este 0.57 é o valor de calibracao impresso na etiqueta do sensor Elotod. No caso um sensor para 0.8 Apico
#define FS 3840
#define PIN_A PA0                       // Pino do conversor analogico/digital escolhido para medir a vReal. Pode ser modificado
#define PIN_LED_INTERNO PC13            // Led interno do stm, definir como saida

unsigned long DELAY = 188;              // DELAY em us necessario para ajustar o tempo do loop de aquisicao e fazer N AMOSTRAS em um ciclo de 50Hz ou 60Hz
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
        abscissa = ((i * 1.0 * FS) / TOTAL_AMOSTRAS);
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
    //AQUISIÇÃO E ARMAZENAMENTO DE 8 CLICLOS = 512 AMOSTRAS DO SENSOR DE vReal
    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      vReal[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO
    
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
      vReal[i] = vReal[i] * GANHO_SENSOR_vReal * TOVOLTAGE;
      Serial.println(vReal[i], 6);
    }

    CMD_DESKTOP = "0";
  }
  
  if (CMD_DESKTOP == "3") //Valor RMS da vReal em cada ciclo
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE 8 CLICLOS = 512 AMOSTRAS DO SENSOR DE vReal
    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      vReal[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO
    
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
      Serial.println(rms_vReal * GANHO_SENSOR_vReal, 6);
      
      aux = i;
      soma_quadrados_vReal = 0;
    }
    
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "4") //Valor RMS de todos os ciclos
  {
    //AQUISIÇÃO E ARMAZENAMENTO DE 8 CLICLOS = 512 AMOSTRAS DO SENSOR DE vReal
    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      vReal[i] = analogRead(PIN_A);
      delayMicroseconds(DELAY);
    }
    //FIM DA AQUISIÇÃO
    
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
    Serial.println(rms_vReal * GANHO_SENSOR_vReal, 6);
  
    CMD_DESKTOP = "0";
  }

  if (CMD_DESKTOP == "5")
  {
    // //AQUISIÇÃO E ARMAZENAMENTO DE N AMOSTRAS DO SENSOR DE vReal
    // for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    // {
    //   vReal[i] = analogRead(PIN_A);
    //   delayMicroseconds(DELAY);
    // }
    // //FIM DA AQUISIÇÃO

    for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    {
      vImag[i] = 0.0;
    }
          
    // //CALCULO MEDIA ARITMETICA
    // float media_aritmetica_vReal = 0;
    // for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    // {
    //   media_aritmetica_vReal += vReal[i];     //equivale a: media_aritmetica_vReal = media_aritmetica_vReal + vReal[i];
    // }
    // media_aritmetica_vReal /= TOTAL_AMOSTRAS;
    // //FIM CÁLCULO MÉDIA ARITMÉTICA
    
    // //REMOVER O NIVEL DC DA FORMA DE ONDA DA vReal ELETRICA
    // for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    // {
    //   vReal[i] -= media_aritmetica_vReal;
    //   vReal[i] = vReal[i] * TOVOLTAGE * GANHO_SENSOR_vReal;
    // }
    //FIM REMOÇÃO DO NIVEL DC DA FORMA DE ONDA

    //Serial.println("Data:");
    //PrintVector(vReal, TOTAL_AMOSTRAS, SCL_TIME);

    //FFT.Windowing(vReal, TOTAL_AMOSTRAS, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    //Serial.println("Weighed data:");
    // PrintVector(vReal, TOTAL_AMOSTRAS, SCL_TIME);

    FFT.Compute(vReal, vImag, TOTAL_AMOSTRAS, FFT_FORWARD);
    //Serial.println("Computed Real values:");
    //PrintVector(vReal, TOTAL_AMOSTRAS, SCL_INDEX);
    // Serial.println("Computed Imaginary values:");
    // PrintVector(vImag, TOTAL_AMOSTRAS, SCL_INDEX);

    FFT.ComplexToMagnitude(vReal, vImag, TOTAL_AMOSTRAS);
    PrintVector(vReal, (TOTAL_AMOSTRAS >> 1), SCL_FREQUENCY);
    // for (int i = 0; i < TOTAL_AMOSTRAS; i++)
    // {
    //   Serial.println(vReal[i]);
    // }
    CMD_DESKTOP = "0";


  }
}