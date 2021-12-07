/*==================================================
 Name        : genLargint.c
 Author      : Gh. Dastghaiby Fard
 Description : generating n integer random numbers
  : range1....range2 and storing in text file
  To execute : a.out n
  To execute and store in file: a.out n > anyfilename
 ==================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
int main(int argc,char *argv[]) {
  if (argc < 2) {
    printf("\n\tOOOps, INVALID No of Arguements,\n"
       "\tThis program generates n integer random numbers 999999999 thru 100000000\n"
	   "\tTo execute: a.out  n\n\tTo execute and save in any file: a.out n > anyfilename\n");
    exit(1);
  }
	int i, n = atoi(argv[1]);
	uint64_t num;
	srand(time(NULL)); // Initilize Random Seed
	for(i = 1;i <= n;++i){
		num = rand();
		num = (num << 32) | rand();	

		// 100000000 < num <   999999999
		num = (num % 899999999) + 100000000;
		printf ("%9ld ", num);
		if (i % 8 == 0) printf ("\n");
	}
	return EXIT_SUCCESS;
}
