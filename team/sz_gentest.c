// testing generalized patterns  pattern_test.c   B. Wilkinson Dec 19, 2015    Notes:  master acts as one slave

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "suzaku.h"  	// Basic Suzaku macros

		//Declared as constants to allow static arrays for input and output
#define D 2	// # of data items in slave data.
#define P 4	// Number of processes -- this code must be run only with this number of processes

void compute(int taskID, double B[P][D], double A[D]) {   //  each slave

	printf("Slave %d step %d A[0]=%5.2f, B[0][0]=%5.2f, B[1][0]=%5.2f, B[2][0]=%5.2f, B[3][0]=%5.2f\n", SZ_Get_process_num(), taskID,A[0],B[0][0],B[1][0],B[2][0],B[3][0]); 						  
	
	return;
}

int main(int argc, char *argv[]) {
	int i,j,p;				// p is actual number of processes when executing program
	double A[D],B[P][D];			// A is the slave data, B holds data sent from other slaves
	int steps = 2;				// number of time steps

	SZ_Init(p);				// initialize MPI message-passing environment

	if (p != P) printf("ERROR Program must be run with %d processes\n",P);

	SZ_Parallel_begin			// parallel section, all processes do this

	   for (i = 0; i < D; i++) {		// all processes
	  	A[i] = SZ_Get_process_num(); 
	  	for (j = 0; j < P; j++){	// initialize data
		    B[j][i] = 0;
	   	}
	   }
		
	   SZ_Pattern_init("all-to-all",D);	// set up slave interconnections
	   SZ_Print_connection_graph();		// for checking
	   
	   //SZ_Broadcast(A);		// broadcast initial data to all slaves, not actually needed here as data is initialized in each process

	   for (i = 0; i < steps; i++) {
		compute(i,B,A);			// slaves execute compute, master acts as one slave
	   	SZ_Generalized_send(A,B);	// sent compute results to connected slaves
	   }

	   SZ_Gather(A,A);			// collect results from slaves, gather()
		
	SZ_Parallel_end;			// end of parallel

	SZ_Finalize(); 

	return 0;
}
