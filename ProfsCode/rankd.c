/*===================================================
 Name        : rankd.c  rank sort 
 Author      : Gh. Dastghaibyfard
 Date		 : Sep. 25, 2020
 To execute  : a.out inputfilename
===================================================*/
#include <stdio.h> 		 
#include <malloc.h>
	//First create an input file
    //Sequential program to read n distinct
	//integers numbers from an input file and 
	//performing rank sort.
    //TO COMPILE gcc rankd.c
    //TO RUN	 a.out inputfilename
 void printarr(int a[], int n, char *msg){
    int i;
	printf("\n\t%s\n\t", msg); 
    for(i = 0; i < n; i++){
	  printf("%3d ", a[i]); 
	  if ((i+1) % 10 == 0) printf("\n\t");
	}
 } // end printarr

// Find rank of each distinct element of arr a
// and creates sorted array b
 int rankd(int a[], int b[], int n){
    int i, j, tmp, *rnk, cnt, s;
	//allocate space to compute rank of each
	//element of arr a
    rnk = calloc(n, sizeof(int));
    for(i = 0; i < n; i++){
      for(j = 0; j < i; j++)
		if (a[i] >= a[j]) rnk[i]++;
		else rnk[j]++;
	}
	//Based on the rank of each element
	//create array b
    for(i = 0; i < n; i++){
		tmp = rnk[i];
		b[tmp] = a[i];
	}
  } // end rankdistinct

 int main(int argc, char **argv){
    int i, n, *arr, *srt;
	if (argc < 2) {
	 printf("\n\tOOOPS...., INVALID No of Arguements,\n\tSequential program to read n"
	 "distinct integers\n\tnumbers from an input file and performs rank sort.\n\t"
	 "Make sure to create an input file.\n\t"
	 "TO COMPILE: gcc rank.c\n\tTO RUN: a.out inputfilename");
		exit(1);
	} // end if

    // Open input file 
	FILE *inf;
	char *fname;
    fname = argv[1];  // get n
	inf = fopen(fname,"r");	// open finput file	
	fscanf(inf, "%d", &n); // read n
	printf(" n = %4d", n);
	// allocate space for input and sorted
    arr = malloc(n * sizeof(int));//input
    srt = malloc(n * sizeof(int));//sorted
	
	for(i = 0; i < n; i++)// read n numbers 
	fscanf(inf, "%d", &arr[i]);
	fclose(inf); // close finput file	
	
	printarr(arr, n, "input");
    rankd(arr, srt, n);
	printarr(srt, n, "sorted");
	printf("\n");
    return 0;
 } // end main