

import numpy as np

#Temperature
T = 1
#Coupling constant J = 1
J = 1

#----The 16 energy configurations for the four spins----
Ec = np.asarray([-8*J,0,0,0,0,0,0,0,0,8*J,8*J,0,0,0,0,-8*J])
Ec =1.0*Ec

M = np.size(Ec)
#----The 16 magnetization configurations for the four spins----
Mc = np.asarray([4,2,2,2,2,0,0,0,0,0,0,-2,-2,-2,-2,-4])
Mc =1.0*Mc



#----Partition function----
#Setting Boltzmanns constant to 1
Kb = 1
b = 1.0/(Kb*T)

Z=0
for i in range(M):
    zz = np.exp(-b*Ec[i])
    Z = Z+zz    

#----The Boltzmann distribution----
P = np.zeros(np.size(Ec))

for i in range(M): 
    P[i] = np.exp(-b*Ec[i])/Z

#----Expectation value for energy E----
E = 0

for i in range(M):
    ee = Ec[i]*P[i]
    E = E + ee
    
#----Expectation value for magnetisation Mag----
Mag = 0
 
for i in range(M):
    mm = Mc[i]*P[i]
    Mag = Mag + mm

print(Mag)
    
#----Expectation value of the absolute value of magnetization Mag----
Mca = np.abs(Mc)
Maga = 0
 
for i in range(M):
    mma = Mca[i]*P[i]
    Maga = Maga + mma
    
#----The spesific heat Cv (constant volume)----
E2 = 0

for i in range(M):
    ee2 = ((Ec[i])**2)*P[i]
    E2 = E2 + ee2
    
Cv = (E2 - E**2)/(Kb*(T**2))

#----Magnetic susceptibility X----
Mag2a = 0

for i in range(M):
    mm2a = ((Mca[i])**2)*P[i]
    Mag2a = Mag2a + mm2a
    
X = (Mag2a - Maga**2)/(Kb*T)
print(Mag2a)


    
print('')
print('For temperature T = %d, Boltzmanns contstant Kb = %d'%(T,Kb))
print('and coupling constant J = %d, the following alalytical'%(J))
print('expressions are calculated for a 2 x 2 lattice with two spins:')   
print('')
print('The partition function: Z = %.8f'%Z)
print('Expectation value for energy E: <E> = %.8f'%E) 
print('Expectation value for absolute magnetization: <|M|> = %.8f'%Maga)
print('Spesific heat: Cv = %.8f'%Cv)  
print('Magnetic susceptibility (Absolute magnetisation used): X = %.8f'%X) 
    
#---------------Running the script:-------------------

#(base) PS C:\Users\Andreas\Documents\UIO\H2019\FYS4150\Prosjekt4\Programmer> python .\project4a.py

#For temperature T = 1, Boltzmanns contstant Kb = 1
#and coupling constant J = 1, the following alalytical
#expressions are calculated for a 2 x 2 lattice with two spins:

#The partition function: Z = 5973.91664501
#Expectation value for energy E: <E> = -7.98392834
#Expectation value for absolute magnetization: <|M|> = 3.99464293
#Spesific heat: Cv = 0.12832933
#Magnetic susceptibility (Absolute magnetisation used): X = 0.01604296





