import serial, csv
from numpy import arange, fft, angle
import matplotlib.pyplot as plt

ARDUINO = serial.Serial("COM6", baudrate=9600)
AMOSTRAS = 640 #10 CICLOS DE 64
dado = [ ]

for i in range (0, AMOSTRAS):
    VALOR_SERIAL = ARDUINO.readline()
    #print (VALOR_SERIAL)
    dado.append(VALOR_SERIAL)
ARDUINO.close()
for i in range (0, AMOSTRAS):
    dado[i] = float(dado[i])
print (dado)
# # Definic¸˜ao de parˆametros
n_ondas = 10 # escolhe o num. de ondas capturadas
n = n_ondas*64 # s˜ao 64 dados capturados para cada onda
T = n_ondas*1.0/60 # per´ıodo em func¸˜ao do num. de ondas
dt = T/n # intervalo entre cada medida
t = dt*arange(0, n) # gera vetor com os instantes de tempo
# # C´alculo da transformada de Fourier
# Fk = fft.fft(dado)/(n) # coeficientes de Fourier normalizados
# nu = fft.fftfreq(n,dt) # frequˆencias naturais
# delta = angle(Fk) # ˆangulo de fase de cada componente


# ARQUIVO = open('dado.csv', "wb")
# writer = csv.writer(ARQUIVO, delimiter='\t', quotechar='"',
# quoting=csv.QUOTE_ALL)
# for i in range (0, AMOSTRAS):
#     writer.writerow([dado[i], nu[i], abs(Fk[i]), delta[i]])
# ARQUIVO.close()


#Trac¸ado de graficos
#Forma de onda
plt.subplot(2, 1, 1)
plt.xlim(0.001, T) # define limites do eixo x
plt.ylim(200, 800) # define limites do eixo y
plt.plot(t, dado, 'k-')
plt.grid()
plt.show()