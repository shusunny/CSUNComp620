/*===================================================
CSUN COMP 620 Fall 2020 Exam-3 Nov. 29, 2020

I (LastName):  Sun    (FirstName): Zhen      pledge by
honor that I will not talk, text, chat and search
the web during this online exam with anybody or via
any device (phone,internet, ….).

The following sequential program solves one-dimensional (a[]) heat distribution problem with n elements based upon finite difference equation:
a[i]= .25*(a[i-1]+2*a[i]+ ai+1).*/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

// to execute: a.out n f1 f2 f3
int main(int argc, char *argv[]) {
int n, i, mid, iter=0 ;
float *a, *b, f1, f2, f3;
n = atoi(argv[1]);
f1 = atof(argv[2]);
f2 = atof(argv[3]);
f3 = atof(argv[4]);
//print input parameters
printf("n=%d, f1=%5.1f, f2=%5.1f, f3=%5.1f\n",
n, f1, f2, f3);
mid = (1+n)/2;//index of middle element of a[]
a = malloc((n+2) * sizeof(float));
b = malloc((n+2) * sizeof(float));

a[0]=f1; a[n+1] = f3;//fixed will not change

//initialize a[] i=1,..., n to 0.0
for (i = 1 ; i <= n ; i++ )
      a[i]= 0.0;
do{ //continue till converge

     iter++; //iterations
        for(i=1; i <= n; i++)
            b[i] = .25 *(a[i-1]+ 2 * a[i] + a[i+1]);

           for(i=1; i <= n; i++)
           a[i] = b[i];
}while (a[mid] <= f2 );//converge condition
//print Final array a[]
for(i = 1; i <= n; i++){
printf("%5.1f ", a[i]);
if (i % 10 == 0) printf("\n");
}
  printf("\nConverged after %d iterations.\n", iter);
return 1;
}
/*
(80 Pts)a-Convert it to an efficient parallel
algorithm using p processors.
Assume p is even and n (n>>p) is divisible by p.
(10 Pts)b-Compute tcomm and tcomp of your algorithm.
(10 Pts)c-What is the suitable network for your algorithm?

Save your exam in xxxxxE3.c, where xxxxx is at
most the first 5characters of your last name.

To compile : mpicc xxxxxE3.c
To execute : mpiexec -n p a.out n f1 f2 f3
p is number of processors
n is number of inputs, and
f1, f2, f3 are 3 float numbers
Example: mpiexec -n 20 a.out 1000 100.0 50.0 100.0
After compiling and testing your c program, upload this file in canvas Exam-3, before 7:00 p.m.
11/29/20.
NOTES:
NO points will be given to submission with compile error.
In fairness to all, absolutely no file will
be accepted by email.
A file will not be graded if has no name at the
beginning and has compile/execution error.
===================================================*/