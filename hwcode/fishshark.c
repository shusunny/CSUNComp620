#include <stdlib.h>
#include <stdio.h>
#include <time.h>   
#define brdr '-'
#define star '*'
#define fish 'F'
#define shark 'S'

// define globe array adjacent and index;
int adja[8], aindex;

// print 2d space game of life
void printboard(char a[], int n, char *msg){
    int i, n2 = n+2, m = n2 * n2;
	printf("\n\t%s\n\t", msg); 
    for(i = 0; i < m; i++){
	    if (i % n2 == 0) printf("\n\t");
		printf("%c", a[i]); 
	} // end for
    printf("\n");
 } // end printboard

// print initial space game of life
void initboard(char a[], int n){
    int i, j, n2 = n+2, m = n2 * n2;
	//make star
	//int first row
    for(i = 0; i < n2 ; i++) 
		a[i]= brdr;

	//init last row
    for(i = n2*(n2-1); i < m  ; i++) 
		a[i]= brdr;	
	
	//init n by n matrix with star
    for(i = n2; i < m-n2  ; i++) 
		a[i]= star;
	
	//init first and last column
    for(m = 1, i = n2, j = n2+n2-1; m <= n ;
               m++, i+= n2, j+= n2){
		a[i]= brdr;	a[j]= brdr;
	}
 } // end initboard

//Randomly generate cnts sharks and cntf fish
void generate(char a[], int n, int cnts, int cntf ){
    int i, j, ns = 0, nf = 0, m, n2 = n+2;
	// Initilize Random Seed
    srand(time(NULL)); 
    while (ns < cnts){
	//Randomly generate i and j and 
	// store a shark in a[i][j]
		i = rand() % n +1;
		j = rand() % n +1;
		m = i * n2 + j;
        if (a[m] == star){ 
			a[m] = shark;
		    ns ++; //no. sharks generated
		}
	} // end while(ns < cnts)
    while (nf < cntf){
	//Randomly generate i and j and 
	// store a fish in a[i][j]
		i = rand() % n +1;
		j = rand() % n +1;
		m = i * n + j;
        if (a[m] == star){ 
			a[m] = fish;
		    nf ++; //no. fish generated
		}
	} // end while(nf < cntf)
} // end generate

// to find if adjacent cell empty and return count;
int count_empty(char a[], int n2, int c, int adja[], int count){
    int i;
    for (i=0;i<3;i++){
        if ((c-n2-1+i) == star) 
            adja[count] = c-n2-1+i;
            count++;
    }
    if ((c-1) == star) 
        adja[count] = c-1;
        count++;
    
    if ((c+1) == star) 
        adja[count] = c+1;
        count++;
    
    for (i=0;i<3;i++){
        if ((c+n2-1+i) == star) 
            adja[count] = c+n2-1+i;
            count++;
    }
    return count;
}

// to find if adjacent cells have fish and return count;
int fishcount(char a[], int n2, int c, int adja[], int count){
    int i;
    for (i=0;i<3;i++){
        if ((c-n2-1+i) == fish) 
            adja[count] = c-n2-1+i;
            count++;
    }
    if ((c-1) == fish) 
        adja[count] = c-1;
        count++;
    
    if ((c+1) == fish) 
        adja[count] = c+1;
        count++;
    
    for (i=0;i<3;i++){
        if ((c+n2-1+i) == fish) 
            adja[count] = c+n2-1+i;
            count++;
    }
    return count;
}
// make a move or pray
void move(char a[], int c, int dest, int type){
    a[dest] = type;
    a[c] = star;
}

// make a birth
void birth(char a[], int dest, int type){
    a[dest] = type;
}

void play(char a[], int n, int bs, int bf, int iteration){
    int i, j, m;
    int n2 = n+2;
    int count, dest;

    for (i=1; i<n+1; i++){
        for (j=1; j<n+1; j++){
            // get current position
            m = i*n2 + j;
            aindex = 0;
            // if current position is shark
            if (a[m] == shark){
                count = fishcount(a, n2, m, adja, aindex);
                if (count != 0) {
                    dest = rand() % count;
                    dest = a[adja[dest]];
                    // if time to birth
                    if (iteration%bs == 0)  birth(a, dest, shark);
                    // else just move
                    else {
                        move(a, m, dest, shark);
                    }
                }
                // count if current position has empty to move
                else {
                    count = count_empty(a, n2, m, adja, aindex);
                    if (count != 0) {
                        dest = rand() % count;
                        dest = a[adja[dest]];
                        // if time to birth
                        if (iteration%bs == 0)  birth(a, dest, shark);
                        // else just move
                        else {
                            move(a, m, dest, shark);
                        }
                    }
                }
            }
            // else if current position is fish
            else if(a[m] == fish){
                count = count_empty(a, n2, m, adja, aindex);
                if (count != 0) {
                    dest = rand() % count;
                    dest = a[adja[dest]];
                    // if time to birth
                    if (iteration%bs == 0)  birth(a, dest, fish);
                    // else just move
                    else {
                        move(a, m, dest, fish);
                    }
                }
            }
        }
    }
}


int main(int argc, char* argv[]) {
    int i,j, k, n, n2, s, f, bs, bf, g;
	int nfish, nshark;
	char *arr;
    n = atoi(argv[1]); // get n
	n2 = n+2;
	// get percentage of sharks
    s = atoi(argv[2]); 
	// get percentage of fish
    f = atoi(argv[3]);
    bs = atoi(argv[4]);
	bf = atoi(argv[5]);
	g = atoi(argv[6]);

	// Compute no. sharks and fish
	nshark = n * n * s / 100;
	nfish = n * n * f / 100;
	printf("\n\tnshark = %d, nfish = %d", nshark, nfish);

//Embed n by n matrix a into n+2 by n+2 matrix, 
//so every a[i][j] has exactly 8 neighbors
	//Matrix a is mapped to a one dimensional array
    arr = malloc(n2 * n2 * sizeof(char));

	// initilize matrix
    initboard(arr, n);
	
	// Generatte initil matrix
	generate (arr, n, nshark, nfish);
	
	//Prin initil matrix
	printboard (arr, n, "Initial input");

    // play one step
    play(arr, n, bs, bf, 1);
    printboard (arr, n, "first move");

    printf("\n\tby Zhen Sun for Comp620 homework-7\n");

	free(arr);
	return 0;
} // end main