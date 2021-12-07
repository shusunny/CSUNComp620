/*===================================================
HWK-9: Due Date: before midnight 12/14, 200 Pts.
Write an efficient parallel program using p processors to implement shortest path
problem in a directed graph with n nodes, using dynamic load balancing (centralized).
Please name your file xxxxxp9.c, where xxxxx is at most the first 5 characters of your
last name and p9 is the program number. Please print initial input parameters and
distance array from node 0 to nodes 1 thru n-1;
Please PRINT your name at the end of OUTPUT.
Uploaded your program in canvas before the due date.
To execute: mpiexec -n p a.out n inputfile
Example: mpiexec -n 3 a.out 20 inputfile
Please see attached sequential program for shortest path problem.
===================================================*/
#include <stdio.h>     
#include <malloc.h>   
#include <stdlib.h>   
#include <mpi.h> 

#define mpc MPI_COMM_WORLD
#define task_req 1
#define result 2
#define termination_tag 100
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

void shortest(int a[], int n, int id){
    MPI_Status status;
    int i, j, x, rear = 0, front = 0;
    int found, newdist, m, k;
    int task =1;
    //dist[] array to keep distance from node 0
    //to every other node
    int *dist = malloc(n * sizeof(int));

    // master
    if (id == 0){
        //qu[] array to implement queue
        int *qu   = malloc(n * sizeof(int));
        int rear = 0, front = 0, cnt = 0;

        //initialize dist[]
        dist[0] = 0;
	    for (i = 1; i < n; i++) 
            dist[i] = 999;// initialize distance
        
        // add source-vertex (0) to Qu
        addQ(qu, n, 0, &front, &rear, &cnt);
        //cnt keeps no. of elements in the qu[]
        while (cnt > 0){//Qu is not empty 

            MPI_Recv(&task, 1, MPI_INT, MPI_ANY_SOURCE, task_req, mpc, &status);/* request task from slave */
            printf("task request received from p%d\n", status.MPI_SOURCE);
            //retrieve frontQ
            x = qu[(front+1) % n];
            // delete front element
            delQ(qu, n, &front, &rear, &cnt);

            MPI_Send(&x, 1, MPI_INT, status.MPI_SOURCE, task_req, mpc);/* send next vertex and */
            MPI_Send(dist, n, MPI_INT, status.MPI_SOURCE, task_req, mpc);/* current dist array */

            MPI_Recv(&j, 1, MPI_INT, MPI_ANY_SOURCE, result, mpc, &status); /* new distance */
            MPI_Recv(&dist[j], 1, MPI_INT, status.MPI_SOURCE, result, mpc, &status); /* new qu array */

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
            if (found == 0){
                addQ(qu, n, j, &front, &rear, &cnt); 
                printf("task count after update j: %d\n", cnt);
            } //end if
        } //end while

        MPI_Recv(&task, 1, MPI_INT, MPI_ANY_SOURCE, task_req, mpc, &status); /* request task from slave */
        MPI_Send(&x, 1, MPI_INT, status.MPI_SOURCE, termination_tag, mpc); /* termination message*/
    	
        // print result
        printf("\n\tShortest path from node 0"
        " to nodes 1 thru %d:\n Nodes: ", n-1);
        for (k = 1; k < n; k++)
            printf("%3d ", k);
        printf("\n\t");
        for (k = 1; k < n; k++)
            printf("%3d ", dist[k]);
        printf("\n");

        //free allocated spaces in shortest */
    }//end if master 
    
    //Slave (process i)
    else{
        MPI_Send(&task, 1, MPI_INT, 0, task_req, mpc); /* send request for task */
        MPI_Recv(&x, 1, MPI_INT, 0, MPI_ANY_TAG, mpc, &status); /* get vertex number */
        printf("task num%d received at p%d\n", x, id);
        if (status.MPI_TAG != termination_tag) {
            MPI_Recv(dist, n, MPI_INT, 0, task_req, mpc, &status); /* get dist array */
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
                    MPI_Send(&j, 1, MPI_INT, 0, result, mpc);/* send vertex j to master */
                    MPI_Send(&newdist, 1, MPI_INT, 0, result, mpc);/* send vertex j to master */
                }//end if
            }// end for
        } // end if not terminate
    } // end slave
}// end function


//main program
int main(int argc, char *argv[]){
    //--Declare variables
    int i, j, k, m, n, edges, wt;
	char *fname;
    int id, p, err;
    
    // initialize MPI_Init
    err = MPI_Init(&argc, &argv);
    if (err != MPI_SUCCESS){
		printf("\nError initializing MPI.\n");
		MPI_Abort(mpc, err);
    } // end if
	// Get No. of processors
    MPI_Comm_size(mpc, &p);
    // Get processor id
    MPI_Comm_rank(mpc, &id);
      
    // get no. of graph nodes
	n = atoi(argv[1]); 
    //Allocate space for directed graph g
    int *g =malloc(n * n * sizeof(int));
	// get input file name
	if (id == 0) {
      FILE *inf;
      char *fname;
      fname = argv[2];  // get n
      inf = fopen(fname,"r");	// open finput file	
      
      //read No. of edges
      fscanf(inf, "%d", &edges);
      printf("\n\tNo.of nodes = %d, No.of edges = %d", n, edges);

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
    }

      shortest(g, n, id);

      MPI_Finalize(); 
     return 1;
    } // end main

