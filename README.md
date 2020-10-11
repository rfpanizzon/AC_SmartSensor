# AC_SmartSensor
![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/smart_sensor.png)

+ ## Características que o sensor terá:

  + Condicionamento do sinal:  A tensão de saída está referência a tensão de alimentação do sensor, se for 5V excursiona a saída de 0 a 5V se for 3,3 de 0 a 3,3V. Já conta com um filtro analógico de passa baixa de primeira ordem do tipo RC (Resistor Capacitor), com a finalidade de atenuar ruídos e atuar como filtro anti-aliasing;
  
  + Auto-ajuste: Calibração (ainda não evoluí nesta parte);
  
  + Lógica de decisão: O firmware entregará às seguintes funcionalidades:
    + Captura de 10 ciclos de 60hz, com 64 amostras ofertando a média aritmética das 64 amostras, valor da corrente em RMS (valor eficaz);  ESSA PARTE JÁ FUNCIONA 
    + Oferta também a FFT; AQUI TÁ QUASE FUNCIONANDO
    + Poderá também ofertar o valor da tensão (ainda necessita o desenvolvimento do hardware);
    
  + Comunicação de dados: Comunicação serial:
    + Primeira etapa consiste em enviar a média aritmética das 64 amostras, valor da corrente em RMS (valor eficaz) e a FFT, para outro microcontrolador.
    + Segunda etapa consiste em um aplicação para desktop. Página web para visualizar os dados.
    
  + Auto-identificação: Se sobrar tempo dá pra implementar a auto-identificação através da lib WiFiManager na esp 32 12e. A lib WiFiManager, serve para facilitar o gerenciamento de conexões wireless.



### ANOTAÇÕES:
* Estou utilizando arduino por enquanto
* 0 a 1V (esperar confirmação do prof Rebonatto com o prof Spaldin)
* Recomendado pelo prof Spaldin: esp 32 12e, pilha 2032
  
![Sensor](https://github.com/rfpanizzon/AC_SmartSensor/blob/main/img/modelo_smart_sensor.png)
