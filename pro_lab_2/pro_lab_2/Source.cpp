#include <mpi.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <string.h>
using namespace std;


void send_messages_via_synchronous(int rank, int size);
void receive_messages_via_synchronous(int &message, MPI_Status &status);
void send_message_via_buffer(char* strbuffer, int &bufsize, int destination, const char* message);
char* receive_message_via_buffer(char* strbuffer, int bufsize, int arrivedFrom, MPI_Status &status);


int main(int argc, char **argv)
{
	char symbolZ = 'Z';
	int rank, size, TAG = 0, message, bufsize = sizeof("it is tmp text"), numOfSymbol = (int)(symbolZ);
	char *strbuffer = NULL, *received_messageB, symbolK = 'K', symbolTmp = (char)(numOfSymbol);
	MPI_Status status;

	srand(time(NULL));
	status.count = 0;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (size >= 4)
	{
		if (rank == 0)
		{
			//1 - зменшення символу Z до межі К при кільцевій передачі (стандартний)
			cout << "\nFrom rank #" << rank << ":The symbol is: " << symbolTmp;
			MPI_Send(&numOfSymbol, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			while (numOfSymbol > (int)(symbolK))
			{
				MPI_Recv(&numOfSymbol, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
				symbolTmp = (char)(numOfSymbol);
				cout << "\nFrom rank #" << rank << ":The symbol is decreased: " << symbolTmp;
				numOfSymbol--;
				MPI_Send(&numOfSymbol, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			}
			


			//2
			send_message_via_buffer(strbuffer, bufsize, 1, "in the mind to suffer");
			cout << "\nFrom rank #" << rank << ": the message was sent via buffer.";



			//3
			send_messages_via_synchronous(rank, size);
		}
		else if (rank == 1)
		{
			//1
			while (numOfSymbol >= (int)(symbolK))
			{
				MPI_Recv(&numOfSymbol, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
				symbolTmp = (char)(numOfSymbol);
				cout << "\nFrom rank #" << rank << ":The symbol is decreased: " << symbolTmp;
				numOfSymbol--;
				MPI_Send(&numOfSymbol, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}



			//2
			received_messageB = receive_message_via_buffer(strbuffer, bufsize, 0, status);
			cout << "\nFrom rank #" << rank << ": the received message via buffer is: " << received_messageB;
			send_message_via_buffer(strbuffer, bufsize, 2, received_messageB);
			cout << "\nFrom rank #" << rank << ": the message was sent via buffer.";



			//3
			receive_messages_via_synchronous(message, status);
		}
		else if (rank == 2)
		{
			//2
			received_messageB = receive_message_via_buffer(strbuffer, bufsize, 1, status);
			cout << "\nFrom rank #" << rank << ": the received message via buffer is: " << received_messageB;
			send_message_via_buffer(strbuffer, bufsize, 3, received_messageB);
			cout << "\nFrom rank #" << rank << ": the message was sent via buffer.";

			

			//3
			receive_messages_via_synchronous(message, status);
			cout << "\nFrom rank #" << rank << ": the received message via synchronous is: " << message;
		}
		else if (rank == 3)
		{
			//2
			received_messageB = receive_message_via_buffer(strbuffer, bufsize, 2, status);
			cout << "\nFrom rank #" << rank << ": the received message via buffer is: " << received_messageB;

			

			//3
			receive_messages_via_synchronous(message, status);
		}
		else
		{
			//3
			receive_messages_via_synchronous(message, status);
			if(rank % 2 == 0)
				cout << "\nFrom rank #" << rank << ": the received message via synchronous is: " << message;
		}
	}
	else
	{
		cout << "\nNot enough processes!\n";
	}


	MPI_Finalize();
	return 0;
}



void send_messages_via_synchronous(int rank, int size)
{
	int message;
	cout << "\nFrom rank #" << rank << ": Enter an int number: ";
	cin >> message;
	message = pow(message, 4);
	for (int i = 1; i < size; i++){
		MPI_Ssend(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
	cout << "\nFrom rank #" << rank << ": All messages via synchronous were sent!";
}


void receive_messages_via_synchronous(int &message, MPI_Status &status)
{
	MPI_Recv(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
}


void send_message_via_buffer(char* strbuffer, int &bufsize, int destination, const char* message)
{
	bufsize = sizeof(message);
	strbuffer = (char*)malloc(bufsize + MPI_BSEND_OVERHEAD);
	MPI_Buffer_attach(strbuffer, bufsize + MPI_BSEND_OVERHEAD);
	strbuffer = (char*)message;
	MPI_Bsend(&strbuffer, bufsize, MPI_CHAR, destination, 0, MPI_COMM_WORLD);
	MPI_Buffer_detach(&strbuffer, &bufsize);
	free(strbuffer);
	return;
}


char* receive_message_via_buffer(char* strbuffer, int bufsize, int arrivedFrom, MPI_Status &status)
{
	MPI_Recv(&strbuffer, bufsize, MPI_CHAR, arrivedFrom, 0, MPI_COMM_WORLD, &status);
	strbuffer;
	return strbuffer;
}