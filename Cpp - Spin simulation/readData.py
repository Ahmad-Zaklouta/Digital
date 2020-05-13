import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import numpy as np
from math import *


def dataSort(linenr, lines):
    matrix = np.zeros((linenr,9))
    mcpy = np.zeros((linenr,9))
    for i in range(linenr):
        words = lines[i].split()
        for j in range(9):
            matrix[i][j] = words[j]
            mcpy[i][j] =  words[j]
    matrix = matrix.transpose()
    mcpy = mcpy.transpose()
    matrix[0].sort()
    for i in range(linenr):
        for j in range(linenr):
            if(matrix[0][i] == mcpy[0][j]):
                for k in range(8):
                    matrix[k+1][i] = mcpy[k+1][j]
    return matrix
# ---------------------------------------------------
def maxTemp(cvs):
    i = 0
    Temp = 0
    for cv in cvs:
        if(cv == max(cvs)):
            Temp = m1[0][i]
            return Temp
        i = i +1 
    return Temp    
# ---------------------------------------------------

def perSpin(arr,spins):
    i = 0
    done = np.zeros(len(arr))
    for m in arr:
        done[i] = m/(spins**2)
        i = i + 1
    return done

# ---------------------------------------------------
# Reading from file
f=open("data_lattice=40.txt", "r")
lines = f.readlines()
f.close()
f=open("data_lattice=60.txt", "r")
lines2 = f.readlines()
f.close()
f=open("data_lattice=80.txt", "r")
lines3 = f.readlines()
f.close()
f=open("data_lattice=100.txt", "r")
lines4 = f.readlines()
f.close()

# ---------------------------------------------------
m1 = dataSort(112,lines)
m2 = dataSort(112,lines2)
m3 = dataSort(112,lines3)
m4 = dataSort(64,lines4)

cv1 = np.zeros(112)
cv2 = np.zeros(112)
cv3 = np.zeros(112)
cv4 = np.zeros(64)

i = 0
for t in m1[0]:
    cv1[i] = m1[2][i]*t**2/(40**2)
    i = i + 1
i = 0
for t in m2[0]:
    cv2[i] = m2[2][i]*t**2/(60**2)
    i = i +1
i = 0
for t in m3[0]:
    cv3[i] = m3[2][i]*t**2/(80**2)
    i = i + 1
i = 0
for t in m4[0]:
    cv4[i] = m4[2][i]*t**2/(80**2)
    i = i + 1


# ---------------------------------------------------
# Plotting Cv
# ---------------------------------------------------

fig, axes = plt.subplots(4, sharex=True, sharey=False)


#subplots_adjust(left=0.125, bottom=0.9, right=0.1, top=0.9, wspace=0.2, hspace=0.2)

matplotlib.rc('xtick', labelsize=6) 
matplotlib.rc('ytick', labelsize=6) 

axes[0].plot(m1[0][:],cv1,'-r')
axes[0].plot(m2[0][:],cv2,'-b')
axes[0].plot(m3[0][:],cv3,'-g')
axes[0].plot(m4[0][:],cv4,'-c')
axes[0].set_ylabel('Cv',fontsize = 12)
plt.xlabel('Temperature',fontsize = 6)
fig.suptitle(r'$Cv, \langle E \rangle, \chi, \langle |M| \rangle$ vs temperature',fontsize = 12)
axes[0].legend(['L=40 Tc=2.28425', 'L=60 Tc=2.28425','L=80 Tc=2.2775', 'L=100 Tc = 2.28425'],fontsize = 6)
#fig.suptitle("Heat capacity per spin vs temperature", fontsize = 6)
#plt.xlim(0,1000000000)
#plt.yscale('log')
#plt.savefig('critTemp.png')

# ---------------------------------------------------
# Plotting Epected Energy
# ---------------------------------------------------

e1 = perSpin(m1[1],40)
e2 = perSpin(m2[1],60)
e3 = perSpin(m3[1],80)
e4 = perSpin(m4[1],100)

axes[1].plot(m1[0][:],e1,'-r')
axes[1].plot(m2[0][:],e2,'-b')
axes[1].plot(m3[0][:],e3,'-g')
axes[1].plot(m4[0][:],e4,'-c')
axes[1].set_ylabel(r'$\langle E \rangle$',fontsize = 12)

