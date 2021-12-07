// Suzaku Routines with pipeline version 1 -- B. Wilkinson Dec 4, 2015

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>	// Needed for INT_MAX
#include <stdlib.h>  	// needed for malloc in workpool
#include "mpi.h" 

#define __sz_TERM 32767  // used in workpool, tags are non-negative
#define __sz_DUMMY 32766
#define BUFFER_START 4  // start of data in send buffer, 4 bytes for task number at location 0, 4 bytes for map ptr
#define MAPPTR 0	// position for map ptr

#define Pmax 20		// max number of processes. Used to statically declare connection connection_graph and slave busy array
#define SM_SIZE 100	// size of shared memory segment
#define QSIZE 50	// used in task queue, size of queue

#define NOTASK INT_MAX  // used in pipeline to indicate no task yet, tags are non-negative so could use unsigned int in signatures

int terminate = 0;	// curently only in pipeline for programmer to terminate loop
int debug = 0;

int rank;

// -------------used in new version of workpool with put and get
#define NAMES 10	 		// number of names of items, gives max size of look up table (map)
#define NAMESIZE 8			// number of chars in name
#define BUF_MAX_SIZE 10000		// max size of buffer

double task_buffer[BUF_MAX_SIZE];  	// max space for task

char map_name[NAMES][NAMESIZE]; 	// up to NAMES names, each name up to NAMESIZE chars
int map_position[NAMES];		// corresponding position for each name

int buf_position;   			// incremented by size of data during packing and gives size of message
int map_location;			// where map is located in message.  Stored at MAPPTR .
int n_p_index = 0;	      		// index into map_name/position, will give max size of map, computing from map calls

int size; 				// actual number of processes

					// Version of queue ops for workpool2_1
int task_queue[QSIZE];  		// task queue
int task_queue_front;			// task queue index for next task to add
int task_queue_rear;			// task queue index for next item to remove
int task_q_no_tasks; 			// number of items in queue
			
// map and unmap

void SZ_print_map() {  // for testing
	int i;
	printf("Map contents\n");
	for (i = 0; i < NAMES; i++) 
		printf("%s %d\n",map_name[i],map_position[i]);
	return;
}

void SZ_reset_map() {
	int i,j;
	for (i = 0; i < NAMES; i++) {
	  map_position[i] = 0;
	  for (j = 0; j < NAMESIZE; j++)
		map_name[i][j] = (char) 0;
	}
	n_p_index = 0;  	// where next entry placed in map
	buf_position = BUFFER_START; 	// reset buf_position in task_buffer, 4 bytes for map position, int
	return;
}
void SZ_Map_name(char key[NAMESIZE], int pos) {	
	if (n_p_index >= NAMES) printf("ERROR: Exceeded available space in name map\n");
	strcpy(map_name[n_p_index],key); 		// put name into SZ_name
	map_position[n_p_index] = pos; 			// put buf_position
	n_p_index++;					// next location in array
	return;
}

void SZ_Unmap_name(char key[NAMESIZE], int *pos) {
	int i;
	for (i = 0; i < NAMES; i++) {			// sequential search for entry
	   if (strcmp(map_name[i],key) == 0) {		// name found
	   	*pos = map_position[i];	        	// enter position
	   	break;					// finished
	    }
	}
	if (i == NAMES) printf("ERROR - Unmap cannot find name\n");
	return;
}
// SZ_pack_..() called by SZ_Put(key,x) after getting type and size of x
// SZ_unpack_..() called by SZ_Get(key,x) after getting type and size of x

