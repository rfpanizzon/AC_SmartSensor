# Smart Sensor de Corrente Alternada não Invasivo
![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/smart_sensor.png)

+ ## Características do sensor:
  + Visão Geral do Protótipo:
  
    ![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/2.PNG)

  + Circuito de condicionamento de sinal:  A tensão de saída está referência a tensão de alimentação do sensor, se for 5V excursiona a saída de 0 a 5V se for 3,3 de 0 a 3,3V. Já conta com um filtro analógico de passa baixa de primeira ordem do tipo RC (Resistor Capacitor), com a finalidade de atenuar ruídos e atuar como filtro anti-aliasing.
  
  + Lógica de decisão, o firmware entregará às seguintes funcionalidades:
      + Formas de ondas com os dados brutos do AD.
      + Formas de ondas ajustadas.
      + Valor RMS de cada ciclo individual.
      + Valor RMS de todos os ciclos.
      + FFT.

  + Comunicação de dados, comunicação serial:
    + Desenvolvido uma aplicação desktopk na linguagem Python. O código faz a comunicação com o microcontrolador, onde o usuário poderá visualizar os dados processados pelo firmware.
    
+ ## Resultados, a carga utilizada foi um monitor de computador:
    + Função 1 e 2: 

      ![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/3.PNG)
    
      O cálculo do vetor de tempo dos gráficos, representado pelo eixo X, tem duração total de 133ms (milissegundos), tempo necessário para completar 8 ciclos na frequência de 60Hz da corrente elétrica. Pode-se constatar a captura de 8 ciclos completos, visto que a forma de onda se repete 8 vezes. Na esquerda da Figura, temos os dados brutos do AD com resolução de 12 bits, que nos resulta em 4096 valores essa é a função 1 do smart sensor. E na direita da Figura  temos os dados ajustados, que é função 2, onde podemos ver que o pico de corrente chegou próximo de 0.4A. Esses valores são do monitor do meu computador, com o brilho no nível mínimo.

      ![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/4.PNG)
      
      Aqui temos a comparação das duas formas de onda referentes ao monitor de computador. Na esquerda com o brilho no nível mínimo e na direita com o nível no máximo. Podemos perceber que o pico de corrente com o brilho máximo chegou a 0.8A, ou seja, dobrou em relação a esquerda.

    + Função 3 e 4:
    
      ![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/5.PNG)
      
      Aqui temos às funções 3 e 4, onde obeservamos que na função 3 temos 8 valores referentes ao valor rms de cada ciclo e na função 4 apenas um valor que seria o consumo médio da carga medida. Temos a comparação dos valores com o brilho no mínimo e no máximo. Observa-se que o consumo médio com o brilho máximo, ficou em 0.243233A, ou seja, o monitor com o brilho ajustado no nível máximo consome, aproximadamente, 2.19 vezes mais corrente do que com o brilho no mínimo. Também da pra observar que o consumo médio fica entre o menor e maior valor da função 3.

    + Função 5:

      ![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/6.PNG)

      Consideramos para a construção do gráfico exibir até a 8 harmônica da frequência fundamental, que é 480Hz. Podemos visualizar que a frequência de maior amplitude foi em 60Hz e depois as amplitudes vão diminuindo nas harmônicas fundamentais.
