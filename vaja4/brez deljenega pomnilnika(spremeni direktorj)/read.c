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

int main(void) //ta program v originalu piše
{
    int fdOpen, msg, buffer_size, to_file;
    char *buffer;
    int SemId;
    unsigned short SemArray[2];
    struct sembuf Semaphore;

    buffer_size = 1;
    buffer = (char *)malloc(buffer_size);

    if ((fdOpen = open("/home/martin/Desktop/vgradni/vaja4/cifre.txt", O_RDONLY)) == -1) {
        printf("open err\n");
        exit(6);
    }
    printf("%d\n", fdOpen);

    if ((to_file = creat("/home/martin/Desktop/vgradni/vaja4/cifre_write.txt", 0644)) == -1) {
            printf("creat err\n");
            exit(4);
        }

    if ((SemId = semget(SEM_KEY_NO, 2, IPC_CREAT | 0644)) == -1)    {
        printf("semget err\n");
        if (semctl(SemId, 0, IPC_RMID, 0) == -1)    {//odstrani se množica semaforjev
            printf("semctl err\n");
            exit(1);
        }
    }

    //nastavitev semaforjev
    SemArray[SEM_READ] = 0;
    SemArray[SEM_WRITE] = 1;

    if (semctl(SemId, 0, SETALL, SemArray) == -1)   {
        printf("semctl/sem set err\n");
        exit(2);
    }

    do {
        //write lock
        Semaphore.sem_num = SEM_WRITE;
        Semaphore.sem_op = -1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);

        if ((msg = read(fdOpen, buffer, buffer_size)) == -1) {
            printf("read err\n");
            exit(3);
        }
        
        if (write(to_file, buffer, msg) < 0) {
            printf("write err\n");
            exit(5);
        }

        //unlock read
        Semaphore.sem_num = SEM_READ;
        Semaphore.sem_op = 1;
        Semaphore.sem_flg = 0;
        semop(SemId, &Semaphore, 1);

    } while (msg > 0);//tuki je lahko tudi 1
}