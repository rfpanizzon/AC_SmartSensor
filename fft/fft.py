#https://towardsdatascience.com/fast-fourier-transform-937926e591cb

import numpy as np
import matplotlib.pyplot as plt

t = np.linspace(0, 0.5, 500) # 500 números, de 0 a 0.5 -> 1 kHz de amostragem
s = np.sin(40 * 2 * np.pi * t) + 0.5 * np.sin(90 * 2 * np.pi * t)

plt.ylabel("Amplitude")
plt.xlabel("Tempo (s)")
plt.plot(t, s)
plt.savefig('fft_ts.png')
plt.close()

fft = np.fft.fft(s)

T = t[1] - t[0] # 0.001 -> 1/T = 1000
N = s.size
#f = np.linspace(0, 1 / T, N)
# fornece os componentes de frequência correspondentes aos dados
f = np.fft.fftfreq(len(s), T)

frequencias = f[:N // 2]
amplitudes = np.abs(fft)[:N // 2] * 1 / N

plt.ylabel("Amplitude")
plt.xlabel("Frequência (Hz)")
plt.bar(frequencias, amplitudes, width=1.5)
plt.savefig('fft_freq.png')
plt.close()

plt.specgram(s, NFFT=N-1, Fs=1/T, scale='linear', scale_by_freq=False)
plt.colorbar()
plt.ylabel('Frequência (Hz)')
plt.xlabel('Tempo (s)')
plt.savefig('fft_spectrogram.png')
plt.close()