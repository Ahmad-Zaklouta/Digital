# Comment lines
# General makefile for c - choose PROG =   name of given program
# Here we define compiler option, libraries and the  target
CC= mpic++ -std=c++11 -Wall -Wextra -Wpedantic -lgomp -larmadillo -lblas -llapack


DEPS = metroMain.h Ising.h

all: metroMain data

metroMain: metroMain.cpp IsingModel.cpp
	${CC}  -o metroMain.exe metroMain.cpp IsingModel.cpp

data: getData.cpp
	${CC}  -o getData.exe getData.cpp 
