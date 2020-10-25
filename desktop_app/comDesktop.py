#le os dados da porta serial, calcula a DFT e traca graficos da forma de onda e harmonicas
import serial, csv
from numpy import arange, fft, angle
import matplotlib.pyplot as plt

#Comunicacao Serial / alterar a porta COM para a mesma que o Arduino estiver usando
ARDUINO = serial.Serial("COM6", baudrate=9600, timeout=0.00001)
AMOSTRAS = 64
dado = [ ]
for i in range (0, AMOSTRAS):
VALOR_SERIAL = ARDUINO.readline()
dado.append(int(VALOR_SERIAL))
ARDUINO.close()

# Definicao de parametros
n_ondas = 2 # escolhe o num. de ondas capturadas
n = n_ondas*64 # sao 64 dados capturados para cada onda
T = n_ondas*1.0/60 # perıodo em funcao do num. de ondas
dt = T/n # intervalo entre cada medida
t = dt*arange(0, n) # gera vetor com os instantes de tempo
# Calculo da transformada de Fourier
Fk = fft.fft(dado)/(n) # coeficientes de Fourier normalizados
nu = fft.fftfreq(n,dt) # frequencias naturais
delta = angle(Fk) # ˆangulo de fase de cada componente
# Salvando dados com formatacao csv
ARQUIVO = open(’dado.csv’, "wb")
writer = csv.writer(ARQUIVO, delimiter=’\t’, quotechar=’"’,
quoting=csv.QUOTE_ALL)
for i in range (0, AMOSTRAS):
writer.writerow([dado[i], nu[i], abs(Fk[i]), delta[i]])
ARQUIVO.close()
# Trac¸ado de graficos
# Forma de onda
plt.subplot(2, 1, 1)
plt.xlim(0.001, T) # define limites do eixo x
plt.ylim(400, 640) # define limites do eixo y
plt.plot(t, dado, ’k-’)
plt.xlabel(’tempo(s)’)
plt.ylabel(’amplitude’)
# Componentes harmonicas
plt.subplot(2, 1, 2)
plt.xlim(0, 1200) # mostra as harmonicas de 0 a 1200Hz
plt.ylim(0, 50) # define limites eixo y (amplitudes)
plt.bar(nu, abs(Fk), width=2, align=’center’, alpha=0.4, color=’b’,
label=’Frequencia’)
plt.xlabel(’freq (Hz)’)
plt.ylabel(’|A(freq)|’)
plt.show()