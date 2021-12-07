/*
Write an efficient parallel program using p processors to implement odd-even sort, to
sort n numbers in ascending order. Assume n >> p and is divisible by p.
Please name your file xxxxxp11.c, where xxxxx is at most the first 5 characters of your
last name and p11 is the program number. Please print initial input before and after
sorting.
Please PRINT your name at the end of OUTPUT.
Uploaded your program in canvas before the due date.
To execute: mpiexec -n p a.out n inputfile
Example: mpiexec -n 4 a.out 100 inputfile
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> 
#define mpc MPI_COMM_WORLD

  /* The IncOrder function that is called by qsort is defined as follows */ 
  int IncOrder(const void *e1, const void *e2) 
  { 
    return (*((int *)e1) - *((int *)e2)); 
  }
  
  // printarr function
  void printarr(int a[], int n, char *msg){
    int i;
	printf("\n\t%s\n\t", msg); 
    for(i = 0; i < n; i++){
	  printf("%3d ", a[i]); 
	  if ((i+1) % 10 == 0) printf("\n\t");
	}
  } // end printarr

  int main(int argc, char **argv) 
  { 
    int n, i, p, err;         
    int nlocal;    /* The local number of elements, and the array that stores them */ 
    int *elmnts;   /* The array that stores the local elements */ 
    int *relmnts;  /* The array that stores the received elements */ 
    int myrank;    /* The rank(id) of the process */ 
    int oddrank;   /* The rank of the process during odd-phase communication */ 
    int evenrank;  /* The rank of the process during even-phase communication */ 
    int *wspace;   /* Working space during the compare-split operation */ 
    int *arr, *srt;      /* Array to store the initial and final result */
    MPI_Status status; 
 
    /* Initialize MPI and get system information */ 
    err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS){
      printf("\nError initializing MPI.\n");
      MPI_Abort(mpc, err);
    } // end if
    
    MPI_Comm_size(mpc, &p); 
    MPI_Comm_rank(mpc, &myrank); 
 
    n = atoi(argv[1]); 
    nlocal = n/p; /* Compute the number of elements to be stored locally. */ 
    
    /* Allocate memory for the various arrays */ 
    elmnts  = (int *)malloc(nlocal*sizeof(int)); 
    relmnts = (int *)malloc(nlocal*sizeof(int)); 
    wspace  = (int *)malloc(nlocal*sizeof(int)); 
    arr = (int *)malloc(n*sizeof(int));
    srt = (int *)malloc(n*sizeof(int));
    
    /* read the input data */
    if (myrank == 0) {
      FILE *inf;
      char *fname;
      fname = argv[2];  // get n
      inf = fopen(fname,"r");	// open finput file	
      for(i = 0; i < n; i++)// read n numbers and print them formatted 10 per line
        fscanf(inf, "%d", &arr[i]);
      
      fclose(inf); // close finput file
      printarr(arr, n, "orginal array:");
    }

    /* scatter arr to n processor */
    MPI_Scatter(arr, nlocal, MPI_INT, elmnts, nlocal, MPI_INT, 0, mpc);
    /* Sort the local elements using the built-in quicksort routine */ 
    qsort(elmnts, nlocal, sizeof(int), IncOrder); 
 
    /* Determine the rank of the processors that myrank needs to communicate during */ 
    /* the odd and even phases of the algorithm */ 
    if (myrank%2 == 0) { 
      oddrank  = myrank-1; 
      evenrank = myrank+1; 
    } 
    else { 
      oddrank  = myrank+1; 
      evenrank = myrank-1; 
    } 
 
    /* Set the ranks of the processors at the end of the linear */ 
    if (oddrank == -1 || oddrank == p) 
      oddrank = MPI_PROC_NULL; 
    if (evenrank == -1 || evenrank == p) 
      evenrank = MPI_PROC_NULL; 
 
    /* Get into the main loop of the odd-even sorting algorithm */ 
    for (int iter=0; iter<p-1; iter++) { 
      if (iter%2 == 1) /* Odd phase */ 
        MPI_Sendrecv(elmnts, nlocal, MPI_INT, oddrank, 1, relmnts, nlocal, MPI_INT, oddrank, 1, mpc, &status); 
      else /* Even phase */ 
        MPI_Sendrecv(elmnts, nlocal, MPI_INT, evenrank, 1, relmnts, nlocal, MPI_INT, evenrank, 1, mpc, &status); 

      for(i=0;i<nlocal;i++){
        wspace[i]=elmnts[i];
      }
      if(status.MPI_SOURCE==MPI_PROC_NULL)	continue;
      else if(myrank<status.MPI_SOURCE){
        //store the smaller of the two
        int i,j,k;
        for(i=j=k=0;k<nlocal;k++){
          if(j==nlocal||(i<nlocal && wspace[i]<relmnts[j]))
            elmnts[k]=wspace[i++];
          else
            elmnts[k]=relmnts[j++];
        }
      }
      else{
        //store the larger of the two
        int i,j,k;
        for(i=j=k=nlocal-1;k>=0;k--){
          if(j==-1||(i>=0 && wspace[i]>=relmnts[j]))
            elmnts[k]=wspace[i--];
          else
            elmnts[k]=relmnts[j--];
        }
      }//else 
    }//for

    // gather the sort array to srt
    MPI_Gather(elmnts, nlocal, MPI_INT, srt, nlocal, MPI_INT, 0, mpc);
    
    if (myrank== 0){
      printarr(srt, n, "sorted array:");
      printf("\n\tend oddeven sort.\n\tby Zhen Sun\n");
    }
    // free memory
    free(elmnts); free(relmnts); free(wspace); 
    free(arr); free(srt);
    MPI_Finalize(); 

    return 1;
  } 
