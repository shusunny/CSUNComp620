/*===================================================
CSUN COMP 620 Fall 2020 Exam-3 Nov. 29, 2020

I (LastName):  Sun    (FirstName): Zhen    pledge by
honor that I will not talk, text, chat and search
the web during this online exam with anybody or via
any device (phone,internet, ….).

The following sequential program solves one-dimensional (a[]) heat distribution problem with n 
elements based upon finite difference equation:
a[i]= .25*(a[i-1]+2*a[i]+ ai+1).*/


/*----------------------------------------------------------
(80 Pts)a-Convert it to an efficient parallel
algorithm using p processors.
Assume p is even and n (n>>p) is divisible by p.
(10 Pts)b-Compute tcomm and tcomp of your algorithm.
(10 Pts)c-What is the suitable network for your algorithm?

Save your exam in xxxxxE3.c, where xxxxx is at
most the first 5characters of your last name.

To compile : mpicc xxxxxE3.c
To execute : mpiexec -n p a.out n f1 f2 f3
p is number of processors
n is number of inputs, and
f1, f2, f3 are 3 float numbers
Example: mpiexec -n 20 a.out 1000 100.0 50.0 100.0
After compiling and testing your c program, upload this file in canvas Exam-3, before 7:00 p.m.
11/29/20.
NOTES:
NO points will be given to submission with compile error.
In fairness to all, absolutely no file will
be accepted by email.
A file will not be graded if has no name at the
beginning and has compile/execution error.
===================================================*/


/*--------------------- ANSWERS ---------------------
(10 Pts)b-Compute tcomm and tcomp of your algorithm.

for each iter:
each process makes 2 send and 2 receive of edge value and compute n/p data.
therefore:
tcomm = 4(t_startup + nt_data) = O(n)
tcomp = O(n * n/p)

(10 Pts)c-What is the suitable network for your algorithm?

For the 1-D heat problem, each processor only need to know the last value of previous processor, and first value 
of next processor. The edges only need to transfer data to middle. 
Therefore, the most suitable network is linear or chain.

*/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <mpi.h>

#define mpc MPI_COMM_WORLD
MPI_Status status;

int main(int argc, char **argv) { 
    int id, p, err;
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
    
   if (argc < 5) {
        if (id == 0) // Only master prints....
            printf("\n\tINVALID No of Arguements,\n\t"
            "TO COMPILE mpicc program.c\n\t"
            "mpiexec -n p a.out n f1 f2 f3\n\t"
            "p is number of processors\n\t"
            "n is number of points\n\t"
            "f1 f2 f3 are the heat distribution\n");
            // end if id == 0
        MPI_Finalize();  // terminate
        return 0;
    } // end if

    // init the params for data
    int n, i, mid, iter=0;
    float *a, *b, f1, f2, f3;
    n = atoi(argv[1]);
    f1 = atof(argv[2]);
    f2 = atof(argv[3]);
    f3 = atof(argv[4]);

    // master print input parameters
    if (id == 0){
        printf("n=%d, f1=%5.1f, f2=%5.1f, f3=%5.1f\n", n, f1, f2, f3);
    }
    mid = (1+n)/2; //index of middle element of arr[]
    float midv = 0.0; // store mid value

    int k = n/p;
    float *arr = calloc((n+1), sizeof(float));
    a = calloc((k+2), sizeof(float));
    b = calloc((k+2), sizeof(float));
    // init 
    if (id == 0) a[0] = f1;
    if (id == p-1) a[k+1] = f3;
    
    do{ //continue till converge
        iter++;
        // all process compute*/
        for(i=1; i <= k; i++)
            b[i] = .25 *(a[i-1]+ 2 * a[i] + a[i+1]);

        for(i=1; i <= k; i++) a[i] = b[i];

        if (id % 2) {
            // all even send last to id+1
            if (id < p - 1) MPI_Send(&a[k], 1, MPI_FLOAT, id + 1, 0, mpc);
            // all even except 0 send first to id-1
            if (id > 0)     MPI_Send(&a[1], 1, MPI_FLOAT, id - 1, 0, mpc);
            
            // all even recv last from id+1
            if (id < p - 1) MPI_Recv(&a[k+1], 1, MPI_FLOAT, id + 1, 0, mpc, &status);
            // all even except 0 recv first from id-1
            if (id > 0)     MPI_Recv(&a[0], 1, MPI_FLOAT, id - 1, 0, mpc, &status);
        } 
        else {
            // all odd recv first from id-1
            if (id > 0) MPI_Recv(&a[0], 1, MPI_FLOAT, id - 1, 0, mpc, &status);
            // all odd except p-1 recv last from id+1
            if (id < p - 1) MPI_Recv(&a[k+1], 1, MPI_FLOAT, id + 1, 0, mpc, &status);

            // all odd send first to id-1
            if (id > 0) MPI_Send(&a[1], 1, MPI_FLOAT, id - 1, 0, mpc);
            // all odd except p-1 send last to id+1
            if (id < p - 1) MPI_Send(&a[k], 1, MPI_FLOAT, id + 1, 0, mpc);         
        }

        MPI_Barrier(mpc);
        if (id == p/2 ){
            midv = a[0];
        }
        MPI_Bcast(&midv, 1, MPI_FLOAT, p/2, mpc);
    }while (midv <= f2);
    

    //print Final array a[]
    MPI_Barrier(mpc);
    MPI_Gather(a+1, k, MPI_FLOAT, arr+1, k, MPI_FLOAT, 0, mpc);
    if (id == 0 ){
        for(i = 1; i <= n; i++){
           printf("%5.1f ", arr[i]);
        if (i % 10 == 0) printf("\n");
        }
        printf("\nConverged after %d iterations.\n", iter);
    }
    
    // free memory and finalize mpi
    free(a);
    free(b);
    free(arr);

    MPI_Finalize();
    return 0;
}
