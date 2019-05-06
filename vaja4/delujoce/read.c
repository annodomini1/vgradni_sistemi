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
    int fdOpen, msg, BufferSize;
    char *buffer;
    int SemId, ShmId;
    unsigned short SemArray[2];
    struct sembuf Semaphore;
    char *ShmWrite;

    BufferSize = 2;
    buffer = (char *)malloc(BufferSize);

    if ((fdOpen = open("/home/martin/Desktop/vgradni/vaja4/cifre.txt", O_RDONLY)) == -1) {
        printf("open err\n");
        exit(6);
    }
    printf("%d\n", fdOpen);

    // if ((to_file = creat("/home/martin/Desktop/vgradni/vaja4/cifre_write.txt", 0644)) == -1) {
    //         printf("creat err\n");
    //         exit(4);
    //     }

    if ((SemId = semget(SEM_KEY_NO, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
        if (semctl(SemId, 0, IPC_RMID, 0) == -1)    {//odstrani se množica semaforjev
            printf("semctl err\n");
            exit(1);
        }
    }

    //dodelitev del. pom.
    if ((ShmId = shmget(SHM_KEY, BufferSize, IPC_CREAT | 0644)) == -1) { //shmsize more bit velikosti sporocila (BufferSize)
        printf("shmget err\n");
        exit(1);
    }

    //pripni del. pom.
    if ((ShmWrite = shmat(ShmId, NULL, 0)) == (void *) -1)   {
        printf("shmat err\n");
        exit(1);
    }

    //nastavitev semaforjev
    SemArray[SEM_READ] = 0;
    SemArray[SEM_WRITE] = 1;

    if (semctl(SemId, 0, SETALL, SemArray) == -1)   {
        printf("semctl/sem set err\n");
        exit(2);
    }

    // do {
    //     //write lock
    //     Semaphore.sem_num = SEM_WRITE;
    //     Semaphore.sem_op = -1;
    //     Semaphore.sem_flg = 0;
    //     semop(SemId, &Semaphore, 1);

    //     if ((msg = read(fdOpen, buffer, BufferSize)) == -1) {
    //         printf("read err\n");
    //         exit(3);
    //     }

    //     strcpy(ShmWrite, buffer);

    //     //unlock read
    //     Semaphore.sem_num = SEM_READ;
    //     Semaphore.sem_op = 1;
    //     Semaphore.sem_flg = 0;
    //     semop(SemId, &Semaphore, 1);

    // } while (msg > 0);

    while(read(fdOpen, buffer, BufferSize) > 0) {
        //write lock
        Semaphore.sem_num = SEM_WRITE;
        Semaphore.sem_op = -1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);

        strcpy(ShmWrite, buffer);//ali se da brez pretvorbe v string???

        //unlock read
        Semaphore.sem_num = SEM_READ;
        Semaphore.sem_op = 1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);
    }

    if (shmctl(ShmId, IPC_RMID, 0) == -1)   {
        printf("shmctl err\n");
        exit(1);
    }
    return 0;
}