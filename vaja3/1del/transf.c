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
    int buffer_size_read = 640*480*3;
    int buffer_size_write = 640*480*4;
    char *buffer_read;
    char *buffer_write;
    int pack;
    int img_in;

    from_file = open("/home/pi/Desktop/vhod1.raw", O_RDONLY);
    if (from_file == -1)
    {
        printf("napacen vnos from\n");
        exit(1);
    }
    
    to_file = creat("/home/pi/Desktop/vhod2.raw", 0644);
    if (to_file == -1)
    {
        printf("napacen vnos to\n");
        exit(2);
    }

    buffer_read = (char *)malloc(buffer_size_read);
    buffer_write = (char *)malloc(buffer_size_write);
    
    while((img_in = read(from_file, buffer_read, buffer_size_read)) > 0)
    {
        sleep(2);
        lseek(from_file, 0, SEEK_SET);//seek set: abs. pomik glede na začetek
        for (int i=0; i <= 640*480; i++)
        {
            //pozor: rbg/bgr, 24bit -> 32bit
            buffer_write[i*4+0] = buffer_read[i*3 + 2];
            buffer_write[i*4+1] = buffer_read[i*3 + 1];
            buffer_write[i*4+2] = buffer_read[i*3 + 0];
            buffer_write[i*4+3] = 0; //četrti bit je svetlost 
        }
        lseek(to_file, 0, SEEK_SET);
        write(to_file, buffer_write, buffer_size_write);
    }       
    return 0;
}


