/*===================================================
Parallel program to read n integer numbers from an input file,
storing them in an array and applying rank sort with p processors.

 To compile  : mpicc filename.c 
 To execute  : mpiexec -n p a.out n inputfilename
 p is number of processors
 n is number of inputs, and is divisile by p
===================================================*/
#include <stdio.h> 		 
#include <malloc.h>
#include <mpi.h>  		
#include"stdlib.h"
#define mpc MPI_COMM_WORLD

void printarr(int a[], int n, char *msg){
    int i;
	printf("\n\t%s\n\t", msg); 
    for(i = 0; i < n; i++){
	  printf("%3d ", a[i]); 
	  if ((i+1) % 10 == 0) printf("\n\t");
	}
 } // end printarr
 
 // Complete rank method
 void rank(int a[], int rnk[], int n, int id, int p){
	int i, j; 
  int k, start, end;

  k = n/p;
  start = id*k;
  end = start + k;

 // perform Stable sorting
  for(k = 0, i = start; i < end; i++, k++){
    int cnt = 0; // counter
    int tmp = a[i];
    for(j = 0; j < i; j++)
    if (a[j] <= tmp) cnt++;
    for(j = i+1; j < n; j++)
    if (a[j] < tmp) cnt++;
    rnk[k] = cnt;
  } 
 } // end rank


int main(int argc, char **argv){
  int i, n, p, k, id, err;

  // initialize MPI_Init
  err = MPI_Init(&argc, &argv);
  if (err != MPI_SUCCESS){
	printf("\nError initializing MPI.\n");
	MPI_Abort(mpc, err);
  } // end if

  // Get No. of processors
  MPI_Comm_size(mpc, &p);

  // Get processor id
  MPI_Comm_rank(mpc, &id);

  if (argc < 3) {
	if (id == 0) // Only master prints....
	  printf("\n\tOOOPS...., INVALID No of Arguements,\n"
	  "\tParallel program to perform rank sort,"
	  "Using p Processors. The program reads n\n\t" "non distinct integers from any input file,\n\t"
      "TO COMPILE mpicc filename.c\n\t"
      "TO RUN mpirun -n p a.out n inputfilename\n\t"
	  "p is number of processors\n\tn is number of inputs, and is divisile by p");
	// end if id == 0
	MPI_Finalize();  // terminate
	return 0;
  } // end id == argc < 3

  n = atoi(argv[1]);  // get n
  k = n/p; // k integers per processor

  //allocate a[] to store input
  int *arr = malloc(n * sizeof(int));
  int *srt 	= malloc(n * sizeof(int));
  int *ranks = malloc(n * sizeof(int));

  //allocate rnk[] to store rank computed by
  //each processsor
  int *rnk = malloc(k * sizeof(int));

  // Master reads Data from input file.
  if (id == 0) {
		FILE *inf;
		char *fname;
        fname = argv[2];  // get n
		inf = fopen(fname,"r");	// open finput file	
		for(i = 0; i < n; i++)// read n numbers 
			fscanf(inf, "%d", &arr[i]);
		// end for
		fclose(inf); // close finput file
		// print input array
		printarr(arr, n, "Input Before Sorting");
	  } // end id = 0
	// Broadcast data
  MPI_Bcast(arr, n, MPI_INT, 0, mpc);

	// Calling rank method
	rank(arr, rnk, n, id, p);

  // Gather ranks
  MPI_Gather(rnk, k, MPI_INT, ranks, k, MPI_INT, 0, mpc);

	if (id == 0) {
    for (i=0; i<n; i++){
      srt[ranks[i]]=arr[i];
    }
		printarr(srt, n, "After Sorting");
		printf("\t\n");
	}

	free(arr);
	free(rnk);
  free(ranks);
	MPI_Finalize();
	return 0;
   } // end main
