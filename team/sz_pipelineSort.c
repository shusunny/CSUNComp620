
// Suzaku pipeline Sorting using a pipeline B. Wilkinson Dec 3rd, 2015.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "suzaku.h"  	// Basic Suzaku macros

#define N 1		// Size of data being sent
#define P 4		// Number of processes and number of numbers, each process only handles one number

void init(int *T,int *D,int *R) { // initialization. R not used

	*T = 4;
	*D = 1;
	//*R = 1;		// not used

	srand(999);

	return;
}

void diffuse (int taskID,double output[N]) {
	
	if (taskID < P) output[0] = rand()% 100; // P numbers, a number between 0 and 99
	else output[0] = 999;			// otherwise terminator

	return;
		
}

void compute(int taskID, double input[N], double output[N]) {   //  each slave
							  // Only input[0] used in this application
	static double largest = 0;

	if (input[0] > largest) { 
		output[0] = largest; 	// copy current largest into send array
		largest = input[0];	// replace largest with received number
	} else {
		output[0] = input[0];		// copy received number into send array
	} 
	return;

}

void gather(int taskID,double input[N]) { 

	if (input[0] == 999) SZ_terminate();
	return;
}

int main(int argc, char *argv[]) {
	int p;					// p is actual number of processes when executing program

	SZ_Init(p);				// initialize MPI message-passing environment

	if (p != P) 				// number of processes hardcoded
		printf("ERROR number of processes must be %d\n",P); 
	
	SZ_Parallel_begin			// parallel section, all processes do this

		SZ_Debug();

		SZ_Pipeline(init,diffuse,compute,gather);	

	SZ_Parallel_end;			// end of parallel

	SZ_Finalize(); 

	return 0;
}
