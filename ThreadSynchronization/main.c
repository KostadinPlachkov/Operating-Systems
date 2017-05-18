#include <stdio.h>
#include <windows.h>
#include <stdlib.h>

struct threadinfo
{
    HANDLE handle;
    DWORD id;
    int terminate;
} *ThreadP, *ThreadC;

struct item
{
    int data, empty;
} *ress;

int RessNumber = 0;

HANDLE handleFULL, handleEMPTY;

HANDLE mutex;

DWORD WINAPI CycleP (LPVOID arg)
{
    int i, count = 0;
    DWORD waitSemaphore, waitMutex;
    while (!(ThreadP[(int)arg].terminate))
    {
        waitSemaphore = WaitForSingleObject(handleEMPTY, INFINITE); // Tries to down semaphore EMPTY
        if ( waitSemaphore == WAIT_OBJECT_0 )
        {
            waitMutex = WaitForSingleObject(mutex, INFINITE); // Tries to take mutex
            count = 0;
            for( i = 0; i < RessNumber; i++)
                if ( ress[i].empty == 1 )
                {
                    ress[i].data = 1;
                    ress[i].empty = 0;
                    printf("Producer thread %d produced one resource!", (int) arg);
                    break;
                }
            for( i = 0; i < RessNumber; i++)
                if ( ress[i].empty == 0)
                    count++;
            printf(" Resources available: %d\n", count);
            ReleaseMutex(mutex);  // Frees the mutex
            ReleaseSemaphore(handleFULL, 1, NULL); // Ups semaphore FULL
        }
    }
    ReleaseSemaphore(handleFULL, 1, NULL); // Ups semaphore FULL so that consumer may not finish before producer

    return (DWORD) arg;
}

DWORD WINAPI CycleC (LPVOID arg)
{

    int i, count = 0 ;
    DWORD waitSemaphore, waitMutex;
    while (!(ThreadC[(int)arg].terminate))
    {
        waitSemaphore = WaitForSingleObject(handleFULL, INFINITE); // Tries to down semaphore FULL
        if ( waitSemaphore == WAIT_OBJECT_0 )
        {
            waitMutex = WaitForSingleObject(mutex, INFINITE); //Tries to take mutex
            count = 0;
            for( i = 0; i < RessNumber; i++)
                if ( ress[i].empty == 0 )
                {
                    ress[i].data = 0;
                    ress[i].empty = 1;
                    printf("Consumer thread %d consumed one resource!", (int) arg);
                    break;
                }
            for( i = 0; i < RessNumber; i++)
                if ( ress[i].empty == 0)
                    count++;
            printf(" Resources available: %d\n", count);
            ReleaseMutex(mutex); // Frees the mutex
            ReleaseSemaphore(handleEMPTY, 1, NULL); // Ups semaphore EMPTY
        }
    }
    return (DWORD) arg;
}

int main(int argc, char* argv[])
{

    int i, ThreadNumberP, ThreadNumberC;
    HANDLE hthreadsP[100], hthreadsC[100];

    if ( argc != 4)
    {
        printf("Bad number of arguments!");
        return 1;
    }

    RessNumber = atoi(argv[1]);
    if ( RessNumber < 1 || RessNumber > 100)
    {
        printf("Bad argument value!");
        return 1;
    }

    ThreadNumberP = atoi(argv[2]);
    if ( ThreadNumberP < 1 || ThreadNumberP > 100)
    {
        printf("Bad argument value!");
        return 1;
    }

    ThreadNumberC = atoi(argv[3]);
    if ( ThreadNumberC < 1 || ThreadNumberC > 100)
    {
        printf("Bad argument value!");
        return 1;
    }

    ress = (struct item *)malloc(RessNumber*sizeof(*ress));
    if (ress == NULL)
    {
        printf ("Bad memory allocation for resource!\n");
        return 1;
    }

    for (i = 0; i < RessNumber; i++)
    {
        ress[i].empty = 1;
        ress[i].data = 0;
    }

    mutex = CreateMutex (NULL, FALSE, NULL);
    if ( mutex == NULL)
    {
        printf("Cannot create mutex!");
        return  1;
    }

    handleFULL = CreateSemaphore(NULL, 0, RessNumber, NULL);
    if ( handleFULL == NULL)
    {
        printf("Cannot create semaphore!");
        return  1;
    }

    handleEMPTY = CreateSemaphore(NULL, RessNumber, RessNumber, NULL);
    if ( handleEMPTY == NULL)
    {
        printf("Cannot create semaphore!");
        return  1;
    }

    ThreadP = (struct threadinfo *) malloc(sizeof(*ThreadP)*ThreadNumberP);
    if ( ThreadP == NULL )
    {
        printf("Bad memory allocation for producers!");
        return 1;
    }

    ThreadC = (struct threadinfo *) malloc(sizeof(*ThreadC)*ThreadNumberC);
    if ( ThreadC == NULL )
    {
        printf("Bad memory allocation for consumers!");
        return 1;
    }

    for(i = 0; i < ThreadNumberC; i++)
    {
        ThreadC[i].handle = CreateThread(NULL, 0, CycleC, (LPVOID)i, 0, &ThreadC[i].id);
        hthreadsC[i] = ThreadC[i].handle;
        ThreadC[i].terminate = 0;
    }

    for(i = 0; i < ThreadNumberP; i++)
    {

        ThreadP[i].handle = CreateThread(NULL, 0, CycleP, (LPVOID)i, 0, &ThreadP[i].id);
        hthreadsP[i] = ThreadP[i].handle;
        ThreadP[i].terminate = 0;
    }

    for( i = 0; i < 40000; i++); // Empty cycle to give the threads some time

    for(i = 0; i < ThreadNumberP; i++)
        ThreadP[i].terminate = 1;
    for(i = 0; i < ThreadNumberC; i++)
        ThreadC[i].terminate = 1;

    WaitForMultipleObjects( ThreadNumberP, hthreadsP, TRUE, INFINITE);
    WaitForMultipleObjects( ThreadNumberC, hthreadsC, TRUE, INFINITE);

    // Closing the handles and freeing the memory

    for(i = 0; i < ThreadNumberP; i++)
        CloseHandle(ThreadP[i].handle);
    for(i = 0; i < ThreadNumberC; i++)
        CloseHandle(ThreadC[i].handle);

    CloseHandle(mutex);
    CloseHandle(handleEMPTY);
    CloseHandle(handleFULL);

    free(ress);
    free(ThreadP);
    free(ThreadC);

    return 0;
}