matplotlib.rc('xtick', labelsize=6) 
matplotlib.rc('ytick', labelsize=6) 

plt.xlabel('Temperature',fontsize = 6)
plt.ylabel('Expected Energy, <E>',fontsize = 6)
axes[1].legend(['L=40', 'L=60','L=80', 'L=100'],fontsize = 6)
#plt.title("Expected Energy, <E> vs Temperature", fontsize = 6)
#plt.savefig('expectedEnergy.png')

# ---------------------------------------------------
# Plotting Suscpetibility
# ---------------------------------------------------

s1 = perSpin(m1[5],40)
s2 = perSpin(m2[5],60)
s3 = perSpin(m3[5],80)
s4 = perSpin(m4[5],100)

print(maxTemp(s1))
print(maxTemp(s2))
print(maxTemp(s3))
print(maxTemp(s4))
axes[2].plot(m1[0][:],s1,'-r')
axes[2].plot(m2[0][:],s2,'-b')
axes[2].plot(m3[0][:],s3,'-g')
axes[2].plot(m4[0][:],s4,'-c')
axes[2].set_ylabel(r'$\chi$',fontsize = 12)
axes[2].legend(['L=40', 'L=60','L=80', 'L=100'],fontsize = 6)
matplotlib.rc('xtick', labelsize=6) 
matplotlib.rc('ytick', labelsize=6) 

plt.xlabel('Temperature',fontsize = 14)
#plt.ylabel(r'$Susceptibility per spin, \chi$',fontsize = 6)
plt.legend(['L=40', 'L=60','L=80', 'L=100'],fontsize = 6)
#plt.title("Susceptibility per spin vs Temperature", fontsize = 6)
#plt.savefig('susceptebility.png')

# ---------------------------------------------------
# Plotting Magnetisation
# ---------------------------------------------------

absM1 = perSpin(m1[6],40)
absM2 = perSpin(m2[6],60)
absM3 = perSpin(m3[6],80)
absM4 = perSpin(m4[6],100)

axes[3].plot(m1[0][:],absM1,'-r')
axes[3].plot(m2[0][:],absM2,'-b')
axes[3].plot(m3[0][:],absM3,'-g')
axes[3].plot(m4[0][:],absM4,'-c')
axes[3].set_ylabel(r'$\langle |M| \rangle$',fontsize = 12)
matplotlib.rc('xtick', labelsize=6) 
matplotlib.rc('ytick', labelsize=6) 

plt.xlabel('Temperature',fontsize = 14)
#plt.ylabel('Expected Abs Magnetisation',fontsize = 6)
plt.legend(['L=40', 'L=60','L=80', 'L=100'],fontsize = 6)
#plt.title("Exp. Abs. Magnetisation per spin vs Temperature", fontsize = 6)
#plt.savefig('absMagnetisation.png')
plt.savefig('plots.png')


# ----------------------------------------------------------------
#
# ----------------------------------------------------------------


t = [2.28425, 2.28425, 2.2775, 2.28425]
L = [40, 60, 80,100]
a,b = np.polyfit(L,t,1)



x = np.linspace(0,500,10)
y = np.zeros(10)
y2 = np.zeros(10)
i = 0
for xi in x:
    y[i] = a*xi +b
    y2[i] = 2.269
    i = i+1

plt.figure()
matplotlib.rc('xtick', labelsize=9) 
matplotlib.rc('ytick', labelsize=9)
plt.plot(x,y, '-b')
plt.plot(x,y2, '-r')
plt.plot(L,t,'*g')
plt.ylim(2.25,2.3)
plt.xlabel('Latice size',fontsize = 14)
plt.ylabel('Critical temperature',fontsize = 14)
plt.title('Critical temperature vs Latice size',fontsize = 14)
plt.legend(['Interpolation function', r'$\frac{kT_c}{J} = \frac{2}{ln(1+\sqrt{2}} \approx 2.269$', r'$T_c$ computed '],fontsize = 10)
plt.savefig("polyfit.png")