#include <iostream>
#include <chrono>

using namespace std;

int main(int argc, char** argv) {
	//Variable Declaration
	int n;
	int* prime;
	n = atoi(argv[1]);
	int numPrimes = 0;

	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	

	//cout << "Time difference = " << std::chrono::duration_cast<chrono::microseconds>(end - begin).count() << "[�s]" << endl;
	
	double  elapsed_time;
	/*Initialise each number to max
	 *number of factors possible
	 *i.e. the number itself
	 */
	prime = new int[n + 1];
	for (int i = 2; i <= n; i++)
		prime[i] = i;

	/*All composite numbers are marked
	 *with number 0. The square of numbers is
	 *taken and all numbers in the range are marked
	 *as 0
	 */
	for (int i = 2; i*i <= n; i++)
		for (int j = i * i; j <= n; j += i)
			prime[j] = 0;
	
	//Print out the unmarked numbers
	for (int i = 2; i <= n; i++)
		if (prime[i] != 0)
			numPrimes++;
	
	

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	float timeDifference = chrono::duration_cast<chrono::nanoseconds> (end - begin).count();
	cout << "The number of Primes between 2 and " << n << " are " << numPrimes << endl;
	cout << "Elapsed Time = " << timeDifference/1000000000 << " seconds" << endl;
	return 0;
}