void SZ_pack_double(char key[NAMESIZE], double *x, int count) { 
	if (buf_position >= BUF_MAX_SIZE) printf("ERROR: Exceeded message buffer space\n");
	SZ_Map_name(key,buf_position);				// uses current value of buf_position
  	MPI_Pack(x, count, MPI_DOUBLE, task_buffer, BUF_MAX_SIZE, &buf_position, MPI_COMM_WORLD);
	return;
}
void SZ_unpack_double(char key[NAMESIZE], double *x, int __sz_count) {
	int position;
	SZ_Unmap_name(key,&position);
  	MPI_Unpack(task_buffer, BUF_MAX_SIZE, &position, x, __sz_count, MPI_DOUBLE, MPI_COMM_WORLD); 	
	return;
}
void SZ_pack_int(char key[NAMESIZE], int *x, int count) {
	if (buf_position >= BUF_MAX_SIZE) printf("ERROR: Exceeded message buffer space\n");
	SZ_Map_name(key,buf_position);				// uses current value of buf_position
  	MPI_Pack(x, count, MPI_INT, task_buffer, BUF_MAX_SIZE, &buf_position, MPI_COMM_WORLD);
	return;
}
void SZ_unpack_int(char key[NAMESIZE], int *x, int __sz_count) {
	int position;
	SZ_Unmap_name(key,&position);
  	MPI_Unpack(task_buffer, BUF_MAX_SIZE, &position, x, __sz_count, MPI_INT, MPI_COMM_WORLD); 	
	return;
}
void SZ_pack_char(char key[NAMESIZE], char *x, int count) {
	if (buf_position >= BUF_MAX_SIZE) printf("ERROR: Exceeded message buffer space\n");
	SZ_Map_name(key,buf_position);			
  	MPI_Pack(x, count, MPI_CHAR, task_buffer, BUF_MAX_SIZE, &buf_position, MPI_COMM_WORLD);
	return;
}
void SZ_unpack_char(char key[NAMESIZE], char *x, int __sz_count) { 
	int position;
	SZ_Unmap_name(key,&position);
  	MPI_Unpack(task_buffer, BUF_MAX_SIZE, &position, x, __sz_count, MPI_CHAR, MPI_COMM_WORLD); 	
	return;
}
void SZ_pack_map() { // pack map called after all data packed
	int count;
	int location;			// local variable

	map_location = buf_position;  	// set global variable to where map is stored after all data

	count = NAMES * NAMESIZE; 	//currently send max size of arrays

	MPI_Pack(map_name, count, MPI_CHAR, task_buffer, BUF_MAX_SIZE, &buf_position, MPI_COMM_WORLD);
	count = NAMES;
	MPI_Pack(map_position, count, MPI_INT, task_buffer, BUF_MAX_SIZE, &buf_position, MPI_COMM_WORLD);

	location = MAPPTR;
	MPI_Pack(&map_location, 1, MPI_INT, task_buffer, BUF_MAX_SIZE, &location, MPI_COMM_WORLD); // store map position

	return;
}
void SZ_unpack_map() {
	int count;
	int location;

	SZ_reset_map(); // to reset index/clear arrays. Nore also alters buf_position
	buf_position = MAPPTR;  // reset buf_position to beginning

	MPI_Unpack(task_buffer, BUF_MAX_SIZE, &buf_position, &location, 1, MPI_INT, MPI_COMM_WORLD); // get position of map

	count = NAMES * NAMESIZE;
	MPI_Unpack(task_buffer, BUF_MAX_SIZE, &location, map_name, count, MPI_CHAR, MPI_COMM_WORLD);
	count = NAMES;

	MPI_Unpack(task_buffer, BUF_MAX_SIZE, &location, map_position, count, MPI_INT, MPI_COMM_WORLD);

	buf_position = BUFFER_START;  // reset buf_position

	return;
}	

int SZ_Insert_task(int taskID) {  // insert task into task queue
	int status;
	status = 0;
	if (task_q_no_tasks == QSIZE) {
		status = -1; 			// Queue full, no task added
    	} else {
		task_queue[task_queue_front] = taskID;	// Task added
		task_q_no_tasks = task_q_no_tasks + 1;
        	task_queue_front = (task_queue_front + 1) % QSIZE; 	// front points to next free space to insert
		status = task_q_no_tasks;		// returns number of tasks in queue
	}

	return status;
}

int SZ_Remove_task(int *taskID) {  // remove task from task queue
	int status;
	status = 0;
    	if (task_q_no_tasks == 0) {
        	status = -1;  			// Queue empty
    	} else {
    		*taskID = task_queue[task_queue_rear];		//Task removed
		task_q_no_tasks = task_q_no_tasks - 1;
		task_queue_rear = (task_queue_rear + 1) % QSIZE;	// rear points to next item to remove
		status = task_q_no_tasks; 		// returns number of tasks in queue
	}

	return status;
}

