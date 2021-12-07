// Suzaku Workpool version 2 with put and get test program
// B. Wilkinson Nov 16, 2015

#include <stdio.h>
//#include <string.h>
#include "suzaku.h" 
 
#define T 4		// number of tasks, max = INT_MAX - 1

// workpool functions to be provided by programmer:

void init(int *tasks) {  // sets number of tasks
	*tasks = T;	
	return; 
}

void diffuse(int taskID) {
	int j;
	char w[] = "Hello World";
	static int x = 1234; 		// only initialized first time function called
	static double y = 5678; 			
	double z[2][3];
	z[0][0] = 357;
	z[1][1] = 246;

	SZ_Put("w",w);
	SZ_Put("x",&x);
	SZ_Put("y",&y);
	SZ_Put("z",z);

	printf("Diffuse Task sent:     taskID = %2d, w = %s, x = %5d, y = %8.2f, z[0][0] = %8.2f, z[1][1] = %8.2f\n",taskID, w, x, y,z[0][0],z[1][1]);

	x++;
	y++;

	return;
}
	
void compute(int taskID) {  // function done by slaves - simply passing data multiplied by 10 in a different order
	char w[12] = "-----------"; 
	int x = 0;
	double y = 0;
	double z[2][3];
	int slave; 

	z[0][0] = 0;
	z[1][1] = 0;

	slave = SZ_Get_process_num();

	SZ_Get("z",z);
	SZ_Get("x",&x);
	SZ_Get("w",w);
	SZ_Get("y",&y);

	printf("Slave %d Task received: taskID = %2d, w = %s, x = %5d, y = %8.2f, z[0][0] = %8.2f, z[1][1] = %8.2f\n",slave,taskID, w, x, y,z[0][0],z[1][1]);
	x = x * 10;
	y = y * 10;
	z[0][0] = z[0][0] * 10;
	z[1][1] = z[1][1] * 10;
	printf("Slave %d Result:        taskID = %2d, w = %s, x = %5d, y = %8.2f, z[0][0] = %8.2f, z[1][1] = %8.2f\n",slave,taskID, w, x, y,z[0][0],z[1][1]);
 
	SZ_Put("xx",&x);  // use different names for test, could have been same names
	SZ_Put("yy",&y);
	SZ_Put("zz",z);
	SZ_Put("ww",w)
	
	return;
}

void gather(int taskID) {	// function done by master collecting slave results. Final results computed by master
	char w[12] = "-----------";
	int x = 0;
	double y = 0;
	double z[2][3];
	z[0][0] = 0;
	z[1][1] = 0;

	SZ_Get("ww",w);
	SZ_Get("zz",z);
	SZ_Get("xx",&x);
	SZ_Get("yy",&y);

	printf("Gather Task received:  taskID = %2d, w = %s, x = %5d, y = %8.2f, z[0][0] = %8.2f, z[1][1] = %8.2f\n",taskID, w, x, y,z[0][0],z[1][1]);

	return;
}

// additional routines used in this application


int main(int argc, char *argv[]) {
						// All variables declared here are in every process
	int i;
	int P;					// number of processes, set by SZ_Init(P) 			

	SZ_Init(P);				// initialize MPI message-passing environment
							// sets P to be number of processes

	printf("number of tasks = %d\n",T);
	
	SZ_Parallel_begin

		SZ_Workpool2(init,diffuse,compute,gather);
	
	SZ_Parallel_end;			// end of parallel
	
	SZ_Finalize(); 

	return 0;
}
