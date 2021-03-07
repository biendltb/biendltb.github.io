/**
 * Sample for using named pipe for communicate between processes
 * 
 */

#include <unistd.h> // for fork()
#include <stdio.h> // for printf
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // for O_* constant

// for print error message
#include <string.h>
#include <errno.h>

#define NAMED_PIPE "/var/lock/pipename"

// shared data struct
struct message
{
    int pid;
    int counter;
};


int main(int argc, char **argv)
{
    // create the named pipe (fifo) with permission
    int ret = mkfifo(NAMED_PIPE, 0666);
    if (ret < 0)
        printf("Error when creating FIFO. %s\n", strerror(errno));

    // create a child process by calling folk, 
    // it returns a non-zero pid for parent process and 0 for child process created
    pid_t pid = fork();

    //--- the parent process will write to the pipe only
    if (pid != 0)
    {
        int fd = open(NAMED_PIPE, O_WRONLY);
        for (int i = 0; i < 5; i++)
        {
            message msg;
            msg.pid = pid;
            msg.counter = i;
            printf("Process %d: Write %d.\n", pid, i);
            ret = write(fd, &msg, sizeof(msg));
            if (ret < 0)
                printf("Process %d: Error while writing message. %s\n", pid, strerror(errno));
            sleep(0.1);
        }
        close(fd);
    }

    //-- child process will read only
    else
    {
        
        int fd = open(NAMED_PIPE, O_RDONLY);
        for (int i = 0; i < 5; i++)
        {
            message msg;
            ret = read(fd, &msg, sizeof(msg));
            if (ret < 0)
                printf("Process %d: Error while reading message. %s\n", pid, strerror(errno));
            printf("Process %d: Received value %d from the parent process %d.\n", pid, msg.counter, msg.pid);
            sleep(0.1);
        }
        close(fd);
    }

    unlink(NAMED_PIPE);

    return 0;
}