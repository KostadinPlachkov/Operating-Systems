#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

struct threadInfo
{
    // Information about the thread
	HANDLE handle;
	DWORD id;
} *threadArray;

DWORD waiter;

DWORD WINAPI Cycle(LPVOID arg)
{
    // Function, executed by the thread
	int i;
	printf("Start of thread: %d\n", arg); // Message for the start of the thread
	for( i = 0; i < 10; i++);
	printf("End of thread: %d\n", arg); // Message for the end of the thread
	return (DWORD) arg;
}


int main(int argc, char* argv[])
{
	int i, threadNumber;
	HANDLE handleThreads[100];

	if ( argc != 2) // Checks for correct values
	{
		printf("Incorrect params!");
		return 1;
	}

	threadNumber = atoi(argv[1]);
	if ( threadNumber < 1 || threadNumber > 100) {  // Checks if the number is between 1-100
		printf("Bad argument value!");
		return 1;
	}

	threadArray = (struct threadInfo *) malloc(sizeof(threadInfo)*threadNumber); // Memory allocation
	if ( threadArray == NULL ) {
        // Checks for memory allocation error
		printf("Bad memory allocation!");
		return 1;
	}

	for(i = 0; i < threadNumber; i++)
	{
		 threadArray[i].handle = CreateThread(NULL, 0, Cycle, (LPVOID)i, 0, &threadArray[i].id); // Creating a thread
		 handleThreads[i] = threadArray[i].handle;
	}

	waiter = WaitForMultipleObjects(threadNumber, handleThreads, TRUE, INFINITE);

	for(i = 0; i < threadNumber; i++)
        // Loop that closes the threads
		CloseHandle(threadArray[i].handle); // Closing the thread

	return 0;
}
