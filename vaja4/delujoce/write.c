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
    int fdOpen, msg, BufferSize;
    char *buffer;
    int SemId, ShmId;
    struct sembuf Semaphore;
    char *ShmRead;

    BufferSize = 2;
    buffer = (char *)malloc(BufferSize);

    if ((fdOpen = open("/home/martin/Desktop/vgradni/vaja4/cifre_write.txt", O_WRONLY)) == -1) {
        printf("open err\n");
        exit(6);
    }
    printf("%d\n", fdOpen);

    if ((ShmId = shmget(SHM_KEY, BufferSize, IPC_CREAT | 0644)) == -1) {
        printf("shmget err\n");
        exit(1);
    }

    if ((SemId = semget(SEM_KEY_NO, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
    }

    if ((ShmRead = shmat(ShmId, NULL, 0)) == (void *)-1)   {
        printf("shmat err\n");
        exit(1);
    }

    // do {
    //     //read lock
    //     Semaphore.sem_num = SEM_READ;
    //     Semaphore.sem_op = -1;
    //     Semaphore.sem_flg = 0;
    //     semop(SemId, &Semaphore, 1);

    //     printf("%s\n", ShmRead);
    //     write(fdOpen, ShmRead, ShmSize);

    //     //unlock write
    //     Semaphore.sem_num = SEM_WRITE;
    //     Semaphore.sem_op = 1;
    //     Semaphore.sem_flg = 0;
    //     semop(SemId, &Semaphore, 1);

    // } while (1);

    while(1) {
        //read lock
        Semaphore.sem_num = SEM_READ;
        Semaphore.sem_op = -1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);

        printf("%s\n", ShmRead);
        write(fdOpen, ShmRead, BufferSize);

        //unlock write
        Semaphore.sem_num = SEM_WRITE;
        Semaphore.sem_op = 1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);
    }

    if (semctl(SemId, 0, IPC_RMID, 0) == -1)    {
        printf("semctl err\n");
        exit(6);
    }

    if (shmctl(ShmId, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    return 0;
}