void SZ_task_q_print() {  // for testing
	int i;
	printf("Contents of queue\n");
	if (task_q_no_tasks == 0) printf("Queue empty\n");
	
	for(i = 0; i < task_q_no_tasks; i++) {
        	printf("%d ",task_queue[(task_queue_rear + i) % QSIZE] ); // print queue[(rear + i) % QSIZE]
    	}
	printf("\n");
	return;
}

void SZ_task_q_init() {  // initialize to zero
	int i;
	task_queue_front = 0;		// task queue index for next task to add
	task_queue_rear = 0;		// task queue index for next item to remove
	task_q_no_tasks = 0; 		// number of items in queue
	return;
}

// Routines for generalized patterns

int __sz_N;	// size of data sent between slaves
int buffer_size;
double *buffer;
int connection_graph[Pmax][Pmax];	// used in generalized pattern routines

void SZ_Set_conn_graph(int *G) {
	int i,j;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	for (i = 0; i < size; i++)
	for (j = 0; j < size; j++)
		connection_graph[i][j] = G[j + size * i];
	return;
}


void SZ_Print_conn_graph() {  // print out connection graph, for checking
	int i,j;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {		
		printf("Connection graph\n    ");
		for (j = 0; j < size; j++) printf("%2d ",j);
		for (i = 0; i < size; i++) {
			printf("\n%3d ",i);
			for (j = 0; j < size; j++)
				printf("%2d ",connection_graph[i][j]);
		}
		printf("\n");
		fflush(stdout);
	}
	MPI_Barrier(MPI_COMM_WORLD); // make sure print finished
	return;
}

void SZ_Pattern_init(const char* pattern, int N) {

	int i, j;

	__sz_N = N;

	buffer_size = __sz_N * sizeof(double) + MPI_BSEND_OVERHEAD; 	// each process just needs space for one message, done here for efficency
	buffer = (double*) malloc(buffer_size);	 

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size > Pmax) {printf("ERROR - Too many processes\n"); MPI_Abort(MPI_COMM_WORLD,999);}
	
	for (i = 0; i < size; i++)  			// initialize pattern table in all processes
		for (j = 0; j < size; j++)  
			connection_graph[i][j] = -1;  		// initialize to no connection

	// set up pattern. All processes do the same to have identical copies
	if (strcmp(pattern, "all-to-all") == 0) {   	// all-to-all pattern
		for (i = 0; i < size; i++)  		// source process
			for (j = 0; j < size; j++)  	// destination process
				if (i != j) connection_graph[i][j] = i;
	} else if ((strcmp(pattern, "pipeline") == 0) || (strcmp(pattern, "ring") == 0)){  // pipeline pattern
		for (i = 0; i < size-1; i++)  
			connection_graph[i][i+1] = 0;
		connection_graph[size-1][0] = 0; // ring, back to master
	} else if (strcmp(pattern, "stencil") == 0) {  // square stencil pattern
		int x,y,proc,sqrtP;
		sqrtP = (int) round(sqrt(size)); // must have an integer squareroot
		if (sqrtP * sqrtP != size) printf("ERROR number of processes does not have integer squareroot\n");
		for (y = 0; y < sqrtP; y++) // down -- natural ordering
		for (x = 0; x < sqrtP; x++) { // across
			proc = y * sqrtP + x;
			if (x - 1 >= 0)     connection_graph[proc][proc-1] = 1;  	// source to the right
			if (x + 1 < sqrtP)  connection_graph[proc][proc+1] = 0; 	// source to the left
			if (y - 1 >= 0)     connection_graph[proc][proc - sqrtP] = 3;  	// source below
			if (y + 1 < sqrtP)  connection_graph[proc][proc + sqrtP] = 2; 	// source above
		}
	} else printf("No pattern set up\n");

	MPI_Barrier(MPI_COMM_WORLD); // make sure all processes have updated gragh
	return;

} // end of SZ_Pattern

