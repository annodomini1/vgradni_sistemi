#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <stdio.h>

#define SEM_KEY ((key_t)ftok("./SemKey",’k’))
#define SEM_KEY_NO (key_t)1234
#define SEM_READ 0
#define SEM_WRITE 1

int main(void) //ta program bere in printa
{
    int fdOpen, msg, buffer_size;
    char *buffer;
    int SemId;
    struct sembuf Semaphore;

    buffer_size = 1;
    buffer = (char *)malloc(buffer_size);

    if ((fdOpen = open("/home/martin/Desktop/vgradni/vaja4/cifre_write.txt", O_RDONLY)) == -1) {
        printf("open err\n");
        exit(6);
    }
    printf("%d\n", fdOpen);


    if ((SemId = semget(SEM_KEY_NO, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
    }

    do {
        //read lock
        Semaphore.sem_num = SEM_READ;
        Semaphore.sem_op = -1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);

        if ((msg = read(fdOpen, buffer, buffer_size)) == -1) {
            printf("read err\n");
            exit(3);
        }
        
        printf("%d\n", msg);

        //unlock write
        Semaphore.sem_num = SEM_WRITE;
        Semaphore.sem_op = 1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);

    } while (msg > 0);

    if (semctl(SemId, 0, IPC_RMID, 0) == -1)    {
        printf("semctl err\n");
        exit(6);
    }
}