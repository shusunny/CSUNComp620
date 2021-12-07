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
    // init bucket and bucket size
    int *bucket[p+1]; 
    int *bsize = calloc(p+1, sizeof(int));
    
    for(i = 0; i <= p; i++) 
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
    
    // Put array elements into different buckets 
    for (int i = 0; i <= p; i++){
        int index = 0;
        int floor = i * brange;
        int top = floor + brange;
        for (j = 0; j < n; j++){
            if ((arr[j] >= floor) && (arr[j] < top)){
                bucket[i][index] = arr[j];
                index++; 
                bsize[i]++;
            }
        }
    }

    //  Sort individual buckets 
    for (i = 0; i <= p; i++) {
        int floor = i * brange;
        int top = floor + brange;
        countSort(bucket[i], bsize[i], floor, top);
    }

    // Concatenate all buckets into srt[]
    int index = 0; 
    for (i = 0; i <= p; i++) {
        for (int j = 0; j < bsize[i]; j++){ 
            srt[index] = bucket[i][j]; 
            index++;
        }
    }
	
    printarr(srt, n, "sorted");
	printf("\n");
    return 0;
} 