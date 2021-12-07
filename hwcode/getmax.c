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

 int getmax (int a[], int n){
    int i, max;
    // Add my portion Of data
    max = a[0];
    for(i = 1; i < n; i++)
	    if (a[i] > max)
        max = a[i];
         
    return max;
  } // end getmax

    int main(int argc, char **argv){
      int i, p, id, k, n, max, err;
    //  int *arr;
    //  int *maxarr;

      // initialize MPI_Init
      MPI_Status status;
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

      n = atoi(argv[1]);  // get n      
      k = n/p;

      // Master Generates random Data
      if (id == 0) {
        int *arr = malloc(n*sizeof(int));
        int *maxarr = malloc(p*sizeof(int));
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
         
        // Process 0 Broadcast data
        for (i = 1; i < p; i++) { 
              // send k ints to processor i
              MPI_Send(arr+i*k, k, MPI_INT, i, i, mpc); 
              // recv max and store in maxarr;
              MPI_Recv(&maxarr[i], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, mpc, &status);
            }
          // master find it's own max
          maxarr[0] = getmax(arr, k);
          printf("Max computed by processor %2d is: %10d\n", id, maxarr[0]); 
          max = getmax(maxarr, p);
          printf("\nThe final max is %d.\n", max);

          // free memory
          free(arr);
          free(maxarr);
        } // end id == 0
        else
        { 
          int mymax;
          //int arr2[100];
          int *arr2 = malloc(k*sizeof(int));
          MPI_Recv(arr2, k, MPI_INT, 0, id, mpc, &status);
          // Compute my startpt and endpt
          // Add my portion Of data
          mymax = getmax(arr2, k);
          printf("Max computed by processor %2d is: %10d\n", id, mymax); 
          MPI_Send(&mymax, 1, MPI_INT, 0, id, mpc); 
          
          free(arr2);
        }
     
      MPI_Finalize();
      return 0;
    } // end main
