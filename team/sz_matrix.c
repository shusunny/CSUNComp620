// Suzaku Workpool pattern version 1 -- Matrix Multiplication matrixmult_workpool.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "suzaku.h" 

// required Suzaku constants

#define T 9		// number of tasks, max = INT_MAX - 1
#define D 6		// number of data items in each task, 3 elements of row A and 3 elements of column B
#define R 1		// number of data items in result of each task

// gobal variables

#define N 3

double A[N][N], B[N][N], C[N][N], Cseq[N][N];

// required workpool functions

void init(int *tasks, int *data_items, int *result_items) {
	*tasks = T;
	*data_items = D;
	*result_items = R;
	return; 
}

void diffuse(int taskID,double output[D]) {	// useds same approach as Seeds sample but inefficient copying arrays
						// taskID used in computation
	int i;
	int a, b;

	a = taskID / N;
	b = taskID % N;
	for (i = 0; i < N; i++) { 			//Copy one row of A and one column of B into output
		output[i] = A[a][i];
		output[i+N] = B[i][b];
	}
	return;
}
	
void compute(int taskID, double input[D], double output[R]) {

	int i;
	output[0] = 0;
	for (i = 0; i < N; i++) {
		output[0] += input[i] * input[i+N];
	}
	return;
}

void gather(int taskID, double input[R]) {

	int a,b;
	a = taskID / 3;
	b = taskID % 3;
	C[a][b]= input[0];

	return;
}

// additional routines used in this application

void initialize() { // initialize arrays 

	int i,j;
	for (i = 0; i < N; i++){
		for(j = 0; j < N; j++) {
			A[i][j] = i + N * j + 1;
			B[i][j] = j + N * i + 1;
		}
	}
	return;
}

void seq_matrix_mult(double A[N][N], double B[N][N], double C[N][N]) {

	int i,j,k;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++) {
			C[i][j] = 0;
			for (k = 0; k < N; k++)
				C[i][j] += A[i][k] * B[k][j];
		}
	return;
}

void print_array(double array[N][N]) { // print out an array 

	int i,j;
	for (i = 0; i < N; i++){
		printf("\n");
		for(j = 0; j < N; j++) {
			printf("%5.2f ", array[i][j]);
		}
	}
	printf("\n");
	return;
}

int main(int argc, char *argv[]) {
				// All variables declared here are in every process
	int i;
	int P;			// number of processes, set by SZ_Init(P) 		
	double time1, time2; // use clock for timing		

	SZ_Init(P);		// initialize MPI environment, sets P to number of processes

	initialize();		// initialize input arrays
	printf("Array A");
	print_array(A);
	printf("Array B");
	print_array(B);

	seq_matrix_mult(A,B,Cseq);
	printf("Multiplication sequentially");
	print_array(Cseq);

	time1 = SZ_Wtime(); 	// record time stamp
	SZ_Parallel_begin	// start of parallel section

		SZ_Workpool(init,diffuse,compute,gather);
	
	SZ_Parallel_end;	// end of parallel
	time2 = SZ_Wtime(); 	// record time stamp

	printf("Workpool results");
	print_array(C);		// print final result
	printf("Elapsed_time = %f (seconds)\n",time2 - time1);

	SZ_Finalize(); 

	return 0;
}
