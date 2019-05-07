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

#define SEM_READ1 0
#define SEM_WRITE1 1
#define SHM_KEY1 (key_t)ftok(".",'k') //obstojec direktorij
#define SEM_KEY_NO1 (key_t)ftok(".",'k')
#define SEM_READ2 0
#define SEM_WRITE2 1
#define SHM_KEY2 (key_t)ftok("./ShmKey2",'k') //obstojec direktorij
#define SEM_KEY_NO2 (key_t)ftok("./SemKey2",'k')

int main(void) 
{
    int fdOpen, msg, bufferSize;
    int *buffer;
    int semId1, shmId1, semId2, shmId2;
    unsigned short semArray2[2];
    struct sembuf semaphore1;
    struct sembuf semaphore2;
    int *shmWrite2;
    int *shmRead1;

    bufferSize = 1;
    buffer = (int *)malloc(bufferSize);

    //semaforji
    if ((semId1 = semget(SEM_KEY_NO1, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
        if (semctl(semId1, 0, IPC_RMID, 0) == -1)    {//odstrani se množica semaforjev
            printf("semctl err\n");
            exit(1);
        }
    }
    if ((semId2 = semget(SEM_KEY_NO2, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
        if (semctl(semId2, 0, IPC_RMID, 0) == -1)    {//odstrani se množica semaforjev
            printf("semctl err\n");
            exit(1);
        }
    }

    //dodelitev del. pom.
    if ((shmId1 = shmget(SHM_KEY1, bufferSize, IPC_CREAT | 0644)) == -1) { //shmsize more bit velikosti sporocila (bufferSize)
        printf("shmget err\n");
        exit(1);
    }
    if ((shmId2 = shmget(SHM_KEY2, bufferSize, IPC_CREAT | 0644)) == -1) { //shmsize more bit velikosti sporocila (bufferSize)
        printf("shmget err\n");
        exit(1);
    }

    //pripni del. pom.
    if ((shmWrite2 = (int *)shmat(shmId2, NULL, 0)) == (int *) -1)   {
        printf("shmat err\n");
        exit(1);
    }
    if ((shmRead1 = (int *)shmat(shmId1, NULL, 0)) == (int *)-1)   {
        printf("shmat err\n");
        exit(1);
    }

    //nastavitev semaforjev
    semArray2[SEM_READ2] = 0;
    semArray2[SEM_WRITE2] = 1;

    if (semctl(semId2, 0, SETALL, semArray2) == -1)   {
        printf("semctl/sem set err\n");
        exit(2);
    }

    while(1) {
        //write lock
        semaphore1.sem_num = SEM_READ1;
        semaphore1.sem_op = -1;
        semaphore1.sem_flg = 0;
        semop(semId1, &semaphore1, 1);

        //write lock
        semaphore2.sem_num = SEM_WRITE2;
        semaphore2.sem_op = -1;
        semaphore2.sem_flg = 0;
        semop(semId2, &semaphore2, 1);

        *shmWrite2 = *shmRead1 + 1;

        //unlock read
        semaphore1.sem_num = SEM_WRITE1;
        semaphore1.sem_op = 1;
        semaphore1.sem_flg = 0;
        semop(semId1, &semaphore1, 1);

        //unlock read
        semaphore2.sem_num = SEM_READ2;
        semaphore2.sem_op = 1;
        semaphore2.sem_flg = 0;
        semop(semId2, &semaphore2, 1);
    }

    if (shmctl(shmId1, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    if (shmctl(shmId2, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    return 0;
}