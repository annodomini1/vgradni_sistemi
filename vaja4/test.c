#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MY_MODE O_CREAT | O_WRONLY | O_TRUNC
#define MY_FLAGS S_IRUSR | S_IWUSR

int main()
{
    key_t key;
    char *filename = "/dev/random";
    int shmid;
    int semid;

    //ustvarimo datoteko
    if (open(filename, MY_MODE, MY_FLAGS) == -1)
    {
        perror("open err\n");
    }

    //ustvarimo unikaten key
    if ((key = ftok(filename, 'k')) == -1)
    {
        perror("ftok err\n");
    }

    //alokacija deljenega pomnilnika
    if ((shmid = shmget(key, 4069, IPC_CREAT | 0644)) == -1)    
    {
        perror("shmget err\n");
    }

    if ((semid = semget(IPC_PRIVATE, 2, 0644)) == -1)//ALI JE POTREBNA GENERACIJA DODATNEGA KEY-A?
    {
        perror("semget err\n");
    }

    printf("key: %d\n", key);
    printf("shmid: %d\n", shmid);
    printf("semid: %d\n", semid);

    int semarray[2];
    semarray[0] = 0;
    semarray[1] = 1;
    printf("%d\n", semarray[1]);

} 

//TODO: ostal na str. 243 pri semop

//nekaj o semaforjih:
//semget - ustvari mnozico semaforjev
//semctl - upravlja z mnozico semaforjev, postavitev zacetnih vrednosti
//semop - izvaja operaciji: cakaj/javi

//nekaj o deljenem pomnilniku:
//shmget - zahteva deljen pomnilnik
//shmctl - upravljanje z delj. pom.
//shmat - procesu pripne pomnilnik in mu omogoci dostop
//shmdt - deljeni pomnilnik odpne in dostop do pom. ni vec mogoc
//SEGEMNTE DELJENEGA POMNILNIK LAHKO VIDIMO Z: ipcs

//get funkcije vrnejo IDENTIFIKATOR, ki je prepoznaven na nivoju sistema (ne samo provesa kot deskriptor)