/*===================================================
CSUN COMP 620 Fall 2020
Sequential Program for solving System of Linear
Equations (upper-triangular matrix form) with n unknowns.
Matrix is mapped to a one dimensional array

Author : Gh. Dastghaibyfard
Date : Oct. 30, 2020
To compile: gcc HWK5.c
 To execute: a.out n anyinputfilename
 n is number of unknowns and n is divisile by p
 ===================================================*/
 #include <stdlib.h>
 #include <stdio.h>

 void printarr(float a[], float b[],int n, char *msg){
 int i, j;
 printf("\n\t%s\n\t", msg);
 for(i = 0; i < n; i++){
 for(j = 0; j <= i; j++){
 printf("%6.2f ", a[i*n+j]);
 if ((i+1) % 10 == 0) printf("\n\t");
 } // end for
 printf("%6.2f\n\t", b[i]);
 } // end for
 } // end printarr

 void findx(float a[], float b[], float x[], int n){
 int i, j;
 float sum;
 x[0] = b[0]/a[0]; // x[0] computed separately
 //compute n-1 unknowns
 for (i = 1; i < n; i++){
 sum = 0;
 for (j = 0; j < i; j++)
 sum = sum + a[i*n+j]*x[j];
 //end for j = 0
 x[i] = (b[i] - sum)/a[i*n+i];
 }
 } // end findx


 int main(int argc, char* argv[]) {
 int n = atoi(argv[1]);

 //Matrix arr is mapped to a one dimensional array
 float *arr = malloc(n * n * sizeof(float));
 float *b = malloc(n * sizeof(float));
 float *x = malloc(n * sizeof(float));

 FILE *inf = fopen(argv[2], "r");
 int i, j, k;

 //store input as lower triangle
 for (i = 0; i < n; i++) {
 for (j = 0; j < n - i; j++) {
 k = n - i - 1; //save as lower triangle
 fscanf(inf, "%f", &arr[k*n+j]);
 }
 fscanf(inf, "%f", &b[k]);
 }
 fclose(inf);

 // print input
 printarr(arr, b, n, "Initial input Matrix");

 // compute unknowns
 findx(arr, b, x, n);

 // Display unknowns
 printf("Solved System of Equations:\n");
 for (int i = 0; i < n; i++) {
 printf("\tX%d=%5.2f", i, x[i]);
 if ((i + 1) % 4 == 0) printf("\n");
 }
 printf("\n");
 } // end main
