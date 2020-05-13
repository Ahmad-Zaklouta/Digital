import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from math import *




# ---------------------------------------------------
# Reading from file
f=open("time.txt", "r")
lines = f.readlines()
f.close()
time = np.zeros(len(lines))
n    = np.zeros(len(lines))

i = 0
for line in lines:
    words = line.split()
    time[i] = float(words[0])
    n[i] = float(words[1])
    i=i+1

print(n[0:3])
print(n[3:6])
plt.plot(n[0:3],time[0:3], '-*r')
plt.plot(n[3:6],time[3:6], '-*b')
plt.plot(n[6:9],time[6:9], '-*g')
plt.plot(n[9:12],time[9:12], '-*y')
plt.xlabel('number of time points n', fontsize = 14)
plt.ylabel('Computation time [s]', fontsize = 14)
plt.legend(['no optimalization', '-O', '-O2','-O3'])
plt.title('Computation time vs number of time points \n for different optimalization flags')
plt.savefig("timeOptimalization.png")
