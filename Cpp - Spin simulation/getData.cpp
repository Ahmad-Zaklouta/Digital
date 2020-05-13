#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

int main(){
    string rm = "rm *.txt";
    system(rm.c_str());
//  system("rm *.txt");
    string prg = "mpirun -np 16 metroMain.exe ";
    string spc = " ";
    string Cmd;
    string end = " 2 2.3 100";
    string L[4] = {"40", "60", "80", "100"};
    string cycles[4] = {"2000000", "4000000", "6000000", "8000000"}; 
    
    for(int i =0; i<4;i++){
        Cmd.append(prg);
        Cmd.append((L[i]));
        Cmd.append(spc);
        Cmd.append((cycles[i]));  
        Cmd.append(end); 
        cout << Cmd << endl;
        system(Cmd.c_str());
        Cmd.clear();
    }
}
