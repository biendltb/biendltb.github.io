    /**
     * Sample code for sharing memory between processes
     * Two processes will iteratively increase a counter which values stored in a shared memory
     * 
     */


    #include <stdio.h>
    #include <unistd.h> // for fork()

    #include <sys/mman.h> // for shared memory created
    #include <sys/stat.h> // for mode constants
    #include <fcntl.h> // for O_* constant

    #define SHARED_OBJ_NAME "/somename"

    // shared data struct
    struct message
    {
        int pid;
        int counter;
    };

    bool write_message(int pid, int value)
    {
        int shmFd = shm_open(SHARED_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        ftruncate(shmFd, sizeof(message));
        message *msg_ptr = (message*)mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
        
        printf("Process %d: Increase the counter.\n", pid);
        msg_ptr->pid = pid;
        msg_ptr->counter = value;

        munmap(msg_ptr, sizeof(message));

        // remember to close to not hit an error of
        // opening too many files
        close(shmFd);

        return true;
    }

    bool read_message(int curr_pid, int &curr_value)
    {
        int shmFd = shm_open(SHARED_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
        ftruncate(shmFd, sizeof(message));
        message *msg_ptr = (message*)mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

        if (msg_ptr->pid == curr_pid)
        {
            printf("Process %d: No new msg available.\n", curr_pid);
            return false;
        }
        else
        {
            printf("Process %d: Receive %d from PID %d.\n", curr_pid, msg_ptr->counter, msg_ptr->pid);
            curr_value = msg_ptr->counter;
            munmap(msg_ptr, sizeof(message));
        }

        close(shmFd);

        return true;
    }

    int main(int argc, char **argv)
    {
        printf("Init the initial value.\n");
        write_message(-1, 0);

        // create a child process by calling folk, 
        // it returns a non-zero pid for parent process and 0 for child process created
        pid_t pid = fork();

        //--- PARENT PROCESS
        if (pid != 0)
        {
            for (int i = 0; i < 5; i++)
            {
                int value;
                // only write message if reading sucessfully
                if (read_message(pid, value))
                    write_message(pid, ++value);
                sleep(0.1);
            }
        }

        //--- CHILD PROCESS
        else
        {
            for (int j = 0; j < 5; j++)
            {
                int value;
                if (read_message(pid, value))
                    write_message(pid, ++value);
                sleep(0.1);
            }
        }

        printf("=========== End of process %d\n", pid);
        // shm_unlink(SHARED_OBJ_NAME);

        return 0;
    }