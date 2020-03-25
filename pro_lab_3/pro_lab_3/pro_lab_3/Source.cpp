#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
using namespace std;


void task_lvl_2(int rank, int p1, int p2, int* returnedArr, int sizeOfArr);
void bubbleSort(int *a, int n);


int main(int argc, char *argv[])
{
	const int sizeOfArr = 20;
	int rank, size, root0 = 0;

	srand(time(NULL));
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	if (size > 20 || size < 2) {
		cout << "You need to create <=20 or >= 2 processes to enable bubble sort\n";
		MPI_Finalize();
		return 0;
	}


	int numOfElemsToEachProces, numOfExtraProces, sizeOfAdditionalArr;

	//to calculate extra size for an array
	numOfExtraProces = sizeOfArr % size;
	numOfElemsToEachProces = (sizeOfArr - numOfExtraProces) / size;
	if (numOfExtraProces >= 1) {
		numOfElemsToEachProces += 1;
		sizeOfAdditionalArr = sizeOfArr - numOfExtraProces + size;
	}
	else
		sizeOfAdditionalArr = sizeOfArr;
	int *returnedArr = new int[numOfElemsToEachProces];
	int* additionalArr = new int[sizeOfAdditionalArr];


	if (rank == 0)
	{
		cout << "\n\nFrom rank #" << rank << ": Not sorted array: ";
		for (int i = 0; i < sizeOfArr; i++) {
			additionalArr[i] = rand() % 100;
			cout << additionalArr[i] << " ";
		}
		for (int i = sizeOfArr; i < sizeOfAdditionalArr; i++)//to add extra elemnts for an array
			additionalArr[i] = -99;
	}


	MPI_Scatter(additionalArr, numOfElemsToEachProces, MPI_INT, returnedArr, numOfElemsToEachProces, MPI_INT, root0, MPI_COMM_WORLD);
	for (int i = 0; i < size - 1; i++)
		for (int j = i + 1; j < size; j++) {
			task_lvl_2(rank, i, j, returnedArr, numOfElemsToEachProces);
		}
	MPI_Gather(returnedArr, numOfElemsToEachProces, MPI_INT, additionalArr, numOfElemsToEachProces, MPI_INT, root0, MPI_COMM_WORLD);


	if (rank == 0)
	{
		cout << "\nFrom rank #" << rank << ": Sorted array:     ";
		if (numOfExtraProces >= 1)
			for (int i = size - numOfExtraProces; i < sizeOfAdditionalArr; i++)
			{
				cout << additionalArr[i] << " ";
			}
		else
			for (int i = 0; i < sizeOfAdditionalArr; i++)
			{
				cout << additionalArr[i] << " ";
			}
	}


	delete[] returnedArr;
	delete[] additionalArr;
 	MPI_Finalize();
	return 0;
}


void task_lvl_2(int rank, int p1, int p2, int* returnedArr, int sizeOfArr)
{
	MPI_Request request;
	MPI_Status status;
	if (rank == p1)
	{
		int* bf = new int[sizeOfArr * 2];
		MPI_Irecv(bf, sizeOfArr, MPI_INT, p2, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		for (int i = sizeOfArr; i < sizeOfArr * 2; i++)
		{
			bf[i] = returnedArr[i - sizeOfArr];
		}
		bubbleSort(bf, sizeOfArr * 2);
		for (int i = 0; i < sizeOfArr; i++)
		{
			returnedArr[i] = bf[i];
		}
		MPI_Isend(bf + sizeOfArr, sizeOfArr, MPI_INT, p2, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
	}
	if (rank == p2)
	{
		MPI_Isend(returnedArr, sizeOfArr, MPI_INT, p1, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		MPI_Irecv(returnedArr, sizeOfArr, MPI_INT, p1, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
	}
}

void bubbleSort(int *a, int n)
{
	for (int i = n - 1; i >= 0; i--)
		for (int j = 0; j < i; j++)
		{
			if (a[j] > a[j + 1])
				swap(a[j], a[j + 1]);
		}
}