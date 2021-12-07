/*
n: two dimensioal space size(n by n), s: initial sharks percentage, f: initial fish percentage, bs: sharks breeding age, 
bf: fish breeding age, g: generation periods. Initially sharks and fish are placed randomly; 
each cell has 8 neighbors and s+f ≤ 40 percent of n×n, i.e. for n = 50, initial number of sharks and fish ≤ 1000.

HWK-7: Due Date: before midnight 11/14, 100 Pts.
Given a two dimensional space of n × n, (n ≤ 50), write an efficient sequential c program to simulate g generations game 
of life for shark and fish using above rules. Please name your file xxxxxp7.c, where xxxxx is at most the first 5 characters 
of your last name and p7 is the program number. Please print input parameters as well population of sharks and fish after 
every 10 generations in a user-friendly form.

Please PRINT your name at the end of OUTPUT.

To execute: ./a.out   n   s   f     bs     bf     g
Example: ./a.out    50    20    15    5    3   50

Board will be displayed as charactors, with fish as '.', shark as 'X', empty as ' ';
*/

#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <malloc.h>

void initial (char board[], int s, int f, int n) {
	int	i, j, k;
	int gene_s, gene_f;
	int num;
	// init to clear board
	for (i=0; i<n; i++) for (j=0; j<n; j++) 
		board[i*n+j] = ' ';

	gene_s = s*n*n/100;
	gene_f = f*n*n/100;

	// init sharks
	for (k=0; k<gene_s; k++){
		num = (rand() % (n*n + 1)); 
        board[num] = 'X';
	}
	// init fish
	for (k=0; k<gene_f; k++){
		num = (rand() % (n*n + 1)); 
        board[num] = '.';
	}
}

void printboard (char board[], int n) {
	int	i, j;

	/* for each row */
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			printf("%c", board[i*n+j]);
		}
		printf("\n");
	}
}

int main(int argc, char **argv){

    int i, j, n;
    int init_s, init_f;
	int breed_s, breed_f;
	int g;

	// get parameters
    n = atoi(argv[1]); 
	init_s = atoi(argv[2]); 
	init_f = atoi(argv[3]);
	breed_s = atoi(argv[4]);
	breed_f = atoi(argv[5]);
	g = atoi(argv[6]);

	// warning if init sharks and fish over 40 percent
	if (init_s+init_f>40){
		printf("Too many fishes and sharks, please try other s and f less than 40\n");
		return 0;
	}
	
	// init display board
	char *board = malloc(n*n*sizeof(char));
	
	initial(board, init_s, init_f, n);

	printboard(board, n);
	
	printf("by Zhen Sun for Comp620 homework-7\n");

	free(board);
	return 0;
}