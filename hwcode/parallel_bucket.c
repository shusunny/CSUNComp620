 /*===================================================
 CSUN COMP 620 Fall 2020
 Program:Parallel bucket sort n integers in the
 :range of a thru b using p processors
 Author :Gh. Dastghaibyfard
 Date :Oct. 25, 2020
 To compile: mpicc HWK4.c
 To execute: mpiexec -n p a.out n a b anyinputfilename
 p is number of processors
n is number of inputs, and may not be divisile by p
a is start range
b is end range
===================================================*/

// split ar[] with n elements into p buckets
// range = (b-a+1)/ p, range in ith bucket : i*range thru (i+1)*range -1
void split(int ar[], int b[], int m, int range, int a){
int i, j, k;
// m is bucket size and includes index
// no. of ar[] elements = m-1
for(i= 0; i < m-1 ; i++){
// find bucket no.
k = (ar[i] - a)/ range;

//kth bucket starts from b[k*m]
k = k * m;

//increase no. of elements in mth bucket
//b[k] contains no. of elements in the mth bucket
b[k]++;

//Store ar[i] in mth bucket
j = k + b[k];
b[j] = ar[i];
}
} // end split

// print contents of p buckets
void printbuck(int b[], int n, int p){
int i, j, cnt, m;
// print contents of each bucket
for (i = 0; i < p; i++){
//find number of elements in ith bucket
cnt = b[i*n];

//find start point of ith bucket
m = i * n + 1;

printf("\nContent of bucket %d=%d:", i, cnt);
printarr(b+m, cnt);
}
printf("\n");
} // end printbuck

int main(int argc, char **argv){
// n: no.of input,
// p:no. of processors
// id: processors id
// a: start range
// b: end range

// All Lines will be executed by all processors.
// except those with if(id== ?)

inputrange = (b - a + 1);

//range: range of each bucket
range = inputrange / p;
if (range * p != inputrange) range++;

// k data per processor
k = n/p; // assume n is divisible by p

//allocate n spaces for input
arr = malloc(n * sizeof(int));

//allocate k spaces for input of each processor
myarr = malloc(k * sizeof(int));

m = k+1; //one space for indexes

//allocate p*m spaces for p buckets
buck1 = malloc(p * m * sizeof(int));//send bucket
buck2 = malloc(p * m * sizeof(int));//recv bucket

// initialize index of each bucket
for(i = 0; i < p; i++)
buck1[i*m] = 0;

if (id == 0){
code for master to read input data from
input file and storing in arr[].

// Master prints input
printf("\tInput data before sorting:\n\t");
printarr(arr, n);
}

// Masters scatter data
 MPI_Scatter(arr, k, MPI_INT, myarr, k, MPI_INT, 0, mpc);

 //splitt myarr[] into p buckets
 split(myarr, buck1, m, range, a);


 MPI_Alltoall(buck1,m,MPI_INT,buck2,m,MPI_INT,mpc);

 //Every processor concatenates the data
 //in all buckets and sorts them

 //get no. of elements in buck[0]
 cnt = buck2[0];
 for (i = 1; i < p; i++){
 //get no. of elements in buck[i]
 k = buck2[i*m];
 for (j = i*m + 1; j <= i*m + k; j++)
 buck2[++cnt] = buck2[j];
 }//end for

 // store final no. elements in each bucket
 // after concatination
 buck2[0] = cnt;

 //bubble sort each bucket with cnt elements
 bubblesort(buck2+1, cnt);

 //allocate p spaces for final count in each bucket
 counts = malloc(p * sizeof(int);

 //Master gathers number of elements (cnt) in each
 //bucket and stores them in counts[]
 MPI_Gather(&cnt,1,MPI_INT,counts,1,MPI_INT, 0, mpc);


 //allocate p spaces for displacement of each
 //bucket in the final sorted array
 disp = malloc(p * sizeof(int);

 // Master computes displacements of each bucket in the
 // final sorted array for using MPI_gatherv
 if (id == 0){
 disp[0] = 0;
 for (i = 1; i < p; i++)
 disp[i] = disp[i-1] + counts[i-1];
 }

 // master gathers cnt elements from each bucket
 // and stores them at disp[i] in final array
 MPI_Gatherv(buck2+1, cnt,MPI_INT, arr,counts,disp,MPI_INT, 0, mpc);

 // master prints sorted input
 if (id == 0)
 printf("\tInput data after sorting:\n\t");
 printarr(arr, n);
 } // end id == 0

 free(arr);
 free(buck1);
 free(buck2);
 free(myarr);
 free(counts);
 free(disp);

 MPI_Finalize();
 return 0;
 } // end main
