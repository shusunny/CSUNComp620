/* 
To compile:
1. To compile suzaku.o with suzaku.c suzaku.h
mpicc -c -o suzaku.o suzaku.c -lm

2. MontePi_workpool: MontePi_workpool.c suzaku.h suzaku.o
mpicc sz_shortest.c suzaku.o -lm

*/

// Suzaku Workpool version 3
#include <stdio.h>
#include <string.h>
#include "suzaku.h"

// shortest path data
#define N 6		// number of nodes
int w[N][N];  		// Adjacency matrix for w. Each process will have a copy of this without needing to broadcast it
int dist[N];		// Current minimum distances. Each prcess will have their own copy
int newdist_j;

void print_dist(char *msg) {
	int i;
	printf(msg);
	for (i = 0; i < N; i++) 
	   printf("%3d ", dist[i]);
	printf("\n");
	return;
} 
 

void print_tasks(int tasks[N]) {
	int i;
	printf("Tasks = ");
	for (i = 0; i < N; i++) 
	   if (tasks[N] > 0) printf("%3d ", tasks[i]);
	printf("\n");
	return;
} 
// workpool functions to be provided by programmer:

void init(int *T) {  // initialize w and dist (all processes) and insert initial tasks in task queue (master)

	int i,j;

	for (i = 0; i < N; i++) dist[i] = 99999; 	// initialize to greater than the max possible distance
	dist[0] = 0;				 	// distance from first node to itself = zero

	for (i = 0; i < N; i++) 
	for (j = 0; j < N; j++)
		w[i][j] = -1; 				// initialize to no connection
	w[0][1] = 10;	 				// set specific connections, matches values in book
	w[1][2] = 8;
	w[1][3] = 13;
	w[1][4] = 24;
	w[1][5] = 51;
	w[2][3] = 14;
	w[3][4] = 9;
	w[4][5] = 17;

	SZ_Master {
		SZ_Insert_task(0);				// insert first node 0 into queue, strictly only the master needs to do this
		printf("Init() inserting 0 into task queue\n");	// only the queue in the master if used
	}
	return; 
}

void diffuse(int taskID) {  // diffuse attaches the current distances

	SZ_Put("dist",dist);	// from global array dist[] in master
	
	print_dist("diffused distance: ");

	return;
}
	
void compute(int taskID) {

	int i,j;
	int new_tasks[N];  // max of N new tasks
	int slave; 

	SZ_Get("dist",dist);  // update array dist[] in slave

	slave = SZ_Get_process_num();

	for (i = 0; i < N; i++) new_tasks[i] = 0;

	//printf("Slave %d Task %d recvd with dist%3d %3d %3d %3d %3d %3d\n",slave,taskID,dist[0],dist[1],dist[2],dist[3],dist[4],dist[5]);

	i = 0;
	for (j = 0; j < N; j++) {  				// check each destination j from vertex taskno, sequential order
		if (w[taskID][j] != -1) {			// if destination j connected directly
		   newdist_j = dist[taskID] + w[taskID][j];  	// distance to j thro i using current shortest distance to i
		   if (newdist_j < dist[j]) {			// update shortest distance to j if shorter
			dist[j] = newdist_j;
			if (j < N-1) { // do not add last vertex (destination)
			   new_tasks[i] = j;
			   i++;
			   printf("Slave %d Task %d New shorter dist. to vertex %d found. Vertex added to result\n",slave,taskID,j);
			}
		   }
		}
	}

	//printf("Slave %d Task %d Tasks generated %2d,%2d,%2d,%2d,%2d,%2d, current dist. %3d %3d %3d %3d %3d %3d\n",slave,taskID,new_tasks[0],new_tasks[1],new_tasks[2],new_tasks[3],new_tasks[4],new_tasks[5],dist[0],dist[1],dist[2],dist[3],dist[4],dist[5]);

	SZ_Put("result",new_tasks);
	SZ_Put("dist",dist); 
	
	return;
}

void gather(int taskID) {

	int i;
	int dist_recv[N];
	int new_tasks[N];  // max of N new task

	SZ_Get("result",new_tasks);  	// this will only get the first added task
	SZ_Get("dist",dist_recv);

	//printf("Gather  Task %d Tasks received  %2d,%2d,%2d,%2d,%2d,%2d, dist. received %3d %3d %3d %3d %3d %3d\n",taskID,new_tasks[0],new_tasks[1],new_tasks[2],new_tasks[3],new_tasks[4],new_tasks[5],dist_recv[0],dist_recv[1],dist_recv[2],dist_recv[3],dist_recv[4],dist_recv[5]);		

	for (i = 0; i < N; i++) 
	if (dist_recv[i] < dist[i]) dist[i] = dist_recv[i];   // this will update dist in master. Possible received values on the smallest

	for (i = 0; i < N; i++) {
		if (new_tasks[i] != 0) {
			SZ_Insert_task(new_tasks[i]);
		}
	}

	print_dist("updated distance: ");

	return;
}

int main(int argc, char *argv[]) {
						// All variables declared here are in every process
	int i,j;
	int P;					// number of processes, set by SZ_Init(P) 			

	SZ_Init(P);				// initialize MPI message-passing environment
						// sets P to be number of processes	
	SZ_Parallel_begin

		SZ_Workpool3(init,diffuse,compute,gather);
	
	SZ_Parallel_end;			// end of parallel
	
	print_dist("\nFinal results: ");
	
	SZ_Finalize(); 

	return 0;
}
