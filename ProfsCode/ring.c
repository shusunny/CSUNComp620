#include <stdio.h>
#include <mpi.h>
#define mpc MPI_COMM_WORLD
	 //-- Simulating ring network
    //--	Master generates a token and passess it to first processor
    //-- every processors adds 2 to the token and passes it to its right neighbor
    //--	and finally master receives token from last processor  Sep. 16, 20
    //--	TO COMPILE	mpicc ring.c
    //--	TO RUN		mpirun -n p a.out 
    //--	p (integer) No.of Processors
int main (int argc, char *argv[]) {
  int token, p, id, err;
  MPI_Status status;
  // initialize MPI_Init  
  err = MPI_Init(&argc, &argv); 
  if (err != MPI_SUCCESS){
		  printf("\nError initializing MPI.\n");
		  MPI_Abort(mpc, err);
	}
  MPI_Comm_size(mpc, &p); // get no. of processes
  MPI_Comm_rank(mpc, &id); // get processor id
  if (id == 0) {// to print only once....
	if (argc < 1) {
	  printf("\n\tOOOPS...., INVALID No of Arguements,\n"
	  "\tSimulating ring network\n"
	  "\tMaster generates a token and passess it to first processor\n"
	  "\tevery processors adds 2 to the token and passes it to its right neighbor\n"
	  "\tand finally master receives token from last processor.  Sep. 16, 20"
	  "\tTO COMPILE	mpicc fname.c\n"
	  "\tTO RUN	mpirun -n pp a.out \n");

	} // end if
  } // end id == 0
  // To stop all processes if argc < 1
  if (argc < 1) {
	  MPI_Finalize(); 
	  return 0;
  } // end if

  if (id != 0) {
   // Every process except master (id=0) receives
   // token from its left neighbor 
   MPI_Recv(&token, 1, MPI_INT, id - 1, 0, mpc, &status); // p-1 Recv’s
   printf("Process %d received token %d from process %d\n", id, token, id - 1);
  } else // Master sets initial value before sending.
       token = -1;
  token += 2;
  MPI_Send(&token, 1, MPI_INT, (id + 1) % p, 0, mpc); // p Send’s
  if (id == 0) {
    // Now master receives from its left neighbor
    //(the last process)
    MPI_Recv(&token, 1, MPI_INT, p - 1, 0,  mpc, &status); // 1 Recv
    printf("Process %d received token %d from process %d\n", id, token,
    p - 1); 
  }  // end if
  // NOTE: no. of send’s & Recv’s must be equal
  MPI_Finalize(); 
}  // end main