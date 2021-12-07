// Basic heat distribution program to demostrate synchronous stencil program. gen_heat.c    B. Wilkinson Dec 28, 2015
// simplistic version with each process doing one point

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "suzaku.h"  	// Basic Suzaku macros

#define D 1	// # of data items in slave data
#define P 16	// Number of processes -- this code must be run only with this number of processes
#define N 6 	// Number of pts in each dimension, to include border 6 x 6
#define M 4	// Number of pts in each dimension, not including border 4 x 4

int main(int argc, char *argv[])  {
	int i,j,x,y,t;				// loop counters
	int T = 100;				// time period
	int p, pid;

	double pts[N][N];			// array of points to include fixed borders
	double A[1];				// point being computed in slave, output array
	double B[P][D];				// input array
	double temp[M][M];			// hardcoded for 4 x 4

	double pts_seq[2][N][N];		// array to do computation sequentailly.
	int current = 0;
	int next = 1;

	SZ_Init(p);				// initialize MPI message-passing environment
	if (p != P) printf("ERROR Program must be run with %d processes\n",P);
	printf("Number of points in each dimension = %d\n",N);
	printf("Number of time steps = %d\n",T);

/* -------------------------- Set up inital values ---------------*/
	for (i = 0; i < N; i++)			// load inital values into array
		for(j = 0; j < N; j++)		// border and inner points = 20		
			pts[i][j] = 20;		// note C row major order, row i, col j
	for (i = 2; i < N-2; i++)
		pts[0][i] = 100.0;		// top row = 100
		
	printf("Initial numbers");		// print numbers
	for (i = 0; i < N; i++)
		for(j = 0; j < N; j++) {
			if (j == 0) printf("\n");
			printf("%7.2f",pts[i][j]);
		}
	printf("\n");

	// compute values sequentially to check with parallel result, done using Jacobi iteration

	for (i = 0; i < N; i++)			// load inital values into array
	for (j = 0; j < N; j++) {
		pts_seq[current][i][j] = pts[i][j];
		pts_seq[next][i][j] = pts[i][j];
	}
	for (t=0; t < T; t++)	{  		// do computation sequentially, using Jacobi iteration
	   for (i=1; i < N-1; i++)
	   for (j=1; j < N-1; j++)
		pts_seq[next][i][j] = 0.25 * (pts_seq[current][i-1][j] + pts_seq[current][i+1][j] + pts_seq[current][i][j-1] + pts_seq[current][i][j+1]);
	   current = next;
	   next = 1 - current;
	}
	
/* -------------------------Computation-----------------------------------*/
	
	SZ_Parallel_begin		// parallel section, all processes do this

		SZ_Pattern_init("stencil",D);	// set up slave interconnections

		SZ_Broadcast(pts);		// synchronous, includes a barrier
						// Set up initial values in each slave
		pid = SZ_Get_process_num();	
		x = pid / M;  			// row, hardcoded for 16 processes 4 x 4
		y = pid % M;  			// column
		i = x + 1;			// location in pts[][]
		j = y + 1;
		A[0] = pts[i][j];		// copy location in pts[][] into A[0]
		B[0][0] = pts[i][j-1];		// left
		B[1][0] = pts[i][j+1];		// right
		B[2][0] = pts[i-1][j];		// up
		B[3][0] = pts[i+1][j];		// down
	
		for (t = 0; t < T; t++) {	// compute values over time T

			A[0] = 0.25 * (B[0][0] + B[1][0] + B[2][0] + B[3][0]);	// slaves execute computation, master acts as one slave
	
	   		SZ_Generalized_send(A,B); // sent compute results in A to B in connected slaves
		}

		SZ_Gather(A,temp);		// collect results from slaves (A), into array temp, gather()
	
	SZ_Parallel_end;			// end of parallel
	
/* ------------------------- Results -----------------------------------*/

	for (x = 0; x < N; x++)  {		// update inside points
		for (y = 0; y < N; y++) {
		    if ((x > 0) && (x < N-1) && (y > 0) && (y < N-1)) {  // inside point
			i = x - 1; 
			j = y - 1;
			pts[x][y] = temp[i][j];
		    }
		}
	}
	printf("Final numbers");		// print numbers
	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if (j == 0) printf("\n");
			printf("%7.2f",pts[i][j]);
		}
	}
	printf("\n");

	int error = 0; 				// check sequential and parallel versions give same answers
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)   {
		  if ((pts[i][j] - pts_seq[current][i][j] > 0.001) || (pts_seq[current][i][j] - pts[i][j] > 0.001))
     			{ error = -1; break;}
		}
		if (error == -1) break;
	}

	if (error == -1) printf("ERROR, sequential and parallel versions give different answers\n");
	else printf("Sequential and parallel versions give same answers within +-0.001\n");

	SZ_Finalize(); 

	return 0;
}

