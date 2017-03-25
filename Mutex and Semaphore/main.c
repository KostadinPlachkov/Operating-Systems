#include<stdio.h>
#include<windows.h>
#define MAX_SEM_COUNT

struct threadInfo{
		HANDLE handle;
		DWORD id;
};

double global=100.8975;
HANDLE mutex, semaphore;

DWORD WINAPI runThread(LPVOID threadLife){
	double local;
	int count=0, i;
	int value = (int *) threadLife;
	for(i = 0; i < 1000; i++) {
		WaitForSingleObject(semaphore, INFINITE);
		local = global;
		local += value;
		local += value;
		Sleep(1);
		global += value;
		global += value;
		if(global == local)
			count++;
		ReleaseSemaphore(semaphore, 1, NULL);
	}
	printf("Thread %d was interfered %d times\n", value, count);
	return 0;
}
int main(int argc, char* argv[])
{
	int threadNum, i;
	struct threadInfo *threadArray;
	HANDLE *threadHandle;
	//mutex=CreateMutex(NULL,FALSE,NULL);
	semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	if(argc != 2)
		return -1;
	threadNum = atoi(argv[1]);
	threadArray = (struct threadInfo *) malloc(sizeof(struct threadInfo)*threadNum);
	threadHandle = (HANDLE*) malloc(sizeof(HANDLE)*threadNum);
	for(i = 0; i < threadNum; i++){
		threadArray[i].handle = CreateThread(NULL, 0, runThread, &i, 0, &threadArray[i].id);
		threadHandle[i] = threadArray[i].handle;
        Sleep(10);
	}
	WaitForMultipleObjects(threadNum, threadHandle, 1, INFINITE);
	for(i = 0; i < threadNum; i++)
		CloseHandle(threadHandle[i]);
	getchar();
	Sleep(2000);
	return 0;
}
