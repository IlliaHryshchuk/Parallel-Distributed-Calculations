#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <omp.h>
using namespace std;


void task_1();
void printMatrix(vector<vector<int>> matrix);
vector<vector<int>> generateMatrix(vector<vector<int>> matrix);

int main()
{
	task_1();

	system("pause");
	return 0;
}



void task_1()
{
	constexpr int m = 9, n = 6;
	vector<vector<int>> matrix(m, vector<int>(n));

	matrix = generateMatrix(matrix);
	printMatrix(matrix);

	#pragma omp parallel num_threads(m) shared(matrix) 
	{
		#pragma omp for
		for (int i = 0; i < m; i++)
		{
			#pragma omp critical 
			{
				cout << "\nThread #" << omp_get_thread_num() << " has counted sum of a row #" << i << ". It is " << accumulate(matrix[i].begin(), matrix[i].end(), 0);
			}
		}
	}
	cout << endl;
}


vector<vector<int>> generateMatrix(vector<vector<int>> matrix)
{
	srand(time(NULL));
	for (int i = 0; i < matrix.size(); i++)
		generate(matrix[i].begin(), matrix[i].end(), []() { return rand() % 30 + 1; });
	return matrix;
}


void printMatrix(vector<vector<int>> matrix)
{
	cout << "Matrix:";
	for (int i = 0; i < matrix.size(); i++)
	{
		cout << endl;
		copy(matrix[i].begin(), matrix[i].end(), ostream_iterator<int>(cout, " "));
	}
	cout << endl;
}