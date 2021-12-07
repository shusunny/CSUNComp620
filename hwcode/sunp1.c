#include <mpi.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 

    //Program to read n integer numbers from 
	//an input file and storing in an array
    //finding the max in Parallel  09/27/20
    //TO COMPILE mpicc programname.c
    //TO RUN  mpirun -np p a.out n inputfilename
	//p = number of processors
	//n = number of inputs

int main(int argc, char* argv[]) 
{ 
  
    int pid, np, i, n,
        elements_per_process, 
        n_elements_recieved, 
        n_recieved;
    // np -> no. of processes 
    // pid -> process id 

    int *a; // array to store numbers

    MPI_Status status; 
  
    // Creation of parallel processes 
    MPI_Init(&argc, &argv); 
  
    // find out process ID, 
    // and how many processes were started 
    MPI_Comm_rank(MPI_COMM_WORLD, &pid); 
    MPI_Comm_size(MPI_COMM_WORLD, &np); 
    
    if (pid == 0) {// to print only once....
		if (argc < 3) {
			printf("\n\tOOOPS...., INVALID No of Arguements,\n"
				"\tTO RUN	mpirun -np p a.out n inputfilename\n"
				"\t\tp (integer) No. of Processors...\n"
				"\t\tn (integer) No. of inputs (size of array)...\n"
                "\t\tinputfilename filename of source array...\n ");
	    } // end if
    } // end id == 0
    if (argc < 3) {MPI_Finalize(); return 0;} // end if
    
    // init index and arrays
    int index;
    n = atoi(argv[1]);  // get n   
    a = malloc(n * sizeof(int));
    elements_per_process = n / np; 

    // master process 
    if (pid == 0) { 
    	FILE *inf;
		char *fname;
        fname = argv[2];  // get n
		inf = fopen(fname,"r");	// open finput file	
		for(i = 0; i < n; i++){// read n numbers and print them formatted 10 per line
			fscanf(inf, "%d", &a[i]);
			printf("%6d ", a[i]); 
			if ((i+1) % 10 == 0) printf("\n");
			} // end for
		fclose(inf); // close finput file	
  
        // check if more than 1 processes are run 
        if (np > 1) { 
            // distributes the portion of array 
            // to child processes to calculate 
            // their partial maxs 
            for (i = 1; i < np - 1; i++) { 
                index = i * elements_per_process; 
  
                MPI_Send(&elements_per_process, 
                         1, MPI_INT, i, 0, 
                         MPI_COMM_WORLD); 
                MPI_Send(&a[index], 
                         elements_per_process, 
                         MPI_INT, i, 0, 
                         MPI_COMM_WORLD); 
            } 
  
            // last process adds remaining elements 
            index = i * elements_per_process; 
            int elements_left = n - index; 
  
            MPI_Send(&elements_left, 
                     1, MPI_INT, 
                     i, 0, 
                     MPI_COMM_WORLD); 
            MPI_Send(&a[index], 
                     elements_left, 
                     MPI_INT, i, 0, 
                     MPI_COMM_WORLD); 
        } 
  
        // master process add its own sub array 
        int max = 0; 
        for (i = 0; i < elements_per_process; i++) 
            if (a[i] > max)
            max = a[i];     
        printf("Max computed by processor %2d is: %10d\n", pid, max);
  
        // collects partial maxs from other processes 
        int tmp; 
        for (i = 1; i < np; i++) { 
            MPI_Recv(&tmp, 1, MPI_INT, 
                     MPI_ANY_SOURCE, 0, 
                     MPI_COMM_WORLD, 
                     &status); 
            int sender = status.MPI_SOURCE; 
            if (tmp > max)
            max = tmp;
        } 
  
        // prints the final max of array 
        printf("Final Max of array is : %d\n", max); 
    } 
    
    // slave processes 
    else {
        int a2[1000];  
        MPI_Recv(&n_elements_recieved, 
                 1, MPI_INT, 0, 0, 
                 MPI_COMM_WORLD, 
                 &status); 
        // stores the received array segment 
        // in local array a2 
        MPI_Recv(&a2, n_elements_recieved, 
                 MPI_INT, 0, 0, 
                 MPI_COMM_WORLD, 
                 &status); 
  
        // calculates its partial max 
        int partial_max = 0; 
        for (int i = 0; i < n_elements_recieved; i++) 
            if (a2[i] > partial_max)
            partial_max = a2[i];   
        printf("Max computed by processor %2d is: %10d\n", pid, partial_max);
        // sends the partial max to the root process 
        MPI_Send(&partial_max, 1, MPI_INT, 
                 0, 0, MPI_COMM_WORLD); 
    } 
  
    // cleans up all MPI state before exit of process 
    MPI_Finalize(); 
  
    return 0; 
} 