// Suzaku Workpool pattern version 1 -- Application:  Monte Carlo Pi, MontePI_workpool.c
// B. Wilkinson April 4, 2015
/* 
To execute:
1. To compile suzaku.o with suzaku.c suzaku.h
mpicc -c -o suzaku.o suzaku.c -lm

2. MontePi_workpool: MontePi_workpool.c suzaku.h suzaku.o
mpicc -o sz_montePi sz_montePi.c suzaku.o -lm
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "suzaku.h" 

// required Suzaku constants

#define T 20	// number of tasks, max = INT_MAX - 1
#define D 1		// number of data items in each task, doubles only
#define R 1		// number of data items in result of each task, doubles only
// constant used in computation

#define S 5E6	// sample pts done in a slave, total N is 1E8


// gobal variable
double total = 0;	// final result

// required workpool functions

void init(int *tasks, int *data_items, int *result_items) {
	*tasks = T;
	*data_items = D;
	*result_items = R;
	return; 
}

void diffuse(int taskID,double output[D]) {
						// taskID not used in computation
	static int temp = 0;			// only initialized first time function called
	output[0] = ++temp;			// set seed to consecutive data value
	return;
}
	
void compute(int taskID, double input[D], double output[R]) {

	int i;
	double x, y;	
	double inside = 0;

	srand(input[0]);			// initialize random number generator
	for (i = 0; i < S; i++) {
		x = rand() / (double) RAND_MAX;
		y = rand() / (double) RAND_MAX;
		if ( (x * x + y * y) <= 1.0 ) inside++;
	}
	output[0] = inside;
	return;
}

void gather(int taskID, double input[R]) {

	total += input[0];			// aggregate answer
	return;
}

// additional routines used in this application

double get_pi() {

	double pi;
	pi = 4 * total / (S*T);
	printf("\nWorkpool results, Pi = %f\n",pi); 		// print out workpool results
	return; 
}

int main(int argc, char *argv[]) {
				// All variables declared here are in every process
	int i;
	int P;			// number of processes, set by SZ_Init(P) 		
	double time1, time2; 	// for timing		

	SZ_Init(P);		// initialize MPI environment, sets P to number of processes
	printf("number of tasks = %d\n",T);
	printf("number of samples done in slave per task = %.0f\n",S);

	time1 = SZ_Wtime(); 	// record time stamp
	SZ_Parallel_begin	// start of parallel section

	SZ_Workpool(init,diffuse,compute,gather);
	
	SZ_Parallel_end;	// end of parallel
	time2 = SZ_Wtime(); 	// record time stamp

	get_pi();		// calculate final result
	printf("elapsed_time = %f (seconds)\n", time2 - time1);

	SZ_Finalize(); 

	return 0;
}