void SZ_Generalized_send(double *a, double *b) {  // all processes must execute this
	int i,j,temp; 			      // sends a[D] to b[P][D] for all connected processes
	int tag = 2;
	int rank;
	double *b_ptr;

	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (i = 0; i < size; i++) 
	for (j = 0; j < size; j++)  {
		temp = connection_graph[i][j];	
		if (temp >= 0) {
			b_ptr = b + (temp * __sz_N);  // points to b[temp][0]
			if (rank == i) {
				MPI_Bsend(a, __sz_N, MPI_DOUBLE, j, tag, MPI_COMM_WORLD); // using Bsend to avoid potential deadlock, buffer in SZ_Pattern_init
			}
			if (rank == j) {
				MPI_Recv(b_ptr, __sz_N, MPI_DOUBLE, i, tag, MPI_COMM_WORLD, &status);
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	
	return;

} // end of SZ_Send

void SZ_Pattern_finalize() {			// currently not used but really need a way to terminate nicely without asking programmer to do it
	MPI_Buffer_detach(buffer, &buffer_size); 
	return;
}

/* ---------------------------------------------------- */

// workpool version 1
void SZ_Workpool(void (*__sz_init)(int *T, int *D, int *R), void (*__sz_diffuse)(int taskID,double output[]),void (*__sz_compute)(int taskID, double input[], double output[]),void (*__sz_gather)(int taskID, double input[])  ) {

	int i; 
	int rank;     
	MPI_Status status; 

	int __sz_T, __sz_D, __sz_R; // # of tasks, # of data items in each task, # of data items in result of each task,

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	__sz_init(&__sz_T,&__sz_D,&__sz_R);

	double *task = (double*) malloc(__sz_D * sizeof(double));	
	double *result = (double*) malloc(__sz_R * sizeof(double)); 

	int taskID = 0;  // set to zero, programmer may alter this

	if (rank == 0) {   // Master
		int task_ptr = 0;	
		for (i = 1; i < size; i++) { 
			if (task_ptr < __sz_T) { 
				taskID = task_ptr;
				(*__sz_diffuse)(taskID,task);
				MPI_Send(task, __sz_D, MPI_DOUBLE, i, taskID, MPI_COMM_WORLD); 
				task_ptr++; 
			} else MPI_Send(task, 0, MPI_DOUBLE, i, __sz_TERM, MPI_COMM_WORLD); 
		} 
		for (i = 0; i < __sz_T; i++) { 
			MPI_Recv(result, __sz_R, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
			taskID = status.MPI_TAG; 
			(*__sz_gather)(taskID,result); 
			if (task_ptr < __sz_T) {
				taskID = task_ptr;  
				(*__sz_diffuse)(taskID,task); 
				MPI_Send(task, __sz_D, MPI_DOUBLE, status.MPI_SOURCE, taskID, MPI_COMM_WORLD); 
				task_ptr++; 
			} else  MPI_Send(task, 0, MPI_DOUBLE, status.MPI_SOURCE, __sz_TERM, MPI_COMM_WORLD); 
		} 
	} 
	else {			// Slave
		int terminate = 0; 
		do { 
			MPI_Recv(task, __sz_D, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
			taskID = status.MPI_TAG; 
			if (taskID == __sz_TERM) terminate = 1; 
			else { 
				(*__sz_compute)(taskID,task,result);	
				MPI_Send(result, __sz_R, MPI_DOUBLE, 0, taskID, MPI_COMM_WORLD); 
			} 
		} while (terminate == 0); 
	} 
	free (task);
	free (result);
} 

//workpool version 1 with debug messages
void SZ_Workpool_debug(void (*__sz_init)(int *T, int *D, int *R), void (*__sz_diffuse)(int taskID,double output[]),void (*__sz_compute)(int taskID, double input[], double output[]),void (*__sz_gather)(int taskID, double input[])  ) {

	int i; 
	int size; 
	int rank;     
	MPI_Status status; 

	int __sz_T, __sz_D, __sz_R; // # of tasks, # of data items in each task, # of data items in result of each task,

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	__sz_init(&__sz_T,&__sz_D,&__sz_R);

	double *task = (double*) malloc(__sz_D * sizeof(double));	
	double *result = (double*) malloc(__sz_R * sizeof(double)); 

	int taskID = 0;  // set to zero, programmer may alter this

	if (rank == 0) {   // Master
		int task_ptr = 0;	
		for (i = 1; i < size; i++) { 
			if (task_ptr < __sz_T) { 
				taskID = task_ptr;
				(*__sz_diffuse)(taskID,task);
				MPI_Send(task, __sz_D, MPI_DOUBLE, i, taskID, MPI_COMM_WORLD);
				printf("Master sending initial task %d to slave %d\n", taskID,i); 
				task_ptr++; 
			} else {
				MPI_Send(task, 0, MPI_DOUBLE, i, __sz_TERM, MPI_COMM_WORLD); 
				printf("Master sending terminator to slave %d\n", i);
			}
		} 
		for (i = 0; i < __sz_T; i++) { 
			MPI_Recv(result, __sz_R, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
			taskID = status.MPI_TAG; 
			printf("Master receiving task %d result from slave %d\n", taskID, status.MPI_SOURCE);
			(*__sz_gather)(taskID,result); 
			if (task_ptr < __sz_T) {
				taskID = task_ptr;  
				(*__sz_diffuse)(taskID,task); 
				MPI_Send(task, __sz_D, MPI_DOUBLE, status.MPI_SOURCE, taskID, MPI_COMM_WORLD);
				printf("Master sending task %d to slave %d\n", taskID, status.MPI_SOURCE); 
				task_ptr++; 
			} else  {
				MPI_Send(task, 0, MPI_DOUBLE, status.MPI_SOURCE, __sz_TERM, MPI_COMM_WORLD);
				printf("Master sending terminator to slave %d\n", status.MPI_SOURCE);
			} 
		} 
		printf("Master finished, %d tasks sent and received\n", task_ptr);
	} 
	else {			// Slave
		int terminate = 0; 
		do { 
			MPI_Recv(task, __sz_D, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
			taskID = status.MPI_TAG; 
			if (taskID == __sz_TERM) {
				terminate = 1; 
				printf("Slave %d received terminator\n", rank);
			} else { 
				printf("Slave %d received task ID %d\n", rank, taskID);
				(*__sz_compute)(taskID,task,result);	
				MPI_Send(result, __sz_R, MPI_DOUBLE, 0, taskID, MPI_COMM_WORLD);
				printf("Slave %d sending result back for task %d\n", rank, taskID); 
			} 
		} while (terminate == 0); 
	} 
	free (task);
	free (result);
} 

//workpool version 2
void SZ_Workpool2(void (*__sz_init)(int *T), void (*__sz_diffuse)(int taskID),void (*__sz_compute)(int taskID),void (*__sz_gather)(int taskID)) {

	int i; 
	int rank;     
	MPI_Status status; 

	int __sz_T; // # of tasks

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	buf_position = 2;		// data starts at location 2, after position of map

	__sz_init(&__sz_T);

	int taskID = 0;
	int source = 0;

	SZ_reset_map();

	if (rank == 0) {   // Master
		int task_ptr = 0;	
		for (i = 1; i < size; i++) { 	// master send task to each slave in turn first
			if (task_ptr < __sz_T) { 
				
				(*__sz_diffuse)(task_ptr);

				SZ_pack_map();
				MPI_Send(task_buffer, buf_position, MPI_PACKED, i, task_ptr, MPI_COMM_WORLD); 
				task_ptr++; 
			} else {
				MPI_Send(task_buffer, 0, MPI_PACKED, i, __sz_TERM, MPI_COMM_WORLD);
			} 
			SZ_reset_map();
		} 
		for (i = 0; i < __sz_T; i++) { // then hands out tasks to those that return results
			
			MPI_Recv(task_buffer, BUF_MAX_SIZE, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
			SZ_unpack_map();

			taskID = status.MPI_TAG; 
			source = status.MPI_SOURCE;

			(*__sz_gather)(taskID); 

			if (task_ptr < __sz_T) {
				 
				(*__sz_diffuse)(task_ptr);
 
				SZ_pack_map();
				MPI_Send(task_buffer, buf_position, MPI_PACKED, source, task_ptr, MPI_COMM_WORLD);
 
				task_ptr++; 
			} else  {
				MPI_Send(task_buffer, 0, MPI_PACKED, source, __sz_TERM, MPI_COMM_WORLD);
			}
			SZ_reset_map();
		}
 
	} else {			// Slave
		int terminate = 0; 
		do { 
			MPI_Recv(task_buffer, BUF_MAX_SIZE, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 

			taskID = status.MPI_TAG; 
			if (taskID == __sz_TERM) terminate = 1; 
			else { 
				SZ_unpack_map();
				(*__sz_compute)(taskID);

				SZ_pack_map();
				MPI_Send(task_buffer, buf_position, MPI_PACKED, 0, taskID, MPI_COMM_WORLD);
				SZ_reset_map(); 	
			} 
		} while (terminate == 0); 
	} 

} 

int busy[Pmax];				// busy slaves
int busy_slaves;			// number of busy slaves

void SZ_init_free_slave() {
	int i;
	if (size > Pmax) {printf("ERROR -Too many processes\n"); MPI_Abort(MPI_COMM_WORLD,999);}
	for (i = 1; i < size; i++) busy[i] = 0;  // initialize
	busy_slaves = 0;
	return;
}

void SZ_find_free_slave(int *slave) {	// return next free slave and set as busy
	int i;				// in current usage, always one slave will be free when called
	static int x = 0;
	for (i = 1; i < size; i++) {				
		x++;				// next slave
		if (x == size) x = 1; 		// round robin scheduling
		if (busy[x] == 0) {		// slave found
			busy[x] = 1;		// Set slave as busy
			busy_slaves++;
			*slave = x;		// return slave
			break;			// stop looking
		}	
	}
	//printf("Next slave selected is %d, number of busy slaves = %d\n",x,busy_slaves);
	return;
}

//workpool version 3 -- adds an actual task queue
void SZ_Workpool3(void (*__sz_init)(int *T), void (*__sz_diffuse)(int task_no),void (*__sz_compute)(int task_no),void (*__sz_gather)(int task_no) ) {
	int i; 
	int rank;     
	MPI_Status status; 

	int taskID = 0;
	int task_no;
	int source = 0;
	int no_tasks = 0; // # of tasks
	int task_ptr = 0;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	SZ_task_q_init();		// initialize queue pointers

	if (rank == 0) printf("Workpool version 3 Nov 25, 2015\n");	

	__sz_init(&no_tasks);

	if (no_tasks > 0) {  // added for version 2 compatibility
		SZ_Insert_task(task_ptr);
		task_ptr++;
	}

	SZ_reset_map();

	if (rank == 0) {   // Master

	   SZ_init_free_slave(); // initalize slave status

	   do {
		while ((task_q_no_tasks > 0) && (busy_slaves < size-1)) {

			SZ_Remove_task(&taskID);

			if ((no_tasks > 0) && (task_ptr < no_tasks)) {  // added for version 2 compatibility
				SZ_Insert_task(task_ptr);
				task_ptr++;
			}

			(*__sz_diffuse)(taskID); 	// insert anything else

			SZ_find_free_slave(&i);	// select a slave and set busy, increment busy_slaves

			SZ_pack_map();
			MPI_Send(task_buffer, buf_position, MPI_PACKED, i, taskID, MPI_COMM_WORLD);
			SZ_reset_map();

		} 

		if ((busy_slaves == 0) && (task_q_no_tasks == 0)) break;

		if (busy_slaves > 0) {			// then hands out tasks to those that return results

			MPI_Recv(task_buffer, BUF_MAX_SIZE, MPI_PACKED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
			
			SZ_unpack_map();

			taskID = status.MPI_TAG; 
			source = status.MPI_SOURCE;

			busy[source] = 0;	 		// set source as free
			busy_slaves--; 

			(*__sz_gather)(taskID);   // possibly add new tasks
		}
			
	   } while (1);

	   for (i = 1; i < size; i++) MPI_Send(task_buffer, 0, MPI_PACKED, i, __sz_TERM, MPI_COMM_WORLD);

	} else {			// Slave
		int terminate = 0; 
		do { 
			MPI_Recv(task_buffer, BUF_MAX_SIZE, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
			taskID = status.MPI_TAG; 

			if (taskID == __sz_TERM) terminate = 1;
			else { 
				SZ_unpack_map();
				(*__sz_compute)(taskID);
				SZ_pack_map();

				MPI_Send(task_buffer, buf_position, MPI_PACKED, 0, taskID, MPI_COMM_WORLD);  // send back result
				SZ_reset_map(); 	
			} 
		} while (terminate == 0); 
	} 

} 

// Pipeline version 1

void SZ_terminate() {

	terminate = 1;
	return;
}

void SZ_Debug() {  // could possibly do this using conditional compilation

	debug = 1;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // needed as SZ_Debug() called before starting pattern
	if (rank == 0) printf("Debug mode displaying messages\n");

	return;

}

void SZ_Pipeline(void (*__sz_init)(int *T, int *D, int *R), void (*__sz_diffuse)(int taskID,double output[]),void (*__sz_compute)(int taskID, double input[], double output[]),void (*__sz_gather)(int taskID, double input[])  ) {

	int i,j;
	int tag = 1; 
	int src,dest;

	int T, D, R; // # of tasks, # of data items in each task, # of data items in result of each task

	MPI_Status status;

	int taskID, task_ptr = 0;			// tags should really be insigned ints
	int A_taskID = NOTASK,B_taskID = NOTASK;

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	__sz_init(&T,&D,&R);

	// for Bsend
	//int buf_size = 2 *(D * sizeof(double)  + MPI_BSEND_OVERHEAD);  // number of bytes needs for Bsend messages
	//char *buffer = (char*) malloc(buf_size);
	//MPI_Buffer_attach(buffer,buf_size);

	double *A = (double*) malloc(D * sizeof(double));		// what is sent out	
	double *B = (double*) malloc(D * sizeof(double)); 		// what is received

	for(i = 0; i < T*(size-1); i++)  {

		if (rank == 0) {
			A_taskID = task_ptr;				// next task to issue
			__sz_diffuse(A_taskID,A);				// load A in master with data and
			task_ptr++;
		}
			
		MPI_Barrier(MPI_COMM_WORLD);			// wait for diffuse to complete

		for (src = 0; src < size; src++)  {		// send to adjacent processes
			dest = src + 1;
			if (dest == size) dest = 0;
			if (rank == src) 
				MPI_Ssend(A, D, MPI_DOUBLE, dest, A_taskID, MPI_COMM_WORLD); // Bsend seems unnecessary as 2 waits for 1, 3 waits for 2, etc.
			if (rank == dest) {
				MPI_Recv(B, D, MPI_DOUBLE, src, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				B_taskID = status.MPI_TAG; 
			}
		}
		  
		if (rank == 0) { 
			__sz_gather(B_taskID,B);			// here master just print out what is received	
		  
		} else {
			__sz_compute(B_taskID,B,A);			// all procs, determine largest number to put in A
			A_taskID = B_taskID;
		}

		// testing

	if (debug == 1) {
		if (rank == 0) 
			printf("Master sends task %d data = %3.0f \n",A_taskID,A[0]);
		MPI_Barrier(MPI_COMM_WORLD);	// does not seems to guarantee slaves will wait for printed message to display

		for (i = 1; i < size;i++) {
			if (rank == i) 
				printf("Slave %d receives task %d data = %3.0f and returns %3.0f\n",rank,B_taskID,B[0],A[0]);
			MPI_Barrier(MPI_COMM_WORLD);
		}
	
		if (rank == 0) 
			printf("Master receives task %d, %3.0f \n",B_taskID,B[0]);
		MPI_Barrier(MPI_COMM_WORLD);
	}

		MPI_Bcast(&terminate, 1, MPI_INT, 0, MPI_COMM_WORLD);	// slaves will wait to get message
		if (terminate == 1) break;

	}

	//MPI_Buffer_detach(buffer,&buf_size);

	return;
}



















