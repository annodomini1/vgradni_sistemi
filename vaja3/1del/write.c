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
    int buffer_size = 640*480*4;
    char *buffer;
    int pack;

    from_file = open("/home/pi/Desktop/vhod2.raw", O_RDONLY );
    to_file = open("/dev/fb0", O_WRONLY);

    if (from_file == -1)
    {
        printf("napacen vnos from\n");
        exit(1);
    }

    if (to_file == -1)
    {
        printf("napacen vnos to\n");
        exit(2);
    }

    buffer = (char *)malloc(buffer_size);
    
    while (read(from_file, buffer, buffer_size) > 0)
    {
        sleep(2);
        lseek(from_file, 0, SEEK_SET);
        lseek(to_file, 0, SEEK_SET);
        write(to_file, buffer, buffer_size);
    }
    return 0;
}

