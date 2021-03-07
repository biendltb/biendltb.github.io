---
layout: single
title: "Communication between processes in C/C++"
date: 2021-03-03T06:43:00+07:00
categories: tech
classes:
  - dark-theme
tags:
  - technology
  - ipc
  - process
  - c++
  - named pipe
  - shared memory
  - UNIX domain socket
---

Common techniques for communication between processes in C/C++

{% include toc icon="cog" title="Table of Contents" %}

Many choose process over thread for their program since it allows us to separate heavy operations and to provide more independence for further optimization. Take Google Chrome as an example, they choose to use processes to run each of their tabs on the browser. This design choice comes from the fact that each browser tab has to process heavy tasks, e.g. communications with servers, taking care of network resources, rendering, etc., and tabs work very independently. One failed process will not affect or block others. Moreover, this design allows them to go further with the memory optimization since inactive tabs will be treated as lower priority tasks by the operating system and will be swapped to disk when memory run low and is saved for other processes.

There are many situations in which processes have to co-operate in order to increase computational speed and efficiency. However, since they are designed to run on independent memory, they need special communication mechanism provided by the system. This article will present the three most common methods: shared memory, named pipe and UNIX domain socket (different to TCP/IP socket).

## Overview

First, let's take an overview at pros and cons of each of those methods.

| Shared memory               | Named pipe (FIFO)         | UNIX domain socket        |
|:---------------------------:|:-------------------------:|:-------------------------:|
| Asynchronous                | Synchronous (for unidirectial pipe) | Synchronous |
| Data size limit to the physical memory | Queue size is at 16 pages (4096 bytes per page). No limit on data size as long as the data is consumed fast enough | Extremely high throughput. Limit to the speed of the storage device.|
| Multiple processes can access at the same time but synchronization management needed | Unidirectional communication for a single pipe | Bi-directional communication |
| Random access               | Read-write in a linear fashion | Read-write in a linear fashion |
| Manual memory management    | Auto memory mamangement    | Auto memory management   |

In the three techniques listed above, shared memory is the most low-level approach since you have to manage basically everything, whereas UNIX domain socket seems provide you enough devices to set up a proper communication channel between processes at ease. However, convenience always comes with its cost. Shared memory, in the other hand, gives you the flexibility to optimize every bit of the data transfer to fit any purposes with less restrictions. It enables you to take the full use of your memory capacity, manage concurrent accesses from multiple processes. Back to the problem that you are trying to solve, there is no the best method but more or less depend on your purposes and resources.

Let's go deeper into the details of each of those methods with simple examples. (Note: examples used here only target Linux platform, some modifications needed to run on others).

## Shared memory

Shared memory is the fastest one among available IPC techniques. Once the memory is mapped into the address space of the processes that are sharing the memory region, no kernel involvement occurs in passing data between the processes. However, some forms of synchronization between processes (e.g. mutexes, condition variables, read-write locks, record locks, semaphores) are required when storing and fetching data to and from the shared memory region. For the sake of simplicity, the following example will not use any synchronization technique.

In the following example, `fork()` will be used to "duplicate" the parent process into one more child process. We will use pid to differentiate them since child process pid will start with 0. `shm_open` is used to create a shared memory object in the form of a file descriptor (stored in the OS kernel) which will then be mapped to the address space of processes using `mmap`. This returns the address of the pointer which points to the actual shared memory region.

The following program will iteratively increases a counter which passes between the parent and child process.

```cpp
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
```

The above code could be built using `gcc` and run as:

```bash
gcc shared_memory.cpp -I/usr/local/include/ -L/usr/local/lib -lrt -o shared_mem
./shared_mem
```

The outputs from the program:

```txt
Set the initial value.
Process -1: Increase the counter.
Process 26851: Receive 0 from PID -1.
Process 26851: Increase the counter.
Process 26851: No new msg received
Process 0: Receive 1 from PID 26851.
Process 0: Increase the counter.
Process 26851: No new msg received
Process 26851: Receive 2 from PID 0.
Process 0: No new msg received
Process 26851: Increase the counter.
Process 0: Receive 3 from PID 26851.
Process 0: Increase the counter.
Process 26851: Receive 4 from PID 0.
Process 26851: Increase the counter.
Process 0: Receive 5 from PID 26851.
Process 0: Increase the counter.
=========== End of process 26851
Process 0: No new msg received
=========== End of process 0
```

