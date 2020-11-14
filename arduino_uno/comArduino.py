import serial, csv, time
from numpy import arange, fft, angle
import matplotlib.pyplot as plt

ARDUINO = serial.Serial("COM6", baudrate=9600)
time.sleep(2) # wait for ARDUINO

num = input("Digite o numero:\n 1. Forma de Onda \n 2. Valor RMS da corrente\n 3. FFT\n ")
print("\n")
if (num == "1"):
    AMOSTRAS = 192 #3 CICLOS DE 64
    dado = [ ]

    ARDUINO.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your ARDUINO code

    for i in range (0, AMOSTRAS):
        VALOR_SERIAL = ARDUINO.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    ARDUINO.close()

    for i in range (0, AMOSTRAS):
        dado[i] = float(dado[i])
    print (dado)

    # Definicao de parametros
    n_ondas = 3 # escolhe o num. de ondas capturadas
    n = n_ondas*64 # sao 64 dados capturados para cada onda
    T = n_ondas*1.0/60 # perıodo em funcao do num. de ondas
    dt = T/n # intervalo entre cada medida
    t = dt*arange(0, n) # gera vetor com os instantes de tempo

    # Tracado de graficos
    # Forma de onda
    plt.xlim(0.001, T) # define limites do eixo x
    plt.ylim(-1, 1) # define limites do eixo y
    plt.plot(t, dado, 'k-')
    plt.xlabel('T e m p o (s)')
    plt.ylabel('C O R R E N T E (A)')
    plt.grid()
    plt.show()

if (num == "2"):
    RMS_CICLOS = 3 #RECEBENDO O VALOR RMS DE CADA CICLO
    dado = [ ]

    ARDUINO.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your ARDUINO code

    for i in range (0, RMS_CICLOS):
        VALOR_SERIAL = ARDUINO.readline()
        print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    ARDUINO.close()

    for i in range (0, RMS_CICLOS):
        dado[i] = float(dado[i])
    
    print ("Valor da corrente RMS em cada ciclo: ", dado)
    media = (sum(dado) / float(len(dado)))
    num = round(media, 4)
    print ("\nMedia aritimetica: ", num)

if (num == "3"):
    AMOSTRAS = 8 #ateh a 8 armonica
    dado = [ ]
    armonicas = [60, 120, 180, 240, 300, 360, 420, 540]
    ARDUINO.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your ARDUINO code

    for i in range (0, AMOSTRAS):
        VALOR_SERIAL = ARDUINO.readline()
        print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    ARDUINO.close()

    for i in range (0, AMOSTRAS):
        dado[i] = float(dado[i])
    print (dado)

    # Tracado de graficos

    plt.xlim(0, 600) # mostra as harmonicas de 0 a 1200Hz
    plt.ylim(0, 200) # define limites eixo y (amplitudes)
    plt.bar(armonicas, dado, width=3, align='edge', alpha=0.4, color='b', label='Frequencia')
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


