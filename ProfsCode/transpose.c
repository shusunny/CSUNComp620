/*===================================================
 Name        : trnspose.c  
               transposing Matrix m by n
 Author      : Gh. Dastghaibyfard
 Version     :
 Date		 : Sep. 23, 2020
 To execute  : a.out row col
===================================================*/
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<malloc.h>

// print Matrix m by n
void printMatrix(int *arr, int m, int n, char *msg){
    int i, j;
    printf("\n%s is as follows:\n", msg);
	for (i = 0; i < m; i++){
	  for (j = 0; j < n; j++)
	    printf("%2d ", arr[i*n+j]);
	  printf("\n");
	}// end outer for
	printf("\n");
} // end printMatrix

// Genarate Matrix m by n
void genMatrix(int *a, int m, int n){
	int i, j, k=1;
	for (i = 0; i < m; i++)
	  for (j = 0; j < n; j++)
	    a[i*n+j] = k++;
} // end genMatrix

// transpoing the matrix a into b
void transpose(int *a, int m, int n, int *b){
	int i, j;
	for (i = 0; i < n; i++)
	  for (j = 0; j < m; j++)
        b[i*m+j] = a[j*n+i];
} // end GenMatrix

int main(int argc, char * argv[]) {
  int row, col, range, n, *mat, *tr;
  row = atoi(argv[1]);  // get matrix row
  col = atoi(argv[2]);  // get matrix column
  n = row * col;

  //--Allocate Space for Matrix mat 
  mat = malloc (n * sizeof (int));
  //--Allocate Space for rows of Matrix transpose
  tr  = malloc (n * sizeof (int));

  //--Generate and print Matrix a
  genMatrix(mat, row, col); 

  //printing the original matrix
  printMatrix(mat, row, col, " Initial Matrix");

  //transpoing the matrix  
  transpose(mat, row, col, tr);

  //printing the transpoed matrix
  printMatrix(tr, col, row, " TransPose");

  free(mat);
  free(tr);
  return 0;
}