import matplotlib.pyplot as plt
import numpy as np

with open('results.txt', 'r') as file:
    a = file.read().split(' ')

for i in range(len(a)):
    a[i] = a[i][:-1]
a[0] = a[1:]

a = a[0]
for i in range(len(a)):
    a[i] = int(a[i])

a = np.array(a)
print(a)


a -= a[0]
a -= (np.linspace(0.0, 1.0, len(a)) * a[-1]).astype(int)


plt.plot(a)
plt.show()


plt.plot(np.fft.fftshift(np.fft.fftfreq(len(a))) * 1000, np.fft.fftshift(np.abs(np.fft.fft(a - np.average(a)))))
plt.show()

