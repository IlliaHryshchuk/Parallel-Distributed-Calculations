#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

vector<vector<int>> fillVector(vector<vector<int>> vec);
void actionForLeaderOfGroup(int numOfRowsToEachProc, int TAG, int size, int groupSize, MPI_Comm comm_world, MPI_Comm comm_group);
void actionForClients(int numOfRowsToEachProc, int TAG, MPI_Comm comm_group);

//Створити МРІ програму, яка може бути запущена з кількістю процесів N, 2 < N < 10. Перший процес робите сервером.
//Всі інші розбиваєте на дві групи 'клієнтів'. Сервер формує дві матриці 20х20 з рандомними числами.Завдання придумати 
//найоптимальніший спосіб підрахунку суми елементів матриці клієнтами.Тобто кожна група клієнтських процесів якимось 
//способом рахує суму всіх елементів матриці і передає потім результат серверу.
int main(int argc, char *argv[])
{
	int rank, size, rank_server, rank_group1, rank_group2, result1, result2, TAG1 = 1, TAG2 = 2, groupSize, numOfRowsToEachProc;
	vector<int> ranks1, ranks2, ranksServer;
	vector<vector<int>> matrix1(20, vector<int>(20)), matrix2(20, vector<int>(20));

	MPI_Comm comm_world, comm_server, comm_group1, comm_group2, comm_server_group1, comm_server_group2, inter_comm;
	MPI_Group world_group, server, group1, group2;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_dup(MPI_COMM_WORLD, &comm_world);//world group
	MPI_Comm_group(comm_world, &world_group);


	if (size > 2 && size < 10)
	{
		//find numbers of processes for groups
		ranksServer.push_back(0);
		for (int i = 1; i <= size / 2; i++)
			ranks1.push_back(i);
		for (int i = (size / 2) + 1; i < size; i++)
			ranks2.push_back(i);

		//create groups
		MPI_Group_incl(world_group, ranksServer.size(), ranksServer.data(), &server);
		MPI_Group_incl(world_group, ranks1.size(), ranks1.data(), &group1);
		MPI_Group_incl(world_group, ranks2.size(), ranks2.data(), &group2);

		MPI_Comm_create(comm_world, server, &comm_server);
		MPI_Comm_create(comm_world, group1, &comm_group1);
		MPI_Comm_create(comm_world, group2, &comm_group2);

		MPI_Group_rank(server, &rank_server);
		MPI_Group_rank(group1, &rank_group1);
		MPI_Group_rank(group2, &rank_group2);


		if (rank_server >= 0)
		{
			cout << "Server: server rank: " << rank_server << " general rank: " << rank;
			//create vectors
			matrix1 = fillVector(matrix1);
			matrix2 = fillVector(matrix2);

			//send vectors
			for (int i = 0; i < 20; i++) {
				MPI_Send(matrix1[i].data(), 20, MPI_INT, 1, TAG1, comm_world);
				MPI_Send(matrix2[i].data(), 20, MPI_INT, (size / 2) + 1, TAG2, comm_world);
			}

			//receive results
			MPI_Recv(&result1, 1, MPI_INT, 1, TAG1, comm_world, &status);
			MPI_Recv(&result2, 1, MPI_INT, (size / 2) + 1, TAG2, comm_world, &status);
			cout << "\nServer: the result1 is: " << result1 << "\nServer: the result2 is: " << result2;
		}
		if (rank_group1 >= 0)
		{
			cout << "Group1: rank: " << rank_group1 << " general rank: " << rank;

			MPI_Group_size(group2, &groupSize);
			numOfRowsToEachProc = 20 / groupSize;

			if (rank_group1 == 0)
			{
				actionForLeaderOfGroup(numOfRowsToEachProc, TAG1, size, groupSize, comm_world, comm_group1);
			}
			else
			{
				actionForClients(numOfRowsToEachProc, TAG1, comm_group1);
			}
		}
		if (rank_group2 >= 0)
		{
			cout << "Group2: rank: " << rank_group2 << " general rank: " << rank;

			MPI_Group_size(group2, &groupSize);
			numOfRowsToEachProc = 20 / groupSize;

			if (rank_group2 == 0)
			{
				actionForLeaderOfGroup(numOfRowsToEachProc, TAG2, size, groupSize, comm_world, comm_group2);
			}
			else
			{
				actionForClients(numOfRowsToEachProc, TAG2, comm_group2);
			}
		}

		MPI_Group_free(&server);
		MPI_Group_free(&group1);
		MPI_Group_free(&group2);
	}


	MPI_Group_free(&world_group);
	MPI_Comm_free(&comm_world);
	MPI_Finalize();
	return 0;
}


vector<vector<int>> fillVector(vector<vector<int>> vec)
{
	srand(time(NULL));
	for (int i = 0; i < vec.size(); i++)
		for (int j = 0; j < vec.size(); j++)
			vec[i][j] = rand() % 1000 + 1;

	return vec;
}


void actionForLeaderOfGroup(int numOfRowsToEachProc, int TAG, int size, int groupSize, MPI_Comm comm_world, MPI_Comm comm_group)
{
	vector<vector<int>> matrix(20, vector<int>(20));
	MPI_Status status;
	int numOfProc = 0, sum = 0, buf;

	//receive from server
	for (int i = 0; i < 20; i++)
		MPI_Recv(matrix[i].data(), 20, MPI_INT, 0, TAG, comm_world, &status);

	//spread in group
	numOfProc = 0;
	for (int i = numOfRowsToEachProc; i < numOfRowsToEachProc*groupSize; i++) {
		if (i % numOfRowsToEachProc == 0) numOfProc++;
		MPI_Send(matrix[i].data(), 20, MPI_INT, numOfProc, TAG, comm_group);
	}

	//count my sum
	//count for 0 proc
	for (int i = 0; i < numOfRowsToEachProc; i++)
		sum += accumulate(matrix[i].begin(), matrix[i].end(), 0);

	//count sum in extra rows
	for (int i = numOfRowsToEachProc * size; i < 20; i++)
		sum += accumulate(matrix[i].begin(), matrix[i].end(), 0);

	//receive
	numOfProc = 0;
	for (int i = numOfRowsToEachProc; i < numOfRowsToEachProc*groupSize; i++) {
		if (i % numOfRowsToEachProc == 0)numOfProc++;
		MPI_Recv(&buf, 1, MPI_INT, numOfProc, 1, comm_group, &status);

		sum += buf;
	}

	////send to server
	MPI_Send(&sum, 1, MPI_INT, 0, TAG, comm_world);
}


void actionForClients(int numOfRowsToEachProc, int TAG, MPI_Comm comm_group)
{
	vector<int> vec_buf(20);
	MPI_Status status;
	int sum;

	for (int i = 0; i < numOfRowsToEachProc; i++) {
		sum = 0;
		MPI_Recv(vec_buf.data(), 20, MPI_INT, 0, TAG, comm_group, &status);
		sum = accumulate(vec_buf.begin(), vec_buf.end(), 0);
		MPI_Send(&sum, 1, MPI_INT, 0, 1, comm_group);
	}
}