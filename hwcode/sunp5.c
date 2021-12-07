#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

int main(int argc, char **argv){

    int i, j, n;
    float *a, *b, *x;
    float sum;

    //Implement matrix as a one dimensional array a[].
    n = atoi(argv[1]); // get n

    x = malloc(n*sizeof(float));
    b = malloc(n*sizeof(float));
    a = malloc(n*n*sizeof(float));

    FILE *inf;
    char *fname;
    // get file name
    fname = argv[2];
    // open finput file
    inf = fopen(fname,"r");
    // read input
    for (i = 0; i < n; i++){
        for (j = 0; j < n-i; j++){
            fscanf(inf,"%f", &a[(n-i-1)*n+j]); //reading a[i][j]
            //printf("a[%d][%d] = %f\t", (n-i-1), j, a[(n-i-1)*n+j]);
        // end for
        }
        fscanf(inf,"%f", &b[n-i-1]);
        //printf("b[%d] = %f\n\n", (n-i-1), b[(n-i-1)]);
    } // end outer for
    fclose(inf);

    printf("\nthe computed x is: \n");
    x[0] = b[0]/a[0];
    for (i = 1; i < n; i++){
        sum = 0;
        for (j = 0; j < i; j++)
            sum = sum + a[i*n+j]*x[j];
        if(a[i*n+i] == 0)
            x[i] = 0;
        else
            x[i] = (b[i] - sum) / a[i*n+i];
    }

for (i = 0; i < n; i++){
    printf("x[%d] = %f\n", i, x[i]);
} // end outer for

printf("\n");

return 0;
}