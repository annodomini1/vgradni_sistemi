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

#define SEM_KEY ((key_t)ftok("./SemKey",’k’))
#define SEM_KEY_NO (key_t)1234
#define SEM_READ 0
#define SEM_WRITE 1
#define SHM_KEY (key_t)ftok(".",'k')

int main(void)
{
    int fdOpen, msg, bufferSize;
    char *buffer;
    int semId, shmId;
    struct sembuf semaphore;
    int *shmRead;
    int ctr = 1;
    int msgSize = 10;

    bufferSize = 1;
    buffer = (char *)malloc(bufferSize);

    if ((fdOpen = creat("/home/martin/Desktop/vgradni/vaja4/cifre_write.txt", 0644)) == -1)   {
        printf("creat err\n");
        exit(1);
    }

    if ((shmId = shmget(SHM_KEY, bufferSize, IPC_CREAT | 0644)) == -1) {
        printf("shmget err\n");
        exit(1);
    }

    if ((semId = semget(SEM_KEY_NO, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
    }

    if ((shmRead = (int *)shmat(shmId, NULL, 0)) == (int *)-1)   {
        printf("shmat err\n");
        exit(1);
    }

    while(1) {
        //read lock
        semaphore.sem_num = SEM_READ;
        semaphore.sem_op = -1;
        semaphore.sem_flg = 0;
        semop(semId, &semaphore, 1);

        printf("%d\n", *shmRead);
        write(fdOpen, shmRead, bufferSize);

        //unlock write
        semaphore.sem_num = SEM_WRITE;
        semaphore.sem_op = 1;
        semaphore.sem_flg = 0;
        semop(semId, &semaphore, 1);

        ctr++;

        if (ctr > msgSize)   {
            break;
        }
    }

    if (semctl(semId, 0, IPC_RMID, 0) == -1)    {
        printf("semctl err\n");
        exit(6);
    }

    if (shmctl(shmId, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    return 0;
}