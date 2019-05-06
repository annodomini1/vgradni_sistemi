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
    pid_t PID1;
    pid_t PID2;
    int pipe_fd1[2];
    int pipe_fd2[2]; 
    //read
    int from_file;  
    char *buffer;
    int pack;
    //transf
    int to_file;
    int from_file2;
    char *buffer_read;
    char *buffer_write;
    int img_in;
    //write
    int from_file_write;
    int to_file_write;
    char *buffer2;
    int write3;
    
    pipe(pipe_fd1);
    pipe(pipe_fd2);  
    printf("%d %d\n", pipe_fd1[0], pipe_fd1[1]);
    printf("%d %d\n", pipe_fd2[0], pipe_fd2[1]);
      
    PID1 = fork();

    //fork: 0 - otrok, >0 - roditelj
    
    if (PID1 > 0)
    {
        PID2 = fork();
        
        if (PID2 > 0)//read - proces 1
        {   
            from_file = open("/dev/video0", O_RDONLY);
            if (from_file<0)
            {
                    perror("Napaka open video0 "); fflush(stdout);
            };

            buffer = (char *)malloc(640*480*3);
    
            while (1)
            {
                pack = read(from_file, buffer, 640*480*3);
                write(pipe_fd1[1], buffer, pack);//cev za pisanje
            }
        }
        
        else//write - proces 3
        {
            to_file_write = open("/dev/fb0", O_WRONLY);

            buffer2 = (char *)malloc(640*480*4);
    
            while (1)
            {
                read(pipe_fd2[0], buffer2, 640*480*4);
                lseek(to_file_write, 0, SEEK_SET);
                write(to_file_write, buffer2, 640*480*4);
            }
        }
        
    }
    
    else//transf - proces 2
    {
        buffer_read = (char *)malloc(640*480*3);
        buffer_write = (char *)malloc(640*480*4);

        while(1)
        {
            img_in = read(pipe_fd1[0], buffer_read, 640*480*3);
            for (int i=0; i <= 640*480; i++)
            {
                buffer_write[i*4+0] = buffer_read[i*3 + 2];
                buffer_write[i*4+1] = buffer_read[i*3 + 1];
                buffer_write[i*4+2] = buffer_read[i*3 + 0];
                buffer_write[i*4+3] = 0;
            }
            write(pipe_fd2[1], buffer_write, 640*480*4);
        }
    }
    return 0;
}

