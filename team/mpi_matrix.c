#define N 256
#include <stdio.h>
#include <time.h>
#include "mpi.h"

int main(int argc, char *argv[])	{
	int i, j, k, error = 0;
	static double A[N][N], B[N][N], C[N][N], D[N][N], sum;  // arrays declared statically so as not on the stack

	double time1, time2; 		// for timing

	MPI_Status status; 		// MPI variables
	int rank, P, blksz;

	MPI_Init(&argc, &argv);  		// Start MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	if ((rank == 0) && (N % P != 0)) {
		printf("Error -- N/P must be an integer\n"); // should really stop now
	}

	blksz = N/P;

	if (rank == 0) {
		printf ("N = %d P = %d\n",N, P);
		for (i = 0; i < N; i++) {   	// set some initial values for A and B
			for (j = 0; j < N; j++) {
				A[i][j] = j*1;
				B[i][j] = i*j+2;
			}
		}
		for (i = 0; i < N; i++) {   	// sequential matrix multiplication
			for (j = 0; j < N; j++)   {
				sum = 0;
				for (k=0; k < N; k++) {
					sum += A[i][k]*B[k][j]; 
				}      
				D[i][j] = sum;
			}
		}
		time1 = MPI_Wtime(); 	// record  time stamp
	}

	MPI_Scatter(A, blksz*N, MPI_DOUBLE, A, blksz*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);// Scatter input matrix A
	MPI_Bcast(B, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD); // Broadcast the input matrix B

	for(i = 0 ; i < blksz; i++) {
		for(j = 0 ; j < N ; j++) {
			sum = 0;
			for(k = 0 ; k < N ; k++) {
				sum += A[i][k] * B[k][j];
			}
			C[i][j] = sum;
		}
	}

	MPI_Gather(C, blksz*N, MPI_DOUBLE, C, blksz*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if(rank == 0) {
		time2 = MPI_Wtime(); 	// record  time stamp

		int error = 0;		// check sequential and parallel versions same answers, within rounding
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++)   {
				if ( (C[i][j] - D[i][j] > 0.001) || (D[i][j] - C[i][j] > 0.001)) error = -1;
			}
		}
		if (error == -1) printf("ERROR, sequential and parallel code give different answers.\n");
		else printf("Sequential and parallel code give same answers.\n");

		printf("elapsed_time = %lf (seconds)\n", time2 - time1);  // print out execution time
	}
	MPI_Finalize(); 
	return 0;
}