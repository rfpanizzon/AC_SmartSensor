import serial, csv, time
from numpy import arange, fft, angle
import matplotlib.pyplot as plt

ARDUINO = serial.Serial("COM6", baudrate=9600)
time.sleep(2) # wait for Arduino

num = input("Digite o numero:\n 1. Forma de Onda \n 2. Valor RMS da corrente\n 3. FFT\n ")
print("\n\n")
if (num == "1"):
    AMOSTRAS = 640 #10 CICLOS DE 64
    dado = [ ]

    ARDUINO.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your Arduino code

    for i in range (0, AMOSTRAS):
        VALOR_SERIAL = ARDUINO.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    ARDUINO.close()

    for i in range (0, AMOSTRAS):
        dado[i] = float(dado[i])
    print (dado)

    # Definicao de parametros
    n_ondas = 10 # escolhe o num. de ondas capturadas
    n = n_ondas*64 # sao 64 dados capturados para cada onda
    T = n_ondas*1.0/60 # perıodo em funcao do num. de ondas
    dt = T/n # intervalo entre cada medida
    t = dt*arange(0, n) # gera vetor com os instantes de tempo

    # Tracado de graficos
    # Forma de onda
    plt.subplot(2, 1, 1)
    plt.xlim(0.001, T) # define limites do eixo x
    plt.ylim(200, 800) # define limites do eixo y
    plt.plot(t, dado, 'k-')
    plt.xlabel('tempo(s)')
    plt.ylabel('amplitude')
    plt.grid()
    plt.show()

if (num == "2"):
    RMS_CICLOS = 10 #10 CICLOS DE 64
    dado = [ ]

    ARDUINO.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your Arduino code

    for i in range (0, RMS_CICLOS):
        VALOR_SERIAL = ARDUINO.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    ARDUINO.close()

    for i in range (0, RMS_CICLOS):
        dado[i] = float(dado[i])
    
    print ("Valor da corrente RMS em cada ciclo: ", dado)
    media = (sum(dado) / float(len(dado)))
    num = round(media, 4)
    print ("\nMedia aritimetica: ", num)

if (num == "3")
    AMOSTRAS = 640 #10 CICLOS DE 64
    dado = [ ]
    dadofreq [ ]
    dadox = [ ]

    ARDUINO.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your Arduino code

    for i in range (0, AMOSTRAS):
        VALOR_SERIAL = ARDUINO.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    ARDUINO.close()

    for i in range (0, AMOSTRAS, 2):
        dadox[i] = float(dado[i])
    print (dadox)

    for i in range (1, AMOSTRAS, 2):
        dadofreq[i] = float(dado[i])
    print (dadofreq)

    # Tracado de graficos

    #####SE NAO PLOTA PODE SE OS PARAM AQUI DO SUBPLOT
    plt.subplot(2, 1, 2)
    plt.xlim(0, 1200) # mostra as harmonicas de 0 a 1200Hz
    plt.ylim(0, 50) # define limites eixo y (amplitudes)
    plt.bar(dadox, abs(dadofreq), width=2, align='center', alpha=0.4, color='b', label='Frequencia')
    plt.xlabel('freq (Hz)')
    plt.ylabel('|A(freq)|')
    plt.show()




# # C´alculo da transformada de Fourier
# Fk = fft.fft(dado)/(n) # coeficientes de Fourier normalizados
# nu = fft.fftfreq(n,dt) # frequencias naturais
# delta = angle(Fk) # ˆangulo de fase de cada componente


# ARQUIVO = open('dado.csv', "wb")
# writer = csv.writer(ARQUIVO, delimiter='\t', quotechar='"',
# quoting=csv.QUOTE_ALL)
# for i in range (0, AMOSTRAS):
#     writer.writerow([dado[i], nu[i], abs(Fk[i]), delta[i]])
# ARQUIVO.close()


