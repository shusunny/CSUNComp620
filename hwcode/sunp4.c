 /*===================================================
HWK-4: Write an efficient parallel c program (150 Pts) to read n numbers in the range of a thru b 
and perform bucket sort using p processors as explained in Wilkinson Book, chapter 4. Please name 
your file xxxxxp4.c, where xxxxx is at most the first 5 characters of your last name and p4 is 
the program number.

To execute your program: mpiexec -n p a.out n a b anyinputfile
Where p is no. of processors. n may not be divisible by p!

===================================================*/
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <mpi.h> 

#define mpc MPI_COMM_WORLD


void countsort(int b[], int n, int a, int range, int buckno)
{   
    int i, j, k;   
    int *count;
    count = malloc(range * sizeof (int));
   for (j = 0; j < range ; j ++)
      count[j] = 0;
    
    // get frequecy for each num
    for(i=0; i < n; i++){   
       int tmp = b[i] - buckno *range;
       count[tmp]++; 
    }
   // put elements in sorted order.
    k = 0;
    for (j = 0; j < range ; j ++)
        for (i = 0; i < count[j] ; i ++)
            b[k++] = buckno*range + j;
   
   free(count);
}

// print arr function
void printarr(int a[], int n){
   int i;
   for(i = 0; i < n; i++){
      printf("%3d ", a[i]); 
      if ((i+1) % 10 == 0) printf("\n\t");
   }
}

// print numbers in each bucket
void printbuck(int b[], int ind[], int n, int p, int id){
int i, j, cnt, m;
// print contents of each bucket
   for (i = 0; i < p; i++){
      //find number of elements in ith bucket
      cnt = ind[i];
      //find start point of ith bucket
      m = i * n;
      printf("\nContent of bucket %d from Processor %d: ", i, id);
      printarr(b+m, cnt);
   }
   printf("\n");
}

// split numbers into buckets
void split(int arr[], int b[], int index[], int n, int range, int a){
   int i, j, m;
   for (i=0; i<n; i++){
      //find bucket #
      m = (arr[i]-a)/range;
      j = m*n + index[m];
      b[j] = arr[i];

      //increase index
      index[m]++;
   }

}

// sort contents of p buckets using bubble sort
void sortbuck(int b[], int ind[], int n, int range, int a, int p){
   int i, j, cnt, m;
   // count sort each bucket
   for (i = 0; i < p; i++){
      //find number of elements in ith bucket
      cnt = ind[i];
      //find start point of ith bucket
      m = i * n;
      //count sort ith bucket, a is start range
      countsort(b+m, cnt, range, a, i);
   }
}

int main(int argc, char **argv) { 
    int i, n, j, *arr, *srt;   
    int a, b, k;
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

    n = atoi(argv[1]); // get n
    a = atoi(argv[2]); // get a
    b = atoi(argv[3]); // get b
    k = n/p; // k number for each processor
   
    // allocate space for input and sorted
    arr = malloc(n * sizeof(int));//input
    srt = malloc(n * sizeof(int));//sorted
   
   // init array for future use
   int *myarr, *bucket, *index;
   int *new_bucket, *new_index;
   int *mysrt, *cnt, *disp;
   
   // compute bucket range
    int range = (b-a+1)/p; 
   if (range*p != b-a+1) 
      range++;

   // Master Reads Data from inputfile
    if (id == 0) {
	   FILE *inf;
		char *fname;
      fname = argv[4];  // get n
		inf = fopen(fname,"r");	// open finput file	
		for(i = 0; i < n; i++)// read n numbers and print them formatted 10 per line
			fscanf(inf, "%d", &arr[i]);
		fclose(inf); // close finput file
      printf("n = %d, range: %d thru %d, no. of processors&buckets = % d\n\t", n, a, b, p);
      printarr(arr, n);
    }

  
   // init arrays for buckets in each processor and future use.
   myarr = malloc(k*sizeof(int));
   bucket = malloc(n*sizeof(int));
   index = calloc(p, sizeof(int));
   new_bucket = malloc(n*sizeof(int));
   new_index = malloc(p* sizeof(int));
   mysrt = malloc(n*sizeof(int));
   int m = 0;
   int count = 0; 
   int start = a + id*range;
   cnt = malloc(p*sizeof(int));
   disp = malloc(p*sizeof(int));
      
   // scatter arrays to p processor. 
   MPI_Scatter(arr, k, MPI_INT, myarr, k, MPI_INT, 0, mpc);

   // split numbers in each process into bucket.
   split(myarr, bucket, index, k, range, a);

   // sorting each bucket of every processor
   sortbuck(bucket, index, k, range, a, p);
   printf("\nafter split in %d buckets: ", p); 
   printbuck(bucket, index, k, p, id);

   MPI_Barrier(mpc);
   // use all to all method
   MPI_Alltoall(bucket, k, MPI_INT, new_bucket, k, MPI_INT, mpc);
   MPI_Alltoall(index, 1, MPI_INT, new_index, 1, MPI_INT, mpc);
     
   printf("\nafter alltoall method:"); 
   printbuck(new_bucket, new_index, k, p, id);
   
   // store the new array in each processor and get their array size
   for (i = 0; i < p; i++) {
      for (j = 0; j < new_index[i]; j++){ 
         mysrt[m] = new_bucket[i*k+j]; 
         m++;
      }
      count += new_index[i];
   }
   // sort the big bucket
   countsort(mysrt, count, start, range, id);
   
   // gather count from each processor
   MPI_Gather(&count, 1, MPI_INT, cnt, 1, MPI_INT, 0, mpc);
  
   // make count array for final gether
   if (id == 0){
      disp[0] = 0;
      for (i = 0; i < p; i++){
         disp[i+1] = disp[i] + cnt[i];
      }
   }
   MPI_Gatherv(mysrt, count, MPI_INT, srt, cnt, disp, MPI_INT, 0, mpc);

   // master out put the sorted array
   MPI_Barrier(mpc);
   if (id == 0) {
      printf("\nfinal sorted array:\n\t");
      printarr(srt, n);
      printf("\n");
   }
   
   // free memory and finalize mpi
   free(arr);
   free(srt);
   free(bucket);
   free(index);
   free(new_bucket);
   free(new_index);
   free(myarr);
   free(mysrt);
   free(cnt);
   free(disp);

   MPI_Finalize();
   return 0;
} 