/*
   Program to solve the two-dimensional Ising model
   The coupling constant J = 1
   Boltzmann's constant = 1, temperature has thus dimension energy
   Metropolis sampling is used. Periodic boundary conditions.
*/


#include "metroMain.h"
#include "Ising.h"
#include <mpi.h>

void writeToFile(double time, int n);
// -------------------------------------------------------------------------
int main(int nargs, char* args[]){
  int latice, mcs, n;
  double  initial_temp, final_temp, temp_step;
  int numproc, rank;
  double t1, t2, time;
  
  if( nargs <= 5 ){
    cout << "Program needs 5 arguments:" <<endl;
    cout << "1. Lattice dimension" << endl;
    cout << "2. # of computation cycles" << endl;
    cout << "3. Initial Temperature" << endl;
    cout << "4. Final Temperature" << endl;
    cout << "5. # of Temperature points" << endl;
    exit(1);
  }
  // -------------------------------------------------------------------------
  MPI_Init(&nargs, &args);
  MPI_Comm_size(MPI_COMM_WORLD, &numproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  latice=atoi(args[1]);
  mcs=atoi(args[2]);
  initial_temp=atof(args[3]);
  final_temp=atof(args[4]);
  n =atof(args[5]);

  MPI_Bcast(&mcs, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&initial_temp, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&final_temp, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  // -------------------------------------------------------------------------
  temp_step = (final_temp - initial_temp )/n;
  if(temp_step <= 0){
    temp_step = 1;
  }
  double local_n = n/numproc;
  double local_tempStart = initial_temp + rank * local_n * temp_step;
  double local_tempEnd = local_tempStart + local_n * temp_step;
  // -----------------------------------------------------------------------
  MPI_Barrier(MPI_COMM_WORLD);
  t1 = MPI_Wtime();
  for ( double temp = local_tempStart; temp <= local_tempEnd; temp+=temp_step){
    IsingModel model1 = IsingModel(latice,mcs,rank,numproc);
    model1.setup(temp);
    model1.initializeRand();
    model1.compute();  
    model1.writeToFile(mcs);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  t2 = MPI_Wtime();
  time = t2-t1;
  // -----------------------------------------------------------------------
   if (rank== 0){
   cout << "Time used: " << time << " on " << numproc << " threads." << endl;
   writeToFile(time, n);
   }
   MPI_Finalize();
  return 0;
}

// -----------------------------------------------------------------------
// Writes time coputation time and # of time points to time.txt
// -----------------------------------------------------------------------
void writeToFile(double time, int n){

        ofstream dataFile;
        dataFile.open("time.txt", ios::app);
        dataFile << setiosflags(ios::showpoint | ios::uppercase);
        dataFile << " " << setprecision(6) << time;
        dataFile << " " << setprecision(6) << n;   
        dataFile << endl;    
        dataFile.close();  
}