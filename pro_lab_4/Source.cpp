#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <vector>
using namespace std;


void task_lvl_1(MPI_Group world_group, int rank, int size);
void task_lvl_2(MPI_Group world_group, MPI_Comm comm_world, int size, int rank);


int main(int argc, char *argv[])
{
	int rank, size;
	MPI_Comm comm_world;
	MPI_Group world_group;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_dup(MPI_COMM_WORLD, &comm_world);//world group
	MPI_Comm_group(comm_world, &world_group);

	//lvl 1
	//N = 6, {0,1,2,3}, {0,3,4,5}. Знайти різницю груп. 
	task_lvl_1(world_group, rank, size);

	//lvl 2
	//N = 8, {1,2,4,5,6,7}. Вивести ранги парних процесів у створеній групі. Останній та передостанній процеси створеної групи
	//відсилають дані усім іншим процесам групи, парні процеси виводять отримані дані на екран
	task_lvl_2(world_group, comm_world, size, rank);

	MPI_Group_free(&world_group);
	MPI_Comm_free(&comm_world);
	MPI_Finalize();
	return 0;
}


void task_lvl_1(MPI_Group world_group, int rank, int size)
{
	if (size >= 6)
	{
		MPI_Group group1, group2, group_diff;
		int ranks1[] = { 0,1,2,3 }, ranks2[] = { 0,3,4,5 }, group_rank;

		MPI_Group_incl(world_group, 4, ranks1, &group1);//group1
		MPI_Group_incl(world_group, 4, ranks2, &group2);//group2
		MPI_Group_difference(group1, group2, &group_diff);//difference
		MPI_Group_rank(group_diff, &group_rank);
		if (rank == 0)
			cout << "\nTASK1: group1: 0, 1, 2, 3; group2: 0, 3, 4, 5; difference:";
		if (group_rank >= 0 && group_rank <= 6)
			cout << "\nTASK1: Rank " << rank << " is #" << group_rank << " in diff_group";

		MPI_Group_free(&group1);
		MPI_Group_free(&group2);
		MPI_Group_free(&group_diff);
	}
}


void task_lvl_2(MPI_Group world_group, MPI_Comm comm_world, int size, int rank)
{
	if (size >= 8)
	{
		int ranks3[] = { 0, 3 }, group_size, group_rank, mesg1, mesg2;
		MPI_Comm comm_lvl2;
		MPI_Group group_lvl2;

		//create
		MPI_Group_excl(world_group, 2, ranks3, &group_lvl2);
		MPI_Comm_create(comm_world, group_lvl2, &comm_lvl2);
		if (rank == 8) cout << "\nInfo about new group:";
		MPI_Group_size(group_lvl2, &group_size);
		MPI_Group_rank(group_lvl2, &group_rank);


		if (group_rank >= 0)//!= MPI_COMM_NULL)
		{
			//info
			cout << "\nRank " << rank << " is #" << group_rank << " in the group";
			if (group_rank % 2 == 0) cout << "\nRank " << rank << " (#" << group_rank << " in the group) has an even number in the group";

			//init
			if (group_rank == group_size - 1)
			{
				mesg1 = 555;
				cout << "\nRank " << rank << " (#" << group_rank << " in the group) sends message: " << mesg1;
			}
			if (group_rank == group_size - 2)
			{
				mesg2 = 777;
				cout << "\nRank " << rank << " (#" << group_rank << " in the group) sends message: " << mesg2;
			}

			//send & receive
			MPI_Bcast(&mesg1, 1, MPI_INT, group_size - 1, comm_lvl2);
			MPI_Bcast(&mesg2, 1, MPI_INT, group_size - 2, comm_lvl2);

			//print
			MPI_Group_rank(group_lvl2, &group_rank);
			if (group_rank % 2 == 0)
			{
				cout << "\nRank " << rank << " (#" << group_rank << " in the group) received a message:  " << mesg1;
				cout << "\nRank " << rank << " (#" << group_rank << " in the group) received a message: " << mesg2;
			}
		}

		MPI_Group_free(&group_lvl2);
	}
}