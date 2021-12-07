#include <stdio.h> 		 
#include <malloc.h>
#include <time.h> 
#include <mpi.h>  		
#include"stdlib.h"

    /* 
    method 1: slaves get local sums, master compute total sum, then do final collapse
    method 2: slaves do final collapse of each number, master add these and do final
    method 3: slaves do final collapse of nums received, then send 1 final number to master, 
              master add and do final.
    method 4: master do the ultimate collapse of each num, then sum, then do final collapse.
              should get same result with method 2, but only one process works.
    method 5: master sum all numbers then do final collapse. same with method 1, only master.
    */

#define mpc MPI_COMM_WORLD
       
    //Program to read n integer numbers from 
	//an input file and storing in an array,
    //doing collapse of digits of these numbers 
    //in different ways. (by Zhen Sun)

    //TO COMPILE mpicc bcast.c
    //TO RUN	 mpirun -n p a.out n inputfilename m
	// p is number of processors
	// n is number of inputs, n should divisible by p
    // m is number of method(1-5)

// get the sum of n numbers array a[] 
uint64_t getsum(uint64_t a[], int n){
    int i;
    uint64_t sum;
    sum = 0;
    for(i = 0; i < n; i++)
	    sum += a[i];  
    return sum;
} 

// do one time collapse for n
uint64_t collapse(uint64_t n) {  
    uint64_t sum = 0; 
    while (n != 0) 
    { 
        sum += n%10; 
        n = n/10; 
    } 
    return sum; 
} 

// do ultimate collapse for n
uint64_t ult_collapse(uint64_t n){
    uint64_t sum = n; 
    while (sum > 9){
        sum = collapse(sum);
    }
    return sum;
}

// parallel get sum of n number on array a[]
uint64_t p_getsum (uint64_t a[], int n, int id, int p){
    int i, k, startpt, endpt;
    uint64_t sum;
    // Compute my startpt and endpt
    k = n / p; //p is no. of processors
    startpt = id * k;
    endpt   = startpt  + k;
    // Add my portion Of data
    sum = 0;
    for(i = startpt; i < endpt; i++)
	    sum += a[i];     
    return sum;
}

// parallel get collapse of each number in a[]
int p_collapse(uint64_t a[], int n, int id, int p, uint64_t b[]){
    int i, k, m, startpt, endpt;
    k = n / p; //p is no. of processors
    startpt = id * k;
    endpt   = startpt  + k;
    for (i = startpt, m = 0; i < endpt; i++, m++){
        b[m] = ult_collapse(a[i]);    
    }
}

// parallel get final collapse of each number in a[]
uint64_t p_ultcollapse(uint64_t a[], int n, int id, int p){
    int i, k, startpt, endpt;
    uint64_t sum;
    k = n / p; //p is no. of processors
    startpt = id * k;
    endpt   = startpt  + k;
    // Add my portion Of data
    sum = 0;
    for(i = startpt; i < endpt; i++){
        a[i] = ult_collapse(a[i]);
	    sum += a[i];
    }
    sum = ult_collapse(sum);     
    return sum;
}

int main(int argc, char **argv){
    int i, p, id, choice;
    int n, err, k;
    uint64_t *arr, *arr2;
    uint64_t *tmp;
    uint64_t partsum;
    uint64_t finalsum;
    uint64_t partnum;
    uint64_t finalnum;

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
    
    if (argc < 4) {
        if (id == 0) // Only master prints....
        printf("\n\tINVALID No of Arguements,\n\t"
        "TO COMPILE mpicc program.c\n\t"
        "TO RUN mpirun -n p a.out n inputfilename -m\n\t"
        "p is number of processors\n\t"
        "n is number of inputs, should divisible by p\n\t"
        "m is number of process method (1-5)\n");
        // end if id == 0
        MPI_Finalize();  // terminate
        return 0;
    } // end id == argc < 3

    n  = atoi(argv[1]);  // get n     
    k  = n/p;
    //allocate space
    arr = malloc(n * sizeof(uint64_t));
    arr2 = malloc(n * sizeof(uint64_t));
    
    // Master Generates random Data
    if (id == 0) {
	    FILE *inf;
		char *fname;
        fname = argv[2];  // get n
		inf = fopen(fname,"r");	// open finput file	
		for(i = 0; i < n; i++)// read n numbers and print them formatted 10 per line
			fscanf(inf, "%lu", &arr[i]);
		fclose(inf); // close finput file
    }
    
    //get method choice
    choice = atoi(argv[3]);
        switch (choice) {
            // case 1: slaves get local sums, master compute total sum, then do final collapse
            case 1: 
            // Broadcast data
            MPI_Bcast(arr, n, MPI_UINT64_T, 0, mpc);
            partsum = p_getsum(arr, n, id, p);
            // Compute global sum
            finalsum = 0;
            MPI_Reduce(&partsum, &finalsum, 1, MPI_UINT64_T, MPI_SUM, 0, mpc);

            if (id == 0){
                printf("The final sum is %lu.\n", finalsum);
                finalnum = ult_collapse(finalsum);
                printf("The final collapse is %lu\n\n", finalnum);
            } // end id == 0
            
            break;

            // case 2: slaves do final collapse of each number, master add these and do final
            case 2: 
            tmp = malloc(k * sizeof(uint64_t));
            MPI_Bcast(arr, n, MPI_UINT64_T, 0, mpc);
            p_collapse(arr, n, id, p, tmp);
            MPI_Gather(tmp, k, MPI_UINT64_T, arr2, k, MPI_UINT64_T, 0, mpc);

            // master add and get final
            if (id == 0){
                finalsum = getsum(arr2, n);
                printf("The sum of collapse of each number is %lu.\n", finalsum);
                finalnum = ult_collapse(finalsum);
                printf("The final collapse is %lu\n\n", finalnum);

                free(tmp);
            }         
            break;

            // case 3: slaves do final collapse of nums received, then send 1 final number to master, 
            //         master add and do final. 
            case 3: 
            MPI_Bcast(arr, n, MPI_UINT64_T, 0, mpc);
            partnum = p_ultcollapse(arr, n, id, p);
            finalnum = 0;
            MPI_Reduce(&partnum, &finalnum, 1, MPI_UINT64_T, MPI_SUM, 0, mpc);

            if (id == 0){
                printf("The sum of final collapse of each process is %lu.\n", finalnum);
                finalnum = ult_collapse(finalnum);
                printf("The final collapse is %lu\n\n", finalnum);
            }
                      
            break;
            
            // case 4: master do the ultimate collapse of each num, then sum, then do final collapse
            case 4: 
            if (id == 0){
                for (i=0; i<n; i++){
                //    printf("%lu\t", a[i]);
                    arr2[i] = ult_collapse(arr[i]);
                }
                
                // get the ultimate collapse
                finalsum = getsum(arr2, n);
                printf("The sum of collapse of each number is %lu\n", finalsum);
                finalnum = ult_collapse(finalsum);
                printf("The final collapse is %lu\n\n", finalnum);
            }
            
            break;

            // case 5: master sum all numbers then do final collapse
            case 5: 
            if (id == 0){
                
                // get the ultimate collapse
                finalsum = getsum(arr, n);
                printf("The final sum is %lu\n", finalsum);
                finalnum = ult_collapse(finalsum);
                printf("The final collapse is %lu\n\n", finalnum);
            }
            
            break;

            default: 
            if (id == 0)
            printf("Invalid option!\n"
                "choose method from 1 to 5\n");
        }
    
    // --free allocated spaces
    free (arr);
    free (arr2);

    MPI_Finalize();
    return 0;
} // end main