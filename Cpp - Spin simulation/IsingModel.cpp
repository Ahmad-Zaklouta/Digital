#include "Ising.h"
#include <mpi.h>

IsingModel::IsingModel(int latice, int mcs, int rank, int numproc){

  this-> latice = latice;
  this->mcs = mcs;
  this->rank = rank;
  this->numproc = numproc;

  temp = 0;
  spin_matrix= new int *[latice];
  for (int i = 0; i < latice; ++i){
    spin_matrix[i] = new int[latice];
    }
  E = 0;
  M = 0;

  double w[17];
  double average[5];
  int n_spins = latice*latice;
  double E_expect;
  double EE_expect;
  double Evar;
  double Cv;
  double expected_abs_M;
  double M_expect;
  double MM_expect;
  double M_abs_expect;
  double Mvar;
  double suscept; 
  double suscept_abs; 

  int accepted_states;
  long idum = -1*(rank+1);
  double temp;
}

// ----------------------------------------------------------------------
// initialise energy and magnetization
// setup array for possible energy changes
// initialise array for expectation values & resets # of accepted states
// ----------------------------------------------------------------------
void IsingModel::setup(double temp){
      this->temp = temp;
      for( int de =-8; de <= 8; de++) w[de+8] = 0;
      for( int de =-8; de <= 8; de+=4) w[de+8] = exp(-de/temp);
      for( int i = 0; i < 5; i++) average[i] = 0.;
      accepted_states = 0;  
      E = 0;
      M = 0;
}

// ----------------------------------------------------------------------
// function to initialise  spin matrix in an orderered manner, 
// & the magnetisation and the energy.
// ----------------------------------------------------------------------
void IsingModel::initializeOrdered(){
    // setup spin matrix and intial magnetization
      for(int y =0; y < latice; y++) {
        for (int x= 0; x < latice; x++){
          if (temp < 1.5){ 
            spin_matrix[y][x] = 1;
          } 
          M +=  (double) spin_matrix[y][x];
        }
      }
      // setup initial energy
      for(int y =0; y < latice; y++) {
        for (int x= 0; x < latice; x++){
          E -=  (double) spin_matrix[y][x]*
  	            (spin_matrix[periodic(y,latice,-1)][x] +
  	            spin_matrix[y][periodic(x,latice,-1)]);
        }
      }
  }

// ----------------------------------------------------------------------
// function to initialise  spin matrix in a randomly manner, 
// & the magnetisation and the energy.
// ----------------------------------------------------------------------
void IsingModel::initializeRand(){
      for(int y =0; y < latice; y++) {
        for (int x= 0; x < latice; x++){
          if(ran1()<=0.5){
            spin_matrix[y][x] = 1; // spin orientation for the ground state
             M +=  (double) spin_matrix[y][x];
          }
          else{
            spin_matrix[y][x] = -1; // spin orientation for the ground state
             M +=  (double) spin_matrix[y][x];
          }
        }
      }
      // setup initial energy
      for(int y =0; y < latice; y++) {
        for (int x= 0; x < latice; x++){
          E -=  (double) spin_matrix[y][x]*
    	(spin_matrix[periodic(y,latice,-1)][x] +
    	 spin_matrix[y][periodic(x,latice,-1)]);
        }
      }      
}

// ----------------------------------------------------------------------
//  loops over cycles for Metropolis algorythm, and sums up energy and
//  magnetisation values from Metropolis function
// ----------------------------------------------------------------------
void IsingModel::compute(){
  for (int cycles = 1; cycles <= mcs; cycles++){
        Metropolis();         
        average[0] += E;    
        average[1] += E*E;
        average[2] += M;    
        average[3] += M*M; 
        average[4] += fabs(M);
        ExpectationsAndVar(cycles);  
    } 
}

// ----------------------------------------------------------------------
// loops through all the spins, with random postions and computes 
// energy difference between current and possible next spin configuration
// ----------------------------------------------------------------------
void IsingModel::Metropolis(){
    // loop over all spins
    int ix, iy, deltaE;
    for(int y  =0; y < latice; y++) {
      for (int x= 0; x < latice; x++){
        // Find random position
        ix = (int) (ran1()*(double)latice);
        iy = (int) (ran1()*(double)latice);
        deltaE =  2*spin_matrix[iy][ix]*
  	                (spin_matrix[iy][periodic(ix,latice,-1)]+
  	                spin_matrix[periodic(iy,latice,-1)][ix] +
  	                spin_matrix[iy][periodic(ix,latice,1)] +
  	                spin_matrix[periodic(iy,latice,1)][ix]);
        MetroTest(ix, iy, deltaE);
    }
  } 
} 

