#include <stdio.h> 		 
#include <malloc.h>
#include"stdlib.h"

void countSort(int arr[], int n, int a, int b)
{   
    int i, j, k;   
    int range = b-a+1;
    int *count = calloc(range, sizeof(int));  
    
    for(i=0; i < n; i++){    
        (count[arr[i]-a])++; 
    }

    /*for(i=0,j=0; i < n; i++){   
        for(; count[i]>0;(count[i])--) {       
            srt[j++] = i; 
        }  
    }  */ 

    k = 0;
    for (j = 0; j < range ; j ++)
        for (i = 0; i < count[j] ; i ++)
            arr[k++] = a+j;
}

void printarr(int a[], int n, char *msg){
    int i;
	printf("\n\t%s\n\t", msg); 
    for(i = 0; i < n; i++){
	  printf("%3d ", a[i]); 
	  if ((i+1) % 10 == 0) printf("\n\t");
    }
}

int main(int argc, char **argv) { 
    int i, n, j, *arr, *srt;   
    int a, b, p;
    
    n = atoi(argv[1]); // get n
    a = atoi(argv[2]); // get a
    b = atoi(argv[3]); // get b
    p = atoi(argv[4]); // get p
    
    // allocate space for input and sorted
    arr = malloc(n * sizeof(int));//input
    srt = malloc(n * sizeof(int));//sorted

    int range = b - a + 1; // get range
    int brange = range/p; // get bucket range

    // to get ceiling of range
    if (brange*p != range)
        brange++;
    
    // init bucket and bucket size
    int *bucket[p]; 
    int *bsize = calloc(p, sizeof(int));
    
    for(i = 0; i < p; i++) 
        bucket[i] = malloc(n*sizeof(int));         

    // Open input file 
	FILE *inf;
	char *fname;
    fname = argv[5];  // get n
	inf = fopen(fname,"r");	// open finput file	
	for(i = 0; i < n; i++)// read n numbers and print them formatted 10 per line
		fscanf(inf, "%d", &arr[i]);
	fclose(inf); 
	
    printarr(arr, n, "input");
    
    // Put array elements in different buckets 
    for (i = 0; i < n; i++){
          // get bucket no. k
        int k = (arr[i]-a) / brange;
        bucket[k][bsize[k]] = arr[i];
        bsize[k]++;
    }
   
    printarr(bucket[0], bsize[0], "first bucket");
    printarr(bucket[p-1], bsize[p-1], "last bucket");
    
    //  Sort individual buckets 
    for (i = 0; i < p; i++) {
        int floor = i * brange;
        int top = floor + brange;
        countSort(bucket[i], bsize[i], floor, top);
    }

    // Concatenate all buckets into arr[]
    int index = 0; 
    for (i = 0; i < p; i++) {
        for (int j = 0; j < bsize[i]; j++){ 
            srt[index] = bucket[i][j]; 
            index++;
        }
    }
	
    printarr(srt, n, "sorted");
	printf("\n");
    return 0;
} 