
/*

TO RUN: mpiexec -n p SieveOfEratosthenes.exe n
WHERE: p is the number of processors
	   n is the range you want to find primes until, 2,...,n

Optimizations:
1. All even numbers, with the except of 2, are composite. So our program doesn't check them. Instead we just add 1,
   to the total count of primes at the end to account for 2.
2. At first we had the master Bcast k to indicate what prime needed to be checked but we got rid of that as well to
   again make things faster.
*/
#include <mpi.h>
#include <algorithm>
#include <iostream>

using namespace std;

int findPLow(int id, int p, int n) {
	return ((id) * (n) / (p) / 2);
}

int findPHigh(int id, int p, int n) {
	return findPLow(id + 1, p, n) - 1;
}

int findPSize(int id, int p, int n) {
	return findPLow((id)+ 1, p, n) - findPLow(id, p, n);
}

int main(int argc, char** argv) {
	//Declare Variables
	int localPrime, globalPrime, localSize;
	int i, id, n, p, k, phigh, plow, currPrime, firstk, nextPrime, doubledPrime, sqrN, primeMultiple;
	int perp, perplow, perphigh, perpfirst;

	//arrays to keep track of prime numbers and maked number
	char*   marked;
	char*   primes;
	
	//Keeps track of execution time
	double  elapsed_time;

	//Initialize MPI
	MPI_Init(&argc, &argv);

	//start the timer
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = -MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	//Exit program if we don't have the right amount of arguments
	if (argc != 2) {
		if (id == 0)
			cout << "Not enough arguments, please include the number of processors and the 'n' you want to find primes until";
		MPI_Finalize();
		return 0;
	} 

	//Get n's value
	n = atoi(argv[1]);

	//give each processor the range of the array they are responsible for
	plow = 3 + findPLow(id, p, n - 1)  * 2;
	phigh = 3 + findPHigh(id, p, n - 1) * 2;

	//designate each processors local array size
	localSize = findPSize(id, p, n - 1);
	
	//get the square root of n, used when iterating thru array
	sqrN = sqrt(n);

	//create a primes array for each processor of size square root n +1
	primes = (char*)calloc(sqrN + 1, 1);

	//Designate even array positons
	for (primeMultiple = 2; primeMultiple <= sqrN; primeMultiple += 2) {
		primes[primeMultiple] = 1;
	} 

	//Create a corresponding marked array for each processor
	marked = (char*)calloc(localSize * sizeof(char), 1);
	perp = 1024 * 1024;
	
	//Designate the lowest and highest number in each processor's array
	perplow = plow;
	perphigh = min(phigh,plow + perp * 2);


	//iterate through the array for each processor to find primes
	//If a number is not marked it means it is prime and all multiples of that number are then also marked as prime
	for (perpfirst = 0; perpfirst < localSize; perpfirst += perp) {
		//currPrime indicates the current number that the array is being checked for to locate composites
		//we iterate only through the odd multiples of the currPrime to optimize the program and make it faster
		for (currPrime = 3; currPrime <= sqrN; currPrime++) {
			if (primes[currPrime] == 1)
				continue;
			if (currPrime * currPrime > perplow) {
				k = currPrime * currPrime;
			}
			else {
				if (!(perplow % currPrime)) {
					k = perplow;
				}
				else {
					k = currPrime - (perplow % currPrime) + perplow;
				}
			}
			//consider only odd multiples of the prime number
			//using bitwise AND operation
			if ((k + currPrime) & 1) {
				k += currPrime;
			}
			firstk = (k - 3) / 2 - findPLow(id, p, n - 1);
			doubledPrime = currPrime << 1;
			nextPrime = doubledPrime / 2;
			for (i = k; i <= phigh; i += doubledPrime) {
				marked[firstk] = 1;
				firstk += nextPrime;
			}
		}
		perplow += perp * 2;
		perphigh = min(phigh, perphigh + perp * 2);
	}

	//find the number of local primes that each processor has by counting the non-marked elements
	//in the marked array
	localPrime = 0;
	for (i = 0; i < localSize; i++) {
		if (!marked[i])
			localPrime++;
	}

	//Sum number of primes from each processor into globalPrime variable
	MPI_Reduce(&localPrime, &globalPrime, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	 //add timer info to elapsed_time data item
	elapsed_time += MPI_Wtime();

	//Master Displays Results
	if (id == 0) {
		//Since we skipped over even numbers and started at 3
		//we add 1 to the count of primes to account for 2, which is an even prime
		globalPrime += 1;

		//print number of primes and how long the program took to execute
		cout << "There exist "<< globalPrime <<" primes between 2 and " << n <<"\n";
		cout << "The total elapsed time was " << elapsed_time << "s\n";
	}
	MPI_Finalize();
	return 0;
}