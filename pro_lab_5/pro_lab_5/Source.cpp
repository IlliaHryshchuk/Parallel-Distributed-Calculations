//#include "stdafx.h"
#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <chrono>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <omp.h>
using namespace std;


class MyTimer
{
public:
	MyTimer() {}
	~MyTimer() {}
	void startTimer()
	{
		start = std::chrono::high_resolution_clock::now();
	}
	double stopTimer()
	{
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;
		double result = duration.count();
		return result;
	}
private:
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;
};


void task_1();
void task_2();


int main()
{
	srand(time(NULL));
	
	task_1();
	task_2();

	system("pause");
	return 0;
}



void task_1()
{
	constexpr int m = 8, n = 11;
	vector<vector<int>> matrix(m, vector<int>(n));
	vector<int> paralelleResult(m), monoThreadedResult(m);
	int numOfNegatives = 0;
	string text;
	MyTimer timer;

	for (int i = 0; i < m; i++)
		generate(matrix[i].begin(), matrix[i].end(), []() { return rand() % 100 - 50; });


	/////////////////////////////////////////////////////////////////////////////////////////////PARALLEL SECTION
	#pragma omp parallel num_threads(m) shared(matrix, paralelleResult) private(numOfNegatives, text)
	{
		#pragma omp master 
		{
			text = "\nThread " + to_string(omp_get_thread_num()) + "(master): Hryshchuk Illia, var-4\n";
			cout << text.data();
		}
	}

	timer.startTimer();
	#pragma omp parallel num_threads(m) shared(matrix, paralelleResult) private(numOfNegatives, text)	
	{
		//#pragma omp for schedule(static, 6)
		#pragma omp for schedule(dynamic, 4)
		for (int i = 0; i < m; i++)
		{
			numOfNegatives = count_if(matrix[i].begin(), matrix[i].end(), [](int a) {return a < 0; });
			paralelleResult[i] = numOfNegatives;

			text = "\nThread: " + to_string(omp_get_thread_num()) + " has worked with row #" + to_string(i) + ". Result:" + to_string(paralelleResult[i]);
			cout << text.data();
		}
	}
	cout << "\nParallel time: " << timer.stopTimer();



	///////////////////////////////////////////////////////////////////////////////////////////////////NOT PARALLEL SECTION
	timer.startTimer();
	cout << "\n\nMain thread: result: ";
	for (int i = 0; i < m; i++)
	{
		numOfNegatives = count_if(matrix[i].begin(), matrix[i].end(), [](int a) {return a < 0; });
		monoThreadedResult[i] = numOfNegatives;
		cout << monoThreadedResult[i] << " ";
	}
	cout << "\nMono-thread time: " << timer.stopTimer() << endl;
}




void task_2()
{
	#pragma omp parallel
	{
		#pragma omp sections
		{
			#pragma omp section
			{
				constexpr int sizeOfArr = 99;
				vector<int> arr(sizeOfArr);

				generate(arr.begin(), arr.end(), []() { return rand() % 100 - 50; });
				cout << "\nArray: ";
				copy(arr.begin(), arr.end(), ostream_iterator<int>(cout, " "));
				cout << "\nArithmetic mean = " << accumulate(arr.begin(), arr.end(), 0.0) / (float)arr.size();
			}
			#pragma omp section
			{
				constexpr int sizeOfArr = 5;
				vector<vector<float>> matrix(sizeOfArr, vector<float>(sizeOfArr));
				float sumUnderDiag = 0, sumOverDiag = 0;


				for (int i = 0; i < sizeOfArr; i++)
					generate(matrix[i].begin(), matrix[i].end(), []() { return (rand() % 1000 - 500)*0.01; });

				cout << "\n\nMembers of matrix:";
				for (int i = 0; i < sizeOfArr; i++)
				{
					cout << endl;
					copy(matrix[i].begin(), matrix[i].end(), ostream_iterator<float>(cout, " "));
				}


				for (int i = 0; i < sizeOfArr; i++)
					for (int j = 0; j < sizeOfArr; j++)
					{
						if (i < j)
							if (matrix[i][j] < 0)
								sumOverDiag += matrix[i][j];
					}
				for (int i = 0; i < sizeOfArr; i++)
					for (int j = 0; j < sizeOfArr; j++)
					{
						if (i > j)
							if (matrix[i][j] < 0)
								sumUnderDiag += matrix[i][j];
					}
				cout << "\nSum of elements under main diag = " << sumUnderDiag << "\nSum of elements over main diag = " << sumOverDiag << endl;
			}
		}
	}
}