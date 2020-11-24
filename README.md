# AC_SmartSensor
![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/smart_sensor.png)

+ ## Características do sensor:

  + Condicionamento do sinal:  A tensão de saída está referência a tensão de alimentação do sensor, se for 5V excursiona a saída de 0 a 5V se for 3,3 de 0 a 3,3V. Já conta com um filtro analógico de passa baixa de primeira ordem do tipo RC (Resistor Capacitor), com a finalidade de atenuar ruídos e atuar como filtro anti-aliasing;
  
  + Lógica de decisão: O firmware entregará às seguintes funcionalidades:
    + Captura de 8 ciclos de 60hz, com 64 amostras por cilco;
    + Forma de onda; 
    + Valor da corrente em RMS (valor eficaz);
    + FFT;
    
  + Comunicação de dados: Comunicação serial:
    + Desenvolvido uma aplicação desktopk na linguagem Python. O código faz a comunicação com o microcontrolador, onde o usuário poderá visualizar os dados processados pelo firmware.
    

