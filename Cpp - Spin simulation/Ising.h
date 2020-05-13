#ifndef Ising
#define Ising

#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <random>
#include <armadillo>
#include <string>


using namespace  std;
using namespace arma;
// inline function for periodic boundary conditions
inline int periodic(int i, int limit, int add){
  return (i+limit+add) % (limit);
}
#endif


class IsingModel{
    private:
        int latice;                                   // latice dimension
        int mcs;                                      // monte carlo cycles
        int rank;                                     // rank of process 
        int numproc;                                  // number of processes
        int n_spins;                                  // number of spins
        double temp;                                  // temperature
        double ran1();                                // ran function 
        void Metropolis();                            // metropolis algorythm 
        void MetroTest(int ix, int iy, int deltaE);   // metropolis test  in alg
        void ThreadReduce(int cycles_pr_proc);        // adds together vars from all processes 

    public:
        int **spin_matrix;                            // matrix of spins
        double E;                                     // energy
        double M;                                     // magnetisation
        double w[17];                                 // possible energy changes
        double average[5];                            //
        double E_expect;                              //
        double EE_expect;                             //
        double Cv;                                    //
        double M_expect;                              //
        double MM_expect;                             //
        double M_abs_expect;                          //
        double suscept;                               // suceptibility
        double suscept_abs;
        double expected_E;                            //
        double expected_M;                            //
        int accepted_states;                          // nr. of accepted states
        int idum;                                     // ran1 seed

        IsingModel(int latic, int mcs,int rank, int numproc);
        void setup(double temp);
        void initializeOrdered();
        void initializeRand();
        void compute();
        void ExpectationsAndVar(int cycles);
        void printValues();
        void writeToFile(int cycles);
        
};