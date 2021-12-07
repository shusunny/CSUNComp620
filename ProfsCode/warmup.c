/*
2 Name : bubblesort.c
3 Author : Gh. Dastghaibyfard
4 Date : 8/26/20
5 Description : Program to generate n integer random numbers in the
6 range of a....b ( a < b) and sorting them using bubble sort,
7 printing array before and after sorting, k elements per line
8 example: a.out 10000000 100 500 10
9 i.e. generating 10000000 random numbers from 100..500
10 and printing 10 numbers per line
11 */
 #include <stdio.h>
 #include <malloc.h>
 #include <stdlib.h>
 #include <time.h>

 // bubblesort
 void bubblesort(int arr[], int n){
 int i, j;
 for ( i = 1; i < n ; i++)
 for (j = 0; j < n - i; j ++)
 if (arr[j] > arr[j+1]) {
 int tmp = arr[j];
 arr[j] = arr[j+1];
 arr[j+1]= tmp;
 } // end if
 } // end bubble sort

 // Print array a K element per line
 void printArray(int *arr, int n, int k) {
 int i;
 for (i = 0; i < n; i++){
 printf("%7d ", arr[i]);
 if ((i+1) % k == 0) printf("\n");
 } // end for
 printf("\n");
 } // end printarr

/* pseudo code to generate random numbers
Let X & Y be two large prime integer similar to srand = time(t1)
range = b - a + 1;
for (i = 0; i < n; i++){
X = (X * Y) % range ;
arr[i] = X + a;
}// end for
*/
 // Genarate Array A with n elements in the range of a...b
 void genArray(int *arr, int n, int a, int b) {
 int i, range = b - a + 1;
 time_t t1;
 time(&t1); // get system time
 srand(t1); // Initilize Random Seed
 for (i = 0; i < n; i++)
 arr[i] = (rand() % range) + a;
 // end for
 } // end genArray

 int main(int argc, char *argv[]) { // for using command line

 //--Declare variables
 int *arr, a, b, n, k;
 printf("\n\t Bubble Sorting Program\n");

 // --Check no. of arguments
 if (argc < 5) {
 printf("\n\t OOOps, INVALID No of Arguements,\n"
 "\t Program to generate n random integers in the range of\n"
 "\t a....b ( a < b) and sorting them using\n"
 "\t bubble sort, printing array before and after sort,\n"
 "\t k elements per line, example: a.out 10000000 100 500 15\n"
 "\t i.e. generating 10000000 random numbers in the range of"
 "\t 100..500 and printing 15 numbers per line.\n");
 exit(1); // exit the program
 }

 //--Get input data from command line
 n = atoi(argv[1]); // get n
 a = atoi(argv[2]); // get lower range
 b = atoi(argv[3]); // get upper range
 k = atoi(argv[4]); // get k

 //--Allocate Space for Array a
 arr = malloc ((n) * sizeof (int));

 //--Generate and print Array a
 genArray(arr, n, a, b);
 printf("\n INPUT DATA before SORTING\n");
 //printArray(arr, n, k);

 //--Bubble sort Array a
time_t starttime, endtime;
time(&starttime); // get start time
 bubblesort(arr, n);
time(&endtime); // get end time
printf("\n starttime time = %ld", starttime);
printf("\n endtime time = %ld", endtime);
printf("\n Execution time = %ld", (endtime - starttime));
 printf("\n INPUT DATA after SORTING\n");
// printArray(arr, n, k);

 // --Return allocated space
 free (arr);
 return EXIT_SUCCESS;
 } // end main