/* L-16 MCS 572 Wednesday 15 February 2006 : pipeline to sort p numbers
 * This program uses a pipeline to sort p numbers using insertion sort.
 * Compile this program as "mpicc -o pipe_sort pipe_sort.c",
 * and then run on p processors by "mpirun -np p pipe_sort". */

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define v 1      /* verbose flag */
#define tag 100  /* tag for sending a number */

void Compare_and_Send ( int myid, int step, int *smaller, int *gotten );
/* processor "myid" initializes smaller with gotten at step zero,
 * or compares smaller to gotten and sends the larger number through */

void Collect_Sorted_Sequence ( int myid, int p, int smaller, int *sorted );
/* processor "myid" sends its smaller number to the manager who collects
 * the sorted numbers in the sorted array, which is then printed */

int main ( int argc, char *argv[] )
{
   int i,p,*n,j,g,s;
   MPI_Status status;

   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&p);
   MPI_Comm_rank(MPI_COMM_WORLD,&i);

   if(i==0) /* manager generates p random numbers */
   {
      n = (int*)calloc(p,sizeof(int));
      srand(time(NULL));
      for(j=0; j<p; j++) n[j] = rand() % 100;
      if(v>0) 
      {
         printf("The %d numbers to sort : ",p);
         for(j=0; j<p; j++) printf(" %d", n[j]);
         printf("\n"); fflush(stdout);
      }
   }

   for(j=0; j<p-i; j++)  /* processor i performs p-i steps */
      if(i==0)
      {
         g = n[j];
         if(v>0) { printf("Manager gets %d.\n",n[j]); fflush(stdout); }
         Compare_and_Send(i,j,&s,&g);
      }
      else
      {
         MPI_Recv(&g,1,MPI_INT,i-1,tag,MPI_COMM_WORLD,&status);
         if(v>0) { printf("Node %d receives %d.\n",i,g); fflush(stdout); }
         Compare_and_Send(i,j,&s,&g);
      }

   MPI_Barrier(MPI_COMM_WORLD); /* to synchronize for printing */
   Collect_Sorted_Sequence(i,p,s,n);

   MPI_Finalize();
   return 0;
}

void Compare_and_Send ( int myid, int step, int *smaller, int *gotten )
{
   if(step==0)
      *smaller = *gotten;
   else
      if(*gotten > *smaller)
      {
         MPI_Send(gotten,1,MPI_INT,myid+1,tag,MPI_COMM_WORLD);
         if(v>0)
         {
            printf("Node %d sends %d to %d.\n",myid,*gotten,myid+1);
            fflush(stdout);
         }
      }
      else
      {
         MPI_Send(smaller,1,MPI_INT,myid+1,tag,MPI_COMM_WORLD);
         if(v>0) 
         {
            printf("Node %d sends %d to %d.\n",myid,*smaller,myid+1);
            fflush(stdout);
         }
         *smaller = *gotten;
      }
}

void Collect_Sorted_Sequence ( int myid, int p, int smaller, int *sorted )
{
   MPI_Status status;
   int k;

   if(myid==0)
   {
      sorted[0] = smaller;
      for(k=1; k<p; k++)
         MPI_Recv(&sorted[k],1,MPI_INT,k,tag,MPI_COMM_WORLD,&status);
      printf("The sorted sequence : ");
      for(k=0; k<p; k++) printf(" %d",sorted[k]);
      printf("\n");
   }
   else
      MPI_Send(&smaller,1,MPI_INT,0,tag,MPI_COMM_WORLD);
}

/*
    for(i=0; i<k; i++){
        x = a[i*p];
        right_procNum = n - id -1; // number of processor to the right
        MPI_Recv(&x, 1, MPI_INT, id - 1, 0, mpc, &status); 
        printf("p%d get number x = %d\n",id, x);
        for (j = 0; j < right_procNum; j++){
            num = a[j];
            MPI_Recv(&num, 1, MPI_INT, id - 1, 0, mpc, &status); // receive num from p-1
            //printf("p%d get number num = %d\n",id, x);
            if (num > x){
                MPI_Send(&x, 1, MPI_INT, id + 1, 0, mpc); //send x to p+1
                x = num;
            }
            else
                MPI_Send(&num, 1, MPI_INT, id + 1, 0, mpc); // send num to p+1
        }
        MPI_Send(&x, 1, MPI_INT, id + 1, 0, mpc); // send number held
        for (j = 0; j < right_procNum; j++){
            num = a[j];
            MPI_Recv(&num, 1, MPI_INT, id + 1, 0, mpc, &status);
            MPI_Send(&num, 1, MPI_INT, id - 1, 0, mpc);
        }
    }
*/