// Suzaku macros, version 2.00
// Implemented by B. Wilkinson  Dec 29, 2015. 

#include "mpi.h"


//#ifndef string		// to avoid putting in twice

#include <string.h>

//#endif

// Suzaku global variables
	int __sz_count; 
	int __sz_size;     
	int __sz_rank;     
	int __sz_tag=1; 
	int __sz_datasize;
	int __sz_i;    
	int __sz_j;
	MPI_Status __sz_status; 

// Suzaku macros

#define SZ_Init(P) \
	MPI_Init(&argc, &argv); \
	MPI_Comm_size(MPI_COMM_WORLD, &__sz_size); \
	MPI_Comm_rank(MPI_COMM_WORLD, &__sz_rank); \
	P = __sz_size; \
	if (__sz_rank == 0) {
	
#define SZ_Get_process_num() \
	__sz_rank

#define SZ_Master \
	if (__sz_rank == 0) 

#define SZ_Process(x) \
	if (__sz_rank == x)                     

#define SZ_Slave \
	if (__sz_rank != 0) 

#define SZ_Parallel_begin \
	} \
	// blank line 

#define SZ_Parallel_end \
	MPI_Barrier(MPI_COMM_WORLD);\
	if (__sz_rank == 0) { 

#define SZ_Finalize()    \
	} \
	MPI_Finalize()

