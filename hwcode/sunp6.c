 /*===================================================
Write an efficient pipeline parallel c program to Solve a System of Linear Equations
(upper-triangular form) with n unknowns using p processors as explained in Wilkinson
Book, chapter 5. Please name your file xxxxxp6.c, where xxxxx is at most the first 5
characters of your last name and p6 is the program number. Make sure to print input
as well as value of unknowns in a user-friendly form.
Uploaded your program in canvas before due date. Assume n is divisible by p.

To execute: mpiexec -n p a.out n anyinputfile

===================================================*/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <mpi.h> 

#define mpc MPI_COMM_WORLD


int main(int argc, char **argv) { 
    int id, p, err;
    int i, j, n, k;
    int c; 
    float sum;
    float *a, *b, *x, *x1;
    
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
    
   if (argc < 3) {
        if (id == 0) // Only master prints....
            printf("\n\tINVALID No of Arguements,\n\t"
            "TO COMPILE mpicc program.c\n\t"
            "TO RUN mpirun -n p a.out n inputfilename\n\t"
            "p is number of processors\n\t"
            "n is number of inputs\n\t");
            // end if id == 0
        MPI_Finalize();  // terminate
        return 0;
    } // end if
    
    // read n and allocate space
    n = atoi(argv[1]); 
    x = malloc(n*sizeof(float));
    x1 = malloc(n*sizeof(float)); // x1 gather the result
    b = malloc(n*sizeof(float));
    a = malloc(n*n*sizeof(float));
    k = n/p;
    
    // master read inputs
    if (id == 0) {
	    FILE *inf;
		char *fname;
        fname = argv[2];  // get n
		inf = fopen(fname,"r");	// open finput file	
        for (i = 0; i < n; i++){
            for (j = 0; j < n-i; j++){
                fscanf(inf,"%f", &a[(n-i-1)*n+j]); //reading a[i][j]
                printf("a[%d][%d] = %f\t", (n-i-1), j, a[(n-i-1)*n+j]);
            // end for
            }
            fscanf(inf,"%f", &b[n-i-1]);
            printf("b[%d] = %f\n\n", (n-i-1), b[(n-i-1)]);
        } // end outer for
        fclose(inf);
    }
    
    MPI_Bcast(a, n*n, MPI_FLOAT, 0, mpc);
    MPI_Bcast(b, n, MPI_FLOAT, 0, mpc);
    float myx;
    
    for (i=0; i<k; i++) {
        if(id != 0){
            sum = 0;
            c = i*p; // get current work group with p number in each group
            for (j = 0; j < c+id; j++){
                MPI_Recv(&x[j], 1, MPI_FLOAT, id - 1, 0, mpc, &status); // p-1 Recv’s
                MPI_Send(&x[j], 1, MPI_FLOAT, (id + 1) % p, 0, mpc); // p Send’s
                sum = sum + a[(c+id)*n+j]*x[j];
            }
            x[c+id] = (b[c+id] - sum) / a[(c+id)*n + c+id];
            myx = x[c+id];
            MPI_Send(&x[c+id], 1, MPI_FLOAT, (id + 1) % p, 0, mpc); // p Send’s
            
        }
        if (id == 0) {
            sum = 0;
            c = i*p; // get current work group with p number in each group
            for (j = 0; j < c+id; j++){
                MPI_Recv(&x[j], 1, MPI_FLOAT, p-1, 0, mpc, &status); // master receive from p-1
                MPI_Send(&x[j], 1, MPI_FLOAT, (id + 1) % p, 0, mpc); // p Send’s
                sum = sum + a[(c+id)*n+j]*x[j];
            }
            x[c+id] = (b[c+id] - sum) / a[(c+id)*n + c+id];
            myx = x[c+id];
            MPI_Send(&x[c+id], 1, MPI_FLOAT, (id + 1) % p, 0, mpc); // p Send’s
            
        }  // end if
        MPI_Gather(&myx, 1, MPI_FLOAT, x1+c, 1, MPI_FLOAT, 0, mpc);
    }

    // master output the x;
    if (id == 0){
        for (i=0; i<n; i++) 
            printf("Process %d calculated x[%d]= %f\n", i%p, i, x1[i]);
    }
    // free memory and finalize mpi
    free(a);
    free(b);
    free(x);
    free(x1);

    MPI_Finalize();
    return 0;
}