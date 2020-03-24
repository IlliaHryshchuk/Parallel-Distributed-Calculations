#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
using namespace std;


void task_lvl_1(int rank, int root0, int root1, char *buf0, char *buf1, int size);
void task_lvl_2(int rank, int p1, int p2, int* returnedArr, MPI_Status status);
void bubbleSort(int *a, int n);


int main(int argc, char *argv[])
{
	int rank, size, root0 = 0, root1 = 1, arr[12], returnedArr[4];
	char buf0[25], buf1[25];
	MPI_Status status;
	status.count = 0;

	srand(time(NULL));
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	//lvl1
	if (rank == 0)
		strcpy(buf0, "message from root 0");
	else if (rank == 1)
		strcpy(buf1, "message from root 1");
	MPI_Bcast(&buf0, 25, MPI_CHAR, root0, MPI_COMM_WORLD);
	MPI_Bcast(&buf1, 25, MPI_CHAR, root1, MPI_COMM_WORLD);
	if (rank % 2 == 0)
	{
		cout << "\nFrom rank #" << rank << ": The received message via MPI_Bcast is: " << buf0;
		cout << "\nFrom rank #" << rank << ": The received message via MPI_Bcast is: " << buf1;
	}


	//lvl2
	if (size == 3)
	{
		if (rank == 0)
		{
			cout << "\n\nFrom rank #" << rank << ": Not sorted array: ";
			for (int i = 0; i < 12; i++)
			{
				arr[i] = rand() % 100;
				cout << arr[i] << " ";
			}
		}

		MPI_Scatter(arr, 4, MPI_INT, returnedArr, 4, MPI_INT, root0, MPI_COMM_WORLD);
		task_lvl_2(rank, 0, 1, returnedArr, status);
		task_lvl_2(rank, 0, 2, returnedArr, status);
		task_lvl_2(rank, 1, 2, returnedArr, status);
		MPI_Gather(returnedArr, 4, MPI_INT, arr, 4, MPI_INT, root0, MPI_COMM_WORLD);

		if (rank == 0)
		{
			cout << "\nFrom rank #" << rank << ": Sorted array:     ";
			for (int i = 0; i < 12; i++)
			{
				cout << arr[i] << " ";
			}
		}

	}
	else
		if (rank == 0)
			cout << "\nFrom rank #" << rank << ": You need to create 3 processes to enable bubble sort\n";


	MPI_Finalize();
	return 0;
}


void task_lvl_1(int rank, int root0, int root1, char *buf0, char *buf1, int size)
{
	MPI_Bcast(&buf0, 25, MPI_CHAR, root0, MPI_COMM_WORLD);
	MPI_Bcast(&buf1, 25, MPI_CHAR, root1, MPI_COMM_WORLD);
	if (rank % 2 == 0)
	{
		cout << "\nFrom rank #" << rank << ": The received message via MPI_Bcast is: " << buf0;
		cout << "\nFrom rank #" << rank << ": The received message via MPI_Bcast is: " << buf1;
	}
}


void task_lvl_2(int rank, int p1, int p2, int* returnedArr, MPI_Status status)
{
	if (rank == p1)
	{
		int bf[8];
		MPI_Recv(bf, 4, MPI_INT, p2, 0, MPI_COMM_WORLD, &status);
		for (int i = 4; i < 8; i++)
		{
			bf[i] = returnedArr[i - 4];
		}
		bubbleSort(bf, 8);
		for (int i = 0; i < 4; i++)
		{
			returnedArr[i] = bf[i];
		}
		MPI_Ssend(bf + 4, 4, MPI_INT, p2, 0, MPI_COMM_WORLD);
	}
	if (rank == p2)
	{
		MPI_Ssend(returnedArr, 4, MPI_INT, p1, 0, MPI_COMM_WORLD);
		MPI_Recv(returnedArr, 4, MPI_INT, p1, 0, MPI_COMM_WORLD, &status);
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