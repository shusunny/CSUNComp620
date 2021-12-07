/*===================================================
  CSUN COMP 620 Fall 2020 Exam-1 Sep. 30, 2020
  
I (LastName): Sun     (FirstName): Zhen    pledge by
honor that I will not talk, text, chat and search
the web during this online exam with anybody or via
any device (phone,internet, ….).
     
Write an efficient parallel rank sort program using
p processors that reads n (non distinct) integers
from an input file.

 To compile  : mpicc xxxxxE1.c 
 To execute  : mpiexec -n p a.out n  anyinputfilename
 p is number of processors
 n is number of inputs, and is divisile by p

Save your exam in xxxxxE1.c, where xxxxx is at
most the first 5characters of your last name.
Upload this file in canvas Exam-1, before 9:45 p.m.
9/30/20.  
NOTE: In fairness to all, absolutely no file will
be accepted by email. 
A file will not be graded if has no name at the
beginning and has compile/execution error.

 
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
 void rank(int a[], int b[], int n){
	int i, j; 
	int *rank;
	rank = calloc(n, sizeof(int)); 
	for(i=0; i<n; i++)
	{
		for(j=0; j<i; j++)
		{
			if(a[i] >= a[j])
				rank[i]++;
			else
				rank[j]++;
		}	
	}
	
	for(i = 0; i < n; i++){
		b[rank[i]] = a[i];
	}
 } // end rank


int main(int argc, char **argv){
  int i, n, p, k, id, err;
  int *arr, *rnk;
  int *srt;

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
      "TO COMPILE mpicc xxxxxE1.c\n\t"
      "TO RUN mpirun -n p a.out n inputfilename\n\t"
	  "p is number of processors\n\tn is number of inputs, and is divisile by p");
	// end if id == 0
	MPI_Finalize();  // terminate
	return 0;
  } // end id == argc < 3

  n = atoi(argv[1]);  // get n 
  k = n/p; // k integers per processor

  //allocate a[] to store input
  arr   = malloc(n * sizeof(int));
  srt 	= malloc(n * sizeof(int));

  //allocate rnk[] to store rank computed by
  //each processsor
  rnk = malloc(k * sizeof(int));

  // Master reads Data from input file.
      if (id == 0) {
		printf("\tExam-1 (LastName): Sun  (FirstName): Zhen  \n\t");
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

	// Complete the rest........
	// Calling rank method
	rank(arr, srt, n);

	if (id == 0) {
		printarr(srt, n, "After Sorting");
		printf("\t\n");
	}

	free(arr);
	free(rnk);
	MPI_Finalize();
	return 0;
   } // end main
