/*===================================================
 Name        : omega.c  omega routing alogorithm
 Author      : Gh. Dastghaibyfard
 Version     :
 Date		 : Sep. 21, 2020
 To execute  : a.out p source destination
===================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc,char *argv[]) {
	if (argc < 4) {
	    printf("\n\tOOOps, INVALID No of Arguements,\n"
	    "\ta.out p source destination\n"
	    "\tOmega network routing program from src to dest\n");
	    exit(1);
	}
	int p   = atoi(argv[1]); //get No. of processors
	int src = atoi(argv[2]); //get source	
	int des = atoi(argv[3]); //get destination

	//CHECK if p = 2 ^ deg
	int deg = log2(p);
	int k   = pow(2,deg);
	if (p != k ||  src>=p || dst>=p){
	    printf("\n\tOOOPS, No of processors (%d) != 2 ^ %d OR\n"
	    "Source(%d) >= %d OR Dest(%d) >= %d\n", 
		p, deg, src, p, des, p);
	    exit(1);
	}
	printf("deg = %d, k = %d\n",deg, k);
	p = p >> 1;
	while (p > 0){
		int s = src, d = des;
		s = s & p;
		d = d & p;
		if(s == d) //if same bit then pass-through
			printf("pass-through-->");
		else
			printf("crossover-->");
		p = p >> 1;// check next bit
	}
	return EXIT_SUCCESS;
}