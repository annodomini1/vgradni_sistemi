#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>
#include <sys/shm.h>

#define SEM_READ2 0
#define SEM_WRITE2 1
#define SHM_KEY2 (key_t)ftok("./ShmKey2",'k')
#define SEM_KEY_NO2 (key_t)ftok("./SemKey2",'k')

int main(void)
{
    int fdOpen, msg, bufferSize;
    char *buffer;
    int semId2, shmId2;
    struct sembuf semaphore2;
    int *shmRead2;
    int ctr = 1;
    int msgSize = 10;

    bufferSize = 1;
    buffer = (char *)malloc(bufferSize);

    if ((fdOpen = creat("/home/martin/Desktop/vgradni/vaja4/del2/cifreOut.txt", 0644)) == -1)   {
        printf("creat err\n");
        exit(1);
    }

    if ((shmId2 = shmget(SHM_KEY2, bufferSize, IPC_CREAT | 0644)) == -1) {
        printf("shmget err\n");
        exit(1);
    }

    if ((semId2 = semget(SEM_KEY_NO2, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
    }

    if ((shmRead2 = (int *)shmat(shmId2, NULL, 0)) == (int *)-1)   {
        printf("shmat err\n");
        exit(1);
    }

    while(1) {
        //read lock
        semaphore2.sem_num = SEM_READ2;
        semaphore2.sem_op = -1;
        semaphore2.sem_flg = 0;
        semop(semId2, &semaphore2, 1);

        printf("%d\n", *shmRead2);
        write(fdOpen, shmRead2, bufferSize);

        //unlock write
        semaphore2.sem_num = SEM_WRITE2;
        semaphore2.sem_op = 1;
        semaphore2.sem_flg = 0;
        semop(semId2, &semaphore2, 1);

        // ctr++;

        // if (ctr > msgSize)   {
        //     break;
        // }
    }

    if (semctl(semId2, 0, IPC_RMID, 0) == -1)    {
        printf("semctl err\n");
        exit(6);
    }

    if (shmctl(shmId2, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    return 0;
}