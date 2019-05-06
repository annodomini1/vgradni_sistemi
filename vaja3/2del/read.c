#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int from_file;
    int to_file;
    int buffer_size = 640*480*3;
    char *buffer;
    int pack;

    from_file = open("/dev/video0", O_RDONLY);

    if (from_file == -1)
    {
        printf("napacen vnos from\n");
        exit(1);
    }

    to_file = creat("/home/pi/Desktop/fifoin", 0644);
    if (to_file == -1)
    {
        printf("napacen vnos to\n");
        exit(2);
    }

    buffer = (char *)malloc(buffer_size);
        
    while ((pack = read(from_file, buffer, buffer_size)) > 0)
    {
    lseek(to_file, 0, SEEK_SET);
    write(to_file, buffer, pack);
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

