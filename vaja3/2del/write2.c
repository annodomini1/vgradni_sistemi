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

    from_file = open("/home/pi/Desktop/fifoout", O_RDONLY );
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
        lseek(from_file, 0, SEEK_SET);
        lseek(to_file, 0, SEEK_SET);
        for (int i=0; i<480; i++)
        {
            write(to_file, buffer, buffer_size);
            lseek(to_file, 640*4, SEEK_CUR);
        }
    }
    return 0;
}

