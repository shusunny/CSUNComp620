 /*===================================================
                CSUN COMP 620 Fall 2020
 	Parallel program that reads n integers from
 	input file and finds largest, using p processors,
 	and using ONLY Send/Recv.
 	
  FileName    : HWK-1.c  
  Author      : Gh. Dastghaibyfard
  Version     :
  Date		 : Sep. 30, 2020
  To compile  : mpicc HWK-1.c 
  To execute  : mpiexec -n p a.out n  anyinputfilename
  p is number of processors
  n is number of inputs, and is divisible by p
 ===================================================*/
 #include <stdio.h> 		 
 #include <malloc.h>
 #include <time.h> 
 #include <mpi.h>  		
 #include"stdlib.h"
 #define mpc MPI_COMM_WORLD
 
 	//maxx finds largest of an array a[] 
 	int maxx(int a[], int n){
 		int i, max;
 		
 		// Assume a[0] is max 
 		max = a[0];
 		for(i = 1; i < n; i++)
 		  if (a[i] > max) max = a[i];     
 		
 		return max;
 	} // end maxx
 
     int main(int argc, char **argv){
       int n, p, k, id, max, err;
 	  MPI_Status status;
 
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
 		   "\tParallel program that reads n integers "
 		   "from input file and finds largest,\n\t"
 		   "using p processors,and ONLY Send/Recv.\n\t"
 		   "\tand finding largest of these integers in "
 		   "parallel using ONLY Send/Recvd Reduce...\n"
 		   "\tTO RUN	mpirun -n p a.out n inputfilename\n"
 		   "\t\tp (integer) No.of Processors...\n"
 		   "\t\tn (integer) No. of inputs\n"
 		  "\t\tNote: Assume n is divisible by p.\n\n");
 	          // end if id == 0
 	     MPI_Finalize();  // terminate
 	     return 0;
       } // end id == argc < 3
 
       n = atoi(argv[1]);  // get n 
 	   k = n/p; //k integers per processor
 
       // Master reads n integers from input file
       if (id == 0) {
 		FILE *inf;
 		
 		//allocate arr[] for initial input
 		int i, *arr = malloc(n * sizeof(int));
 		
 		//allocate maxarr[] for each processor data
 		int *maxarr = malloc(p * sizeof(int));
 		
 		//Get input filename
 		char *fname;
              fname = argv[2];  // get input file name
 		
 		// open finput file
 		inf = fopen(fname,  "r");
 		
 		//read n integers
 		for(i = 0; i < n; i++){
 			fscanf(inf, "%d", &arr[i]);
			printf("%6d ", arr[i]); 
          	if ((i+1) % 10 == 0) printf("\n");
 		}// end for
 		
 		// close finput file
 		fclose(inf);
 		
 		for(i = 1; i < p; i++){   //p-1 Send/Recvs		 
 		  //send K integers to processors 1,2,..p-1
 		  MPI_Send(arr+i*k, k, MPI_INT, i, i, mpc);
 	
 		  //Recv largest from processors 1,2,..p-1
 		  MPI_Recv(&maxarr[i], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, mpc, &status); 
 		} // end for
 		
 		//find largest for master
 		maxarr[0] = maxx(arr, k);	
 
 		//find largest from p largest
 		max = maxx(maxarr, p);
 
 		//print Largest in input
 		printf("\nLargest in input = %d.\n", max);
 		
 		//free maxarr
         free (maxarr);
 
 		//free arr
         free (arr);
       } // end id == 0
     else{
 		//p-1 processors will execute in parallel
 		//allocate myarr[] for k integers
 		int *myarr = malloc(k * sizeof(int));
 
 		//Recv k integers from master
 		MPI_Recv(myarr, k, MPI_INT, 0, id, mpc, &status);
 
 		//Find largest for processors 1,2,..p-1
 		max = maxx(myarr, k);

		printf("Max computed by processor %2d is: %10d\n", id, max); 
 		//Send largest to master(p-1 Sends)		
 		MPI_Send(&max, 1, MPI_INT, 0, id, mpc);
 
 		free (myarr);  //free myarr
     } // end else part
 	MPI_Finalize();
 	return 0;
    } // end main