#define SZ_Point_to_point(p1, p2, a, b) \
do {\
	__sz_datasize = sizeof(*(a)); \
	strcpy(__sz_string,#a);\
	__sz_ptr=strchr(__sz_string,'&');\
	if (__sz_ptr == __sz_string) __sz_count = 1;\
	else __sz_count = sizeof(a) / __sz_datasize;\
	if (__sz_datasize == sizeof(char)) {\
	   if (__sz_rank == p1)  \
		MPI_Ssend(a, __sz_count, MPI_CHAR, p2, __sz_tag, MPI_COMM_WORLD); \
	   if (__sz_rank == p2)  \
		MPI_Recv(b, __sz_count, MPI_CHAR, p1, __sz_tag, MPI_COMM_WORLD, &__sz_status);\
	} else if (__sz_datasize == sizeof(int)) {\
	   if (__sz_rank == p1)  \
		MPI_Ssend(a, __sz_count, MPI_INT, p2, __sz_tag, MPI_COMM_WORLD); \
	   if (__sz_rank == p2)  \
		MPI_Recv(b, __sz_count, MPI_INT, p1, __sz_tag, MPI_COMM_WORLD, &__sz_status);\
	} else if (__sz_datasize == sizeof(double)) {\
	  if (__sz_rank == p1)  \
		MPI_Ssend(a, __sz_count, MPI_DOUBLE, p2, __sz_tag, MPI_COMM_WORLD); \
	  if (__sz_rank == p2)  \
		MPI_Recv(b, __sz_count, MPI_DOUBLE, p1, __sz_tag, MPI_COMM_WORLD, &__sz_status);\
	} else if (__sz_datasize > sizeof(double)) {\
	  __sz_count = sizeof(a) / sizeof(double);\
	  if (__sz_rank == p1)  \
		MPI_Ssend(a, __sz_count, MPI_DOUBLE, p2, __sz_tag, MPI_COMM_WORLD); \
	  if (__sz_rank == p2)  \
		MPI_Recv(b, __sz_count, MPI_DOUBLE, p1, __sz_tag, MPI_COMM_WORLD, &__sz_status);\
	} else printf("ERROR IN SZ_Point_to_point --  CANNOT IDENTIFY ARUMENT TYPE\n");\
} while (0)

// for last if covers multidimensional arrays, then assumed to be doubles

#define SZ_Scatter(a, b) \
do {\
	__sz_count = sizeof(b) / ( sizeof(double) ); \
	MPI_Scatter(a,__sz_count,MPI_DOUBLE,b,__sz_count,MPI_DOUBLE,0,MPI_COMM_WORLD);\
	MPI_Barrier(MPI_COMM_WORLD);\
} while (0)
// Note: size is size of b, the destination

#define SZ_Broadcast(a) \
do {\
	__sz_datasize = sizeof(*(a)); \
	strcpy(__sz_string,#a);\
	__sz_ptr=strchr(__sz_string,'&');\
	if (__sz_ptr == __sz_string) __sz_count = 1;\
	else __sz_count = sizeof(a) / __sz_datasize;\
	if (__sz_datasize == sizeof(char)) {\
	   MPI_Bcast(a, __sz_count, MPI_CHAR, 0, MPI_COMM_WORLD); \
	} else if (__sz_datasize == sizeof(int)) {\
	   MPI_Bcast(a, __sz_count, MPI_INT, 0, MPI_COMM_WORLD); \
	} else if (__sz_datasize == sizeof(double)) {\
	  MPI_Bcast(a, __sz_count, MPI_DOUBLE, 0, MPI_COMM_WORLD); \
	} else if (__sz_datasize > sizeof(double)) {\
	  __sz_count = sizeof(a) / sizeof(double);\
	  MPI_Bcast(a, __sz_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);\
	} else printf("ERROR IN SZ_Broadcast --  CANNOT IDENTIFY ARUMENT TYPE\n");\
	MPI_Barrier(MPI_COMM_WORLD);\
} while (0)
// Notes: not really good to have broadcast within if statements. Assumes multidimensional arrays are doubles

#define SZ_AllBroadcast(a) \
do {\
	__sz_count = sizeof(a) / (sizeof(double) * __sz_size); \
	for (__sz_i = 0; __sz_i < __sz_size; __sz_i++)  \
	  MPI_Bcast(a[__sz_i], __sz_count, MPI_DOUBLE, __sz_i, MPI_COMM_WORLD);\
	MPI_Barrier(MPI_COMM_WORLD);\
} while (0)

#define SZ_Gather(a, b) \
do {\
	__sz_count = sizeof(a) / (sizeof(double)); \
	MPI_Gather(a,__sz_count,MPI_DOUBLE,b,__sz_count,MPI_DOUBLE,0,MPI_COMM_WORLD); \
	MPI_Barrier(MPI_COMM_WORLD);\
} while (0)
// Note: size is size of a, the source

#define SZ_Barrier() \
	MPI_Barrier(MPI_COMM_WORLD)

#define SZ_Wtime() \
	MPI_Wtime()

// for workpool version 2
char *__sz_ptr = NULL;
char __sz_string[10];

//SZ_Put and SZ_Get Assumes multidimensional arrays are doubles
#define SZ_Put(key,x) \
	__sz_datasize = sizeof(*(x));\
	strcpy(__sz_string,#x);\
	__sz_ptr=strchr(__sz_string,'&');\
	if (__sz_ptr == __sz_string) __sz_count = 1;\
	else __sz_count = sizeof(x) / __sz_datasize;\
	if (__sz_datasize == sizeof(char)) SZ_pack_char(key,x,__sz_count);\
	else if (__sz_datasize == sizeof(int)) SZ_pack_int(key,x,__sz_count);\
	else if (__sz_datasize == sizeof(double)) SZ_pack_double(key,x,__sz_count);\
	else if (__sz_datasize > sizeof(double)) {\
	__sz_count = sizeof(x) / sizeof(double);\
	SZ_pack_double(key,x,__sz_count);\
	} else printf("ERROR: SZ_Put --  CANNOT IDENTIFY ARUMENT TYPE\n");\

#define SZ_Get(key,x) \
	__sz_datasize = sizeof(*(x));\
	strcpy(__sz_string,#x);\
	__sz_ptr=strchr(__sz_string,'&');\
	if (__sz_ptr == __sz_string) __sz_count = 1;\
	else __sz_count = sizeof(x) / __sz_datasize;\
	if (__sz_datasize == sizeof(char)) SZ_unpack_char(key,x,__sz_count);\
	else if (__sz_datasize == sizeof(int)) SZ_unpack_int(key,x,__sz_count);\
	else if (__sz_datasize == sizeof(double)) SZ_unpack_double(key,x,__sz_count);\
	else if (__sz_datasize > sizeof(double)) {\
	__sz_count = sizeof(x) / sizeof(double);\
	SZ_unpack_double(key,x,__sz_count);\
	} else printf("ERROR: SZ_Get --  CANNOT IDENTIFY ARUMENT TYPE\n");\

// using do {\} while (0) does not seem to be foolproof, so use { .. } in user code

// Workpool function prototype

//extern void SZ_Workpool(void (*__sz_init)(int *T, int *D, int *R), void (*__sz_diffuse)(int taskID,double output[]),void (*__sz_compute)(int taskID, double input[], double output[]),void (*__sz_gather)(int taskID, double input[])  );

// Generalized pattern routines

extern void SZ_Pattern(const char* pattern);
// extern void SZ_Send(double a[N], double b[P][N]);
extern void SZ_PrintGraph(void);

// new workpool version

extern void SZ_Map_name(char key[10], int position);

 
