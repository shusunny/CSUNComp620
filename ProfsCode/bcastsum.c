#include <stdio.h> 		 
#include <malloc.h>
#include <time.h> 
#include <mpi.h>  		
#include"stdlib.h"

#define mpc MPI_COMM_WORLD
    //Program to read n integer numbers from 
	//an input file and storing in an array
    //adding these no.'s in Parallel  09/21/20
    //TO COMPILE mpicc bcast.c
    //TO RUN	 mpirun -n p a.out n inputfilename
	// p is number of processors
	// n is number of inputs, and is divisile by p

 int getsum (int a[], int n, int id, int p){
    int i, k, sum, startpt, endpt;
    // Compute my startpt and endpt
    k = n / p; //p is no. of processors
    startpt = id * k;
    endpt   = startpt  + k;
    // Add my portion Of data
    sum = 0;
    for(i = startpt; i < endpt; i++)
	  sum += a[i];     
    printf("Sum computed by processor %2d is: %10d\n", id, sum);
    return sum;
  } // end getsum

    int main(int argc, char **argv){
      int i, p, id, mysum, finalsum;
      int *arr, n, err;

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

      if (id == 0) {// to print only once....
	   if (argc < 3) {
		printf("\n\tOOOPS...., INVALID No of Arguements,\n"
		"\tProgram to read n integer numbers from any input file\n"
		"\tand adding these numbers in parallel using Bcast and Reduce...\n"
		"\tTO RUN	mpirun -n p a.out n inputfilename\n"
		"\t\tp (integer) No.of Processors...\n"
		"\t\tn (integer) No. of inputs (size of array)...\n"
		"\t\tNote: Assume n is divisible by p.\n\n");
	   } // end if
      } // end id == 0

      if (argc < 3) {
		  MPI_Finalize(); return 0;
	  } // end if

      n   = atoi(argv[1]);  // get n      
      arr = malloc(n * sizeof(int));//allocate space

      // Master Generates random Data
      if (id == 0) {
		FILE *inf;
		char *fname;
        fname = argv[2];  // get n
		inf = fopen(fname,"r");	// open finput file	
		for(i = 0; i < n; i++){// read n numbers and print them formatted 10 per line
			fscanf(inf, "%d", &arr[i]);
			printf("%6d ", arr[i]); 
			if ((i+1) % 10 == 0) printf("\n");
			} // end for
		fclose(inf); // close finput file	
      } // end id == 0

      // Broadcast data
      MPI_Bcast(arr, n, MPI_INT, 0, mpc);

      // Compute my portion Of input	  
	   mysum = getsum(arr, n, id, p);

      // Compute global sum
      MPI_Reduce(&mysum, &finalsum, 1, MPI_INT, MPI_SUM, 0, mpc);

      if (id == 0){
		printf("\nThe final sum is %d.\n", finalsum);
      } // end id == 0

      // --free allocated spaces
      free (arr);//free allocated space for array a

      MPI_Finalize();
      return 0;
    } // end main
