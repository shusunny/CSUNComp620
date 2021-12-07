/*===================================================
CSUN COMP 620 Fall 2020 Exam-2 Nov. 01, 2020

I Sun, Zhen pledge by honor that I will not talk, 
text, chat and search the web during this online 
exam with anybody or via any device (phone,internet, ….).

Write an efficient parallel c program using type-2
pipline with p processors to implement parallel
insertion sort as explained in chapter 5, Wilkinson
book, to sort n numbers in ascending order.

Save your exam in xxxxxE1.c, where xxxxx is at
most the first 5characters of your last name.

To compile : mpicc xxxxxE2.c
To execute : mpiexec -n p a.out n anyinputfilename
p is number of processors
n is number of inputs, and is divisile by p

1-Create a data file that has at least 50 integers and contains duplicate.
2-After compiling and testing your c program, upload this file in canvas Exam-1, before 7:00 p.m.
11/01/20.
3-NO points will be given to submission with compile error.
NOTE: In fairness to all, absolutely no file will
be accepted by email.
A file will not be graded if has no name at the
beginning and has compile/execution error.
===================================================*/
#include <stdio.h>
#include <malloc.h>
#include <mpi.h>
#include"stdlib.h"
#define mpc MPI_COMM_WORLD
#define tag 100

void printarr(int a[], int n){
   int i;
   for(i = 0; i < n; i++){
      printf("%3d ", a[i]); 
      if ((i+1) % 10 == 0) printf("\n\t");
   }
   printf("\n");
}

void Compare_and_Send (int myid, int step, int *smaller, int *gotten )
{   
   
   if(step==0)
      *smaller = *gotten;
   else
      if(*gotten > *smaller)
            MPI_Send(gotten,1,MPI_INT,myid+1,tag,mpc);

      else{
            MPI_Send(smaller,1,MPI_INT,myid+1,tag,mpc);
            *smaller = *gotten;
      }
}

void intSort(int arr[], int n) 
{ 
    int i, key, j; 
    for (i = 1; i < n; i++) { 
        key = arr[i]; 
        j = i - 1; 
  
        while (j >= 0 && arr[j] > key) { 
            arr[j + 1] = arr[j]; 
            j = j - 1; 
        } 
        arr[j + 1] = key; 
    } 
} 

void Collect_Sorted_Sequence ( int myid, int p, int smaller, int *sorted )
{
    MPI_Status status;
    int k;
    if(myid==0){
      sorted[0] = smaller;
      for(k=1; k<p; k++)
         MPI_Recv(&sorted[k],1,MPI_INT,k,tag,mpc,&status);
   }
   else
      MPI_Send(&smaller,1,MPI_INT,0,tag,mpc);
}

void P_intSort(int a[], int b[], int p, int id){
    int g, s, j;
    MPI_Status status;
    for (j = 0; j < p-id; j++){
        if(id==0)
        {
            g = a[j];
            //printf("Master gets %d.\n",a[c+j]);
            Compare_and_Send(id,j,&s,&g);
        }
        else
        {
            MPI_Recv(&g,1,MPI_INT,id-1,tag,mpc,&status);
            //printf("p%d receives %d.\n",id,g);
            Compare_and_Send(id,j,&s,&g);
        }
    }
    Collect_Sorted_Sequence(id,p,s,b);
}

int main(int argc, char **argv) { 
    int id, p, err;
    int i, j, n, k;
    int *a, *srt, *myarr;
    int g, s, c; 
    
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
    a = malloc(n*sizeof(int));
    srt = malloc(n*sizeof(int));
    k = n/p;
    
    // master read inputs
    if (id == 0) {
	    FILE *inf;
		char *fname;
        fname = argv[2];  // get n
		inf = fopen(fname,"r");	// open finput file	
        for (i = 0; i < n; i++){
            fscanf(inf,"%d", &a[i]);
        } // end outer for
        fclose(inf);
        printf("Original array: \n\t");
        printarr(a, n);
    }

    // sort array use P_intSort and store back to srt.
    for (i=0; i<k; i++){        
        P_intSort(a+i*p, srt+i*p, p, id);
    }
    intSort(srt, n);
    MPI_Barrier(mpc);
    
    if (id == 0) {
        printf("Sorted array: \n\t");
        printarr(srt, n);
    }

    // free memory and finalize mpi
    free(a);
    free(srt);

    MPI_Finalize();
    return 0;
}