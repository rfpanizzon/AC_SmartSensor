import serial, csv, time, math
from numpy import arange, fft, angle
import matplotlib.pyplot as plt

STM32 = serial.Serial("COM12", baudrate=115200)
time.sleep(2) # wait for STM32

num = input("Digite o numero:\n 1. Forma de Onda Dados Brutos \n 2. Forma de Onda Ajustada \n 3. Valor RMS da corrente em cada ciclo \n 4. Valor RMS de todos os ciclos \n 5. FFT\n ")
print("\n")
if (num == "1"):
    AMOSTRAS = 512 #8 CICLOS DE 64
    dado = [ ]

    print("Opcao 1 selecionada. \n")

    STM32.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your STM32 code

    for i in range (0, AMOSTRAS):
        VALOR_SERIAL = STM32.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    STM32.close()

    for i in range (0, AMOSTRAS):
        dado[i] = float(dado[i])
    print (dado)

    # Definicao de parametros
    n_ondas = 8 # escolhe o num. de ondas capturadas
    n = n_ondas*64 # sao 64 dados capturados para cada onda
    T = n_ondas*1.0/60 # perıodo em funcao do num. de ondas
    dt = T/n # intervalo entre cada medida
    t = dt*arange(0, n) # gera vetor com os instantes de tempo

    # Tracado de graficos
    # Forma de onda
    plt.xlim(0.001, T) # define limites do eixo x
    plt.ylim(0, 1024) # define limites do eixo y
    plt.plot(t, dado, 'k-')
    plt.xlabel('T e m p o (s)')
    plt.ylabel('D A D O S (ADC)')
    plt.grid()
    plt.show()

if (num == "2"):
    AMOSTRAS = 512 #10 CICLOS DE 64
    dado = [ ]

    print("Opcao 2 selecionada. \n")

    STM32.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your STM32 code

    for i in range (0, AMOSTRAS):
        VALOR_SERIAL = STM32.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    STM32.close()

    for i in range (0, AMOSTRAS):
        dado[i] = float(dado[i])
    print (dado)

    # Definicao de parametros
    n_ondas = 8 # escolhe o num. de ondas capturadas
    n = n_ondas*64 # sao 64 dados capturados para cada onda
    T = n_ondas*1.0/60 # perıodo em funcao do num. de ondas
    dt = T/n # intervalo entre cada medida
    t = dt*arange(0, n) # gera vetor com os instantes de tempo

    # Tracado de graficos
    # Forma de onda
    plt.xlim(0.001, T) # define limites do eixo x
    plt.ylim(-0.7,0.7) # define limites do eixo y
    plt.plot(t, dado, 'k-')
    plt.xlabel('T e m p o (ms)')
    plt.ylabel('C O R R E N T E (A)')
    plt.grid()
    plt.show()

if (num == "3"):
    print("Opcao 3 selecionada. \n")

    RMS_CICLOS = 16 #RECEBENDO O VALOR RMS DE CADA CICLO
    dado = [ ]

    STM32.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your STM32 code

    for i in range (0, RMS_CICLOS):
        VALOR_SERIAL = STM32.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    STM32.close()

    for i in range (0, RMS_CICLOS):
        dado[i] = float(dado[i])
    
    print ("Valor da corrente RMS em cada ciclo: ", dado)
    # media = (sum(dado) / float(len(dado)))
    # num = round(media, 4)
    # print ("\nMedia aritimetica: ", num)

if (num == "4"):
    print("Opcao 4 selecionada. \n")

    dado = 0

    STM32.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your STM32 code


    VALOR_SERIAL = STM32.readline()
    #print (VALOR_SERIAL)
    dado = VALOR_SERIAL

    STM32.close()

    dado = float(dado)
    
    print ("Valor da corrente RMS todos os ciclos: ", dado)
    # media = (sum(dado) / float(len(dado)))
    # num = round(media, 4)
    # print ("\nMedia aritimetica: ", num)    

if (num == "5"):
    print("Opcao 5 selecionada. \n")

    AMOSTRAS = 512 #ateh a 8 armonica
    dado = [ ]

    amplitude = [ ]
    armonicas = []
    STM32.write(num.encode())
    time.sleep(2) # I shortened this to match the new value in your STM32 code

    for i in range (0, AMOSTRAS):
        VALOR_SERIAL = STM32.readline()
        #print (VALOR_SERIAL)
        dado.append(VALOR_SERIAL)

    STM32.close()

    for i in range (0, AMOSTRAS):
        dado[i] = float(dado[i])
    print (dado)

    for i in range (0, 128, 2):
        amplitude.append(dado[i+1])
        armonicas.append(dado[i])

    # Tracado de graficos

    # for i in range (0, 18, 2):
    #   aux = 0
    #   aux = math.sqrt((dado[i] * dado[i]) + (dado[i+1] * dado[i+1]))

    #   aux2 = 2 * (aux/512)
    #   amplitude.append(aux2)


    # print(amplitude)

    # plt.ylabel("Amplitude")
    # plt.xlabel("Frequência (Hz)")
    # plt.bar(armonicas, amplitude, width=1.5)
    # plt.show()


    plt.xlim(0, 480) # mostra as harmonicas de 0 a 1200Hz
    plt.ylim(0, 5) # define limites eixo y (amplitudes)
    plt.bar(armonicas, amplitude, width=3, align='edge', alpha=0.4, color='b', label='Frequencia')
    plt.xlabel('freq (Hz)')
    plt.ylabel('Amplitude')
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


