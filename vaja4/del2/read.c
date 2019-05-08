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

int main(void) {
    int fdOpen, msg, bufferSize;
    int *buffer;
    int semId1, shmId1;
    unsigned short semArray1[2];
    struct sembuf semaphore1;
    int *shmWrite1;
    int ctr = 1;

    bufferSize = 640*480*3;
    buffer = (int *)malloc(bufferSize);

    if ((fdOpen = open("/dev/video0", O_RDONLY)) == -1) {
        printf("open err\n");
        exit(6);
    }
    printf("%d\n", fdOpen);

    if ((semId1 = semget(SEM_KEY_NO1, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
        if (semctl(semId1, 0, IPC_RMID, 0) == -1)    {//odstrani se množica semaforjev
            printf("semctl err\n");
            exit(1);
        }
    }

    //dodelitev del. pom.
    if ((shmId1 = shmget(SHM_KEY1, bufferSize, IPC_CREAT | 0644)) == -1) { //shmsize more bit velikosti sporocila (bufferSize)
        printf("shmget err\n");
        exit(1);
    }

    //pripni del. pom.
    if ((shmWrite1 = (int *)shmat(shmId1, NULL, 0)) == (int *) -1)   {
        printf("shmat err\n");
        exit(1);
    }

    //nastavitev semaforjev
    semArray1[SEM_READ1] = 0;
    semArray1[SEM_WRITE1] = 1;

    if (semctl(semId1, 0, SETALL, semArray1) == -1)   {
        printf("semctl/sem set err\n");
        exit(2);
    }

    while(read(fdOpen, buffer, bufferSize) > 0) {
        //write lock
        semaphore1.sem_num = SEM_WRITE1;
        semaphore1.sem_op = -1;
        semaphore1.sem_flg = 0;
        semop(semId1, &semaphore1, 1);

        *shmWrite1 = *buffer;
        //printf("%d\n", *buffer);

        //unlock read
        semaphore1.sem_num = SEM_READ1;
        semaphore1.sem_op = 1;
        semaphore1.sem_flg = 0;
        semop(semId1, &semaphore1, 1);

        printf("%d\n", ctr);
        ctr++;
    }

    if (shmctl(shmId1, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    return 0;
}