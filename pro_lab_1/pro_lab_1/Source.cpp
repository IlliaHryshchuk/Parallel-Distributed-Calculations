#include <mpi.h>
#include <iostream>
#include <time.h>
#include <cstdlib>

using namespace std;


int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv); 
	srand(time(NULL));
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (rank == 0)
	{
		const int sizeOfArr = 99;
		int arr[sizeOfArr];
		float arithmeticMean = 0;

		cout << "\nFrom rank #" << rank << ": Members of array:\n";
		for (int i = 0; i < sizeOfArr; i++)
		{
			arr[i] = rand() % 100 + 1;
			cout << arr[i] << " ";
		}

		for (int i = 0; i < sizeOfArr; i++)
			arithmeticMean += arr[i];

		arithmeticMean = arithmeticMean / sizeOfArr;
		cout << "\nFrom rank #" << rank << ": Arithmetic mean = " << arithmeticMean << endl;
	}
	else if (rank == 1)
	{
		const int sizeOfArr = 5;
		float sumUnderDiag = 0, sumOverDiag = 0, arr[5][5];

		cout << "\nFrom rank #" << rank << ": Members of matrix:";
		for (int i = 0; i < sizeOfArr; i++)
		{
			cout << "\n";
			for (int j = 0; j < sizeOfArr; j++)
			{
				arr[i][j] = (rand() % 10000 / 100.0)- 50;
				cout << arr[i][j] << " ";
			}
		}


		for (int i = 0; i < sizeOfArr; i++)
			for (int j = 0; j < sizeOfArr; j++)
			{
				if (i < j)
					if (arr[i][j] < 0)
						sumOverDiag += arr[i][j];
			}


		for (int i = 0; i < sizeOfArr; i++)
			for (int j = 0; j < sizeOfArr; j++)
			{
				if (i > j)
					if (arr[i][j] < 0)
						sumUnderDiag += arr[i][j];
			}

		cout << "\nFrom rank #" << rank << ":\nSum of elements under main diag = " << sumUnderDiag << "\nSum of elements over main diag = " << sumOverDiag << endl;
	}
	else
	{
		cout << "\nFrom rank #" << rank << ": is not defined\n";
	}


	MPI_Finalize();

	//system("pause");
	return 0;
}