In the example, we could see that the structural message is passed between the parent and the child. A process increase the value and pass it to the other and wait until the other process increase it. All data passing occurs in a shared memory space.

## Named pipe

Named pipe (FIFO) is a special file similar to a pipe but with a name on the filesystem. It is used like message passing where a process send a piece of information and other process receive it. Data goes into the pipe with high throughput speed in a FIFO style. However, the maximum size of data that the queue could hold at a time is 16 pages or 65536 bytes.

```cpp
/**
 * Example for using named pipe for communicating between processes
 * This demo is for a unidirectional named pipe which transfer data in one direction
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
```

Build and run the example:

```bash
gcc named_pipe.cpp -I/usr/local/include/ -L/usr/local/lib -o named_pipe
./named_pipe
```

Output:
```txt
Process 17830: Write 0.
Process 17830: Write 1.
Process 17830: Write 2.
Process 0: Received value 0 from the parent process 17830.
Process 17830: Write 3.
Process 0: Received value 1 from the parent process 17830.
Process 17830: Write 4.
Process 0: Received value 2 from the parent process 17830.
Process 0: Received value 3 from the parent process 17830.
Process 0: Received value 4 from the parent process 17830.
```

## UNIX domain socket

UNIX domain socket provides us a convenient way to set up a communication channel between processes with many useful built-in features. It supports both stream-oriented (TCP) and datagram-oriented (UDP) protocols as the TCP/IP internet socket. We can also choose between the blocking and non-blocking modes.

You first need to create the socket and specify `AF_UNIX` as the domain socket in the `socket` function. After a it is created, you must bind the socket to a unique file path using the `bind` function. Unlike internet sockets in the AF_INET domain where the socket is bound to a unique IP address and port number, a UNIX domain socket is bound to a file path. A file will be created in the file system and you have to manually remove it when the programe closes and the file is no longer required.

In the example bellow, a client will connect and say hello to server and receive the response from the server.

