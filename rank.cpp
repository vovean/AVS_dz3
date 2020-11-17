#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <string>
#include <ctime>

using namespace std;

/// <summary>
/// Create square matrix with random values between -100 and 100 inclusive
/// </summary>
/// <param name="size">Size of matrix</param>
/// <returns>matrix as double**</returns>
double** createMatrix(int size) {
	// create matrux itself as 2-dimensional double pointer array
	double** matrix = new double* [size];

	cout << "Please, enter matrix of size " << size << " x " << size << ". " << size * size << " numbers in total" << endl;
	// read matrix from user
	for (int i = 0; i < size; i++)
	{
		matrix[i] = new double[size];
		for (int j = 0; j < size; j++)
		{
			// random number between -100 and 100 inclusive
			cin >> matrix[i][j];
		}
	}
	// print matrix for clarification
	cout << "Your matrix:" << endl;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			cout << matrix[i][j] << '\t';
		}
		cout << endl;
	}
	return matrix;
}


/// <summary>
/// Subtracts line from others for Row Reduce method
/// </summary>
/// <param name="curThr">Current thread number</param>
/// <param name="thrNum">Number of threads</param>
/// <param name="mSize">Matrix size</param>
/// <param name="matrix">Matrix as double**</param>
/// <param name="line">Line number</param>
/// <param name="leading_el">Fist non-zero element in line</param>
void subtractLineFromOthers(int currentThread, int threadsAmount, int matrixSize, double* matrix[], int line, int primeElement) {
	// iterate over all threads
	for (int thr = currentThread; thr < matrixSize; thr += threadsAmount)
	{
		// check we do not subtract from itself and leading element is not zero
		if (thr != line && matrix[thr][primeElement] != 0)
		{
			// subtract line #j from others 
			for (int p = primeElement + 1; p < matrixSize; p++)
			{
				matrix[thr][p] -= matrix[line][p] * matrix[thr][primeElement];
			}
		}
	}
}

/// <summary>
/// Calculates rank of the given matrix
/// </summary>
/// <param name="threadsCount">Amount of threads to use for calculations</param>
/// <param name="matrix">Matrix as 2-dimnesional double array</param>
/// <param name="matrixSize">Size of the provided matrix</param>
/// <returns></returns>
int calculateRank(int threadsCount, double* matrix[], int matrixSize)
{
	int rank = matrixSize;
	vector<bool> usedLines(matrixSize, false);
	for (int i = 0; i < matrixSize; i++)
	{
		// need j later so define it outside for loop
		int j;
		for (j = 0; j < matrixSize; j++)
		{
			// if current line is used or elemnt is zero than exit loop
			if (!usedLines[j] && matrix[j][i] != 0)
			{
				break;
			}
		}
		if (j == matrixSize)
			rank--;
		else
		{
			// create pointer array of threads
			thread* threads = new thread[threadsCount];
			// mark current line as used
			usedLines[j] = true;
			// divide whole string by its first element
			for (int p = i + 1; p < matrixSize; p++)
			{
				matrix[j][p] /= matrix[j][i];
			}
			// create all threads
			for (int k = 0; k < threadsCount; k++)
			{
				threads[k] = thread{
					subtractLineFromOthers,
					k, threadsCount, matrixSize, ref(matrix), j, i
				};
			}
			// join all threads
			for (int k = 0; k < threadsCount; k++) threads[k].join();
			// free memory
			delete[] threads;
		}
	}
	return rank;
}


int N, threadNumber, rank;
double** matrix;
int main()
{
	// initialize randomizer
	srand(time(0));

	// read matrix size
	cout << "Enter the size of matrix (N x N). N: ";
	cin >> N;
	if (N < 1) { // check invalid input
		cerr << "Invalid amount of threads: " << N;
		exit(1);
	}

	// create matrix
	matrix = createMatrix(N);

	// read amount of threads to be used in runtime
	cout << "Enter amount of threads to use for determining the rank of the matrix: ";
	cin >> threadNumber; //Узнаем число потоков
	if (threadNumber < 1) { // check invalid input
		cerr << "Invalid amount of threads";
		exit(2);
	}

	cout << "Rank: " << calculateRank(threadNumber, matrix, N);

	// delete whole matrix from memory
	for (int i = 0; i < N; i++) delete[] matrix[i];
	delete[] matrix;
}