// ----------------------------------------------------------------------
// Here we perform the Metropolis test:
// if it passes we flip one spin and accept new spin configuration
// update energy and magnetization and the # of accepted states
// ----------------------------------------------------------------------
void IsingModel::MetroTest(int ix, int iy, int deltaE){
  if( ran1() <= w[deltaE+8] ){   
  	spin_matrix[iy][ix] *= -1;  
    M += (double) 2*spin_matrix[iy][ix];
    E += (double) deltaE;
    accepted_states += 1;
  }
}
// ----------------------------------------------------------------------
// Computes the expectation values for energy and magnetisation,
// heat capaciy, and susceptibility 
// Not per spin though, that is taken care of in python scripts
// ----------------------------------------------------------------------
void IsingModel::ExpectationsAndVar(int cycles){
    E_expect     = average[0]/((double) cycles);
    EE_expect    = average[1]/((double) cycles);
    M_expect     = average[2]/((double) cycles);
    MM_expect    = average[3]/((double) cycles);
    M_abs_expect = average[4]/((double) cycles);
 
    Cv            = (EE_expect-pow(E_expect,2))/(pow(temp,2));
    suscept       = (MM_expect -pow(M_expect,2))/(temp);
    suscept_abs   = (MM_expect -pow(M_abs_expect,2))/(temp);
}
// ----------------------------------------------------------------------
// Writes the data to file 
// ----------------------------------------------------------------------
void IsingModel::writeToFile(int cycles){
        ofstream dataFile;
        string filename = "data_lattice="+to_string(latice)+".txt";
        dataFile.open(filename, ios::app);
        dataFile << setiosflags(ios::showpoint | ios::uppercase);
        dataFile << " " << setprecision(9) << temp;
        dataFile << " " << setprecision(9) << E_expect;
        dataFile << " " << setprecision(9) << Cv;
        dataFile << " " << setprecision(9) << M_expect;
        dataFile << " " << setprecision(9) << suscept;
        dataFile << " " << setprecision(9) << suscept_abs;
        dataFile << " " << setprecision(9) << M_abs_expect;
        dataFile << " " << setprecision(9) << accepted_states;
        dataFile << " " << cycles          << endl;
        dataFile.close(); 
}

// ----------------------------------------------------------------------
// Prints final results
// ----------------------------------------------------------------------
void IsingModel::printValues(){
  if (rank== 0){
  cout << "L:"                      << latice <<endl;
  cout << "cycles: "                << mcs<<endl;
  cout <<" Temp: "                  << temp<<endl; 
  cout <<"E: "                      <<E<<endl;
  cout << "M: "                     <<average[2]<<endl;
  cout <<"<E>: "                    <<E_expect<<endl;
  cout <<"Cv: "                     <<Evar<<endl;
  cout <<"M expect: "               <<M_expect<< endl;
  cout <<"abs(M) expexted "         << M_abs_expect<<endl;
  cout <<"Susceptebilety: "         << suscept<<endl;
  cout <<"E expect: "               <<E_expect<<endl;
  cout << "# of Accepted states: "  << accepted_states << endl;
  }
}

// ----------------------------------------------------------------------
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

// ----------------------------------------------------------------------
//  Computes a number between -1 and 1 which is returned
// ----------------------------------------------------------------------
double IsingModel::ran1(){
int             j;
long            k;
static long     iy=0;
static long     iv[NTAB];

if (idum <= 0 || !iy) {
   if (-(idum) < 1) idum=1;
   else idum = -(idum);
   for(j = NTAB + 7; j >= 0; j--) {
      k     = (idum)/IQ;
      idum = IA*(idum - k*IQ) - IR*k;
      if(idum < 0) idum += IM;
      if(j < NTAB) iv[j] = idum;
   }
   iy = iv[0];
}
k     = (idum)/IQ;
idum = IA*(idum - k*IQ) - IR*k;
if(idum < 0) idum += IM;
j     = iy/NDIV;
iy    = iv[j];
iv[j] = idum;
if((temp=AM*iy) > RNMX) return RNMX;
else return temp;
}
// ----------------------------------------------------------------------
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------