```cpp
/**
 * Sample for UNIX domain socket
 * Two-way communication between process server and client:
 * 1) Client connect and say hello to server
 * 2) Server receive client message and display
 * 3) Server say hello back to client
 * 4) Client receive server message and display
 * 5) Done.
 */

#include <unistd.h> // for fork()
#include <stdio.h> // for printf
#include <stdlib.h> // for exit()
#include <sys/socket.h>
#include <sys/un.h> // socket in Unix

// for print error message
#include <string.h>
#include <errno.h>

#define SERVER_SOCK_PATH "unix_sock.server"
#define CLIENT_SOCK_PATH "unix_sock.client"
#define SERVER_MSG "HELLO FROM SERVER"
#define CLIENT_MSG "HELLO FROM CLIENT"


int main(int argc, char **argv)
{
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));
    int rc;
    // for simplicity, we will assign a fixed size for buffer of the message
    char buf[256];

    // create two processes of client and server
    pid_t pid = fork();


    //---------- SERVER PROCESS
    if (pid != 0)
    {
        // maximum number of client connections in queue
        int backlog = 10;

        /****************************************************/
        /* Open the server socket with the SOCK_STREAM type */
        /****************************************************/
        int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_sock == -1)
        {
            printf("SERVER: Error when opening server socket.\n");
            exit(1);
        }

        /*************************************/
        /* Bind to an address on file system */
        /*************************************/
        // similar to other IPC methods, domain socket needs to bind to a file system
        // so that client know the address of the server to connect to
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
        int len = sizeof(server_addr);

        // unlink the file before bind, unless it can't bind
        unlink(SERVER_SOCK_PATH);

        rc = bind(server_sock, (struct sockaddr *)&server_addr, len);
        if (rc == -1)
        {
            printf("SERVER: Server bind error: %s\n", strerror(errno));
            close(server_sock);
            exit(1);
        }

        /***************************************/
        /* Listen and accept client connection */
        /***************************************/
        // set the server in the "listen" mode and maximum pending connected clients in queue
        rc = listen(server_sock, backlog);
        if (rc == -1)
        {
            printf("SERVER: Listen error: %s\n", strerror(errno));
            close(server_sock);
            exit(1);
        }

        printf("SERVER: Socket listening...\n");
        int client_fd = accept(server_sock, (struct sockaddr *) &client_addr, (socklen_t*)&len);
        if (client_fd == -1)
        {
            printf("SERVER: Accept error: %s\n", strerror(errno));
            close(server_sock);
            close(client_fd);
            exit(1);
        }
        printf("SERVER: Connected to client at: %s\n", client_addr.sun_path);
        printf("SERVER: Wating for message...\n");

        /********************/
        /* Listen to client */
        /********************/
        memset(buf, 0, 256);
        int byte_recv = recv(client_fd, buf, sizeof(buf), 0);
        if (byte_recv == -1)
        {
            printf("SERVER: Error when receiving message: %s\n", strerror(errno));
            close(server_sock);
            close(client_fd);
            exit(1);
        }
        else
            printf("SERVER: Server received message: %s.\n", buf);

        /**********************/
        /* Response to client */
        /**********************/
        printf("SERVER: Respond to the client...\n");
        memset(buf, 0, 256);
        strcpy(buf, SERVER_MSG);
        rc = send(client_fd, buf, strlen(buf), 0);
        if (rc == -1)
        {
            printf("SERVER: Error when sending message to client.\n");
            close(server_sock);
            close(client_fd);
            exit(1);
        }
        printf("SERVER: Done!\n");

        close(server_sock);
        close(client_fd);
        remove(SERVER_SOCK_PATH);
    }

    //---------- CLIENT PROCESS
    else
    {
        /**************************************************/
        /* Open a client socket (same type as the server) */
        /**************************************************/
        int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client_sock == -1)
        {
            printf("CLIENT: Socket error: %s\n", strerror(errno));
            exit(1);
        }

        /********************************************/
        /* Bind client to an address on file system */
        /********************************************/
        // Note: this binding could be skip if we want only send data to server without receiving
        client_addr.sun_family = AF_UNIX;
        strcpy(client_addr.sun_path, CLIENT_SOCK_PATH);
        int len = sizeof(client_addr);

        unlink (CLIENT_SOCK_PATH);
        rc = bind(client_sock, (struct sockaddr *)&client_addr, len);
        if (rc == -1)
        {
            printf("CLIENT: Client binding error. %s\n", strerror(errno));
            close(client_sock);
            exit(1);
        }

        /****************************************/
        /* Set server address and connect to it */
        /****************************************/
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
        rc = connect(client_sock, (struct sockaddr*)&server_addr, len);
        if(rc == -1)
        {
            printf("CLIENT: Connect error. %s\n", strerror(errno));
            close(client_sock);
            exit(1);
        }
        printf("CLIENT: Connected to server.\n");

        /**************************/
        /* Send message to server */
        /**************************/
        memset(buf, 0, sizeof(buf));
        strcpy(buf, CLIENT_MSG);
        rc = send(client_sock, buf, sizeof(buf), 0);
        if (rc == -1)
        {
            printf("CLIENT: Send error. %s\n", strerror(errno));
            close(client_sock);
            exit(1);
        }
        printf("CLIENT: Sent a message to server.\n");

        /**************************************/
        /* Listen to the response from server */
        /**************************************/
        printf("CLIENT: Wait for respond from server...\n");
        memset(buf, 0, sizeof(buf));
        rc = recv(client_sock, buf, sizeof(buf), 0);
        if (rc == -1)
        {
            printf("CLIENT: Recv Error. %s\n", strerror(errno));
            close(client_sock);
            exit(1);
        }
        else
            printf("CLIENT: Message received: %s\n", buf);

        printf("CLIENT: Done!\n");

        close(client_sock);
        remove(CLIENT_SOCK_PATH);
    }

    return 0;
}
```

Build and run the example:
```bash
gcc unix_domain_socket.cpp -I/usr/local/include/ -L/usr/local/lib -o unix_domain_socket
./unix_domain_socket
```

Output:

```txt
SERVER: Socket listening...
SERVER: Connected to client at: unix_sock.client
SERVER: Wating for message...
CLIENT: Connected to server.
CLIENT: Sent a message to server.
CLIENT: Wait for respond from server...
SERVER: Server received message: HELLO FROM CLIENT.
SERVER: Respond to the client...
SERVER: Done!
CLIENT: Message received: HELLO FROM SERVER
CLIENT: Done!
```