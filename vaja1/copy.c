#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
    int from_file;
    int to_file;
    int buffer_size = 1024*1024;
    int *buffer;//pointer na integer
    int counter;
    int pack;
    //pointer: spremenljivka, ki vsebuje naslov druge spremenljivke
    //&-> "daj mi naslov"
    //*-> "daj mi vsebino naslova"

    printf("arg1: from, arg2: to\n");

    from_file = open(argv[1], O_RDONLY); //lib:fcntl.h
    //open vrne majhen nenegativen int, ce je vse ok
    if (from_file == -1)
    {
        printf("napacen vnos from\n");
        exit(1);
    }

    //to_file = creat(argv[2], 0644); //0644 pravice za urejanje, b: 110 100 100-> rw-r--r-- ->osmisko: 0644
    to_file = creat(argv[2], 0644);
    if (to_file == -1)
    {
        printf("napacen vnos to\n");
        exit(2);
    }

    buffer = (int *)malloc(buffer_size);//alokacija prostora v spominu (byti), vrne pointer na alociran prostor

    while((pack = read(from_file, buffer, 1024)) > 0) //izhod read -> stevilo prebranih bite-ov
    //read pise v buffer pakete v velikosti 1024*1024, "from_file" je file descriptor (poz. int)
    {
        printf("pack size: %d\n", pack);
        write(to_file, buffer, pack);
        //write pise pise iz bufferja pakete v velikosti pack v file descriptor "to_file"
        //pri uspesnem prenosu vrne velikost prenesenega paketa
    }

    if (close(from_file) == -1)
    {
        exit(4);
    }
    if (close(to_file) == -1)
    {
        exit(5);
    }
    return 0;
}