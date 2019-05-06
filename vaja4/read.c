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
#define SHM_KEY (key_t)ftok(".",'k') //obstojec direktorij

int main(void) //(help: https://stackoverflow.com/questions/21227270/read-write-integer-array-into-shared-memory)
{
    int fdOpen, msg, bufferSize;
    int *buffer;
    int semId, shmId;
    unsigned short semArray[2];
    struct sembuf semaphore;
    int *shmWrite;

    bufferSize = 1;
    buffer = (int *)malloc(bufferSize);

    if ((fdOpen = open("/home/martin/Desktop/vgradni/vaja4/cifre.txt", O_RDONLY)) == -1) {
        printf("open err\n");
        exit(6);
    }
    printf("%d\n", fdOpen);

    if ((semId = semget(SEM_KEY_NO, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
        if (semctl(semId, 0, IPC_RMID, 0) == -1)    {//odstrani se množica semaforjev
            printf("semctl err\n");
            exit(1);
        }
    }

    //dodelitev del. pom.
    if ((shmId = shmget(SHM_KEY, bufferSize, IPC_CREAT | 0644)) == -1) { //shmsize more bit velikosti sporocila (bufferSize)
        printf("shmget err\n");
        exit(1);
    }

    //pripni del. pom.
    if ((shmWrite = (int *)shmat(shmId, NULL, 0)) == (int *) -1)   {
        printf("shmat err\n");
        exit(1);
    }

    //nastavitev semaforjev
    semArray[SEM_READ] = 0;
    semArray[SEM_WRITE] = 1;

    if (semctl(semId, 0, SETALL, semArray) == -1)   {
        printf("semctl/sem set err\n");
        exit(2);
    }

    while(read(fdOpen, buffer, bufferSize) > 0) {
        //write lock
        semaphore.sem_num = SEM_WRITE;
        semaphore.sem_op = -1;
        semaphore.sem_flg = 0;
        semop(semId, &semaphore, 1);

        *shmWrite = *buffer;
        printf("%d\n", *buffer);

        //unlock read
        semaphore.sem_num = SEM_READ;
        semaphore.sem_op = 1;
        semaphore.sem_flg = 0;
        semop(semId, &semaphore, 1);
    }

    if (shmctl(shmId, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    return 0;
}