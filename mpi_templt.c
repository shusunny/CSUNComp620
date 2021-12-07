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
            "TO RUN mpirun -n p a.out n a b inputfilename\n\t"
            "p is number of processors\n\t"
            "n is number of inputs\n\t"
            "a b is the floor and ceil of the array\n");
            // end if id == 0
        MPI_Finalize();  // terminate
        return 0;
    } // end if

    // free memory and finalize mpi


   MPI_Finalize();
   return 0;
}