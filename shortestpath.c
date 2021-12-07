/*===================================================
          CSUN COMP 620 Fall 2020	
 Sequential Program Moore's shortest path problem.
 Graph is represented by adjacency Matrix.
 Matrix is mapped to a one dimensional array.

 Author    : Gh. Dastghaibyfard
 Date	   : Nov. 24, 2020
 To compile: gcc shortestpath.c  
 To execute: a.out n anydatafile
 n: No. of graph nodes 
===================================================*/
#include <stdio.h>     
#include <malloc.h>   
#include <stdlib.h>   
    //Implementing queue with array of size n
    //addq:if (rear+1 == front) then Queue is full.
	int addQ(int qu[], int n, int x,
             int *front, int *rear, int *cnt){
	  if((*rear+1)% n == *front){
		  //printf("qu is full.");
		  return 0;//front = rear = 0;
	  }		  
	  *rear = (*rear + 1) % n; //incremerear
	  qu[*rear] = x;
	  (*cnt) ++;
	  return 1;
	}  

    //delq:if (rear == front) then Queue is empty.
	int delQ(int qu[], int n, int *front, 
	         int *rear, int *cnt){
	  //printf(" DelQ ");
	  if(*front == *rear){
		  //printf(" qu is empty.");
		  return 0;//front = rear = 0;
	  }		  
	  *front = (*front + 1) % n; //incremerear
	  (*cnt) --;
	  return qu[*front];
	}  

void printarr(int a[], int n, char *msg){
    int i, j, m;
	printf("\n\t%s\n\t    ", msg); 
    for(i = 0; i < n; i++)
		printf("%3d ", i);
	printf("\n\t");
    for(i = 0; i < n; i++){
	  printf("%3d ", i);
      for(j = 0; j < n; j++){
		m = i*n+j;
		if (a[m] == 999)
		  printf(" ** "); 
	    else
		  printf("%3d ", a[m]);
	  }	  
	  printf("\n\t");
	} // end for
 } // end printarr

  void shortest(int a[], int n){
	int i, j, x, rear = 0, front = 0;
	int found, cnt = 0, newdist, m, k;
	//dist[] array to keep distance from node 0
	//to every other node
	int *dist = malloc(n * sizeof(int));

	//qu[] array to implement queue
	int *qu   = malloc(n * sizeof(int));
	
	//initialize dist[]
	for (i = 0; i < n; i++) 
		dist[i] = 999;// initialize distance
	dist[0] = 0;
	
	// add source-vertex (0) to Qu
	addQ(qu, n, 0, &front, &rear, &cnt);
	printf("task count init: %d\n", cnt);
	//cnt keeps no. of elements in the qu[]
	while (cnt > 0){//Qu is not empty 
	  //retrieve frontQ
	  x = qu[(front+1) % n];
	  printf("task num: %d\n", x);
	  // delete front element
	  delQ(qu, n, &front, &rear, &cnt);
	  printf("task count after del: %d\n", cnt);
	  //if there is an edge from x to every other
	  //nodes update dist[]
	  for (j = 1; j < n; j++) {
		//mapping a[x][j] to a[m]
		m = x * n + j; 
	    if(a[m] == 999)//no edge from x to j
		  continue; //do next j
		  
	    // an edge from x to j exist
    	newdist = dist[x] + a[m];

		//check if newdist<dist[j]
		if(newdist < dist[j]){
		  dist[j] = newdist;
		  //check if j is in the Qu or not
		  found = 0;
		  for (k = 1; k <= cnt; k++){
			m = (front+k) % n;
			if(qu[m]== j){
			  found = 1;
			  break; //exit for loop
			}
		  }//end check

		  //add j to Qu if not there
		  if (found == 0)
		  	printf("get new edge %d from \n", j);
 	 	    addQ(qu, n, j, &front, &rear, &cnt); 
			printf("task count after update j: %d\n", cnt);
 	    } // end outer if
      }// end for 
	} // end while
	printf("\n\tShortest path from node 0"
	" to nodes 1 thru %d:\n Nodes: ", n-1);
	for (k = 1; k < n; k++)
		printf("%3d ", k);
	printf("\n\t");
	for (k = 1; k < n; k++)
		printf("%3d ", dist[k]);
	printf("\n");

	//free allocated spaces in shortest
    free (dist);
    free (qu);
  }

	//main program
    int main(int argc, char *argv[]){
      //--Declare variables
      int *g, i, j, k, m, n, edges, wt;
	  FILE *inf;
	  char *fname;
	  // get no. of graph nodes
	  n = atoi(argv[1]); 
	  // get input file name
	  fname = argv[2]; 

	  //open input file
	  inf = fopen(fname,"r");	
	  
	  //read No. of edges
      fscanf(inf, "%d", &edges);
	  
      printf("\n\tNo.of nodes = %d, No.of edges = %d", n, edges);
	  
	  //Allocate space for directed graph g
      g =malloc(n * n * sizeof(int));
	  
	  //initialize directed graph g
      for (i = 0; i < n * n; i ++) 
		  g[i] = 999;
	  
	  //read edges information
      for (k = 1; k <= edges; k++) {
		//read weight of g[i][j]
		fscanf(inf, "%d%d%d", &i, &j, &wt);
		//save weight
		m =  i * n + j; 
        g[m] = wt;
      }
	  // close finput file
	  fclose(inf);
	  
	  //Display graph using adjacency matrix
	  printarr(g, n, "Initial Input");

      shortest(g, n);
	  
	  //free allocated spaces in main
      free (g);
     return 1;
    } // end main
