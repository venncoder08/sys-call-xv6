#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    int fd1[2]; // Parent -> Child
    int fd2[2]; // Child -> Parent

    if(pipe(fd1) < 0 || pipe(fd2) < 0)
    {
        printf("Pipe failed.\n");
        exit(0);
    }

    int pid = fork();
    if(pid < 0)
    {
        printf("Fork failed.\n");
        exit(0);
    }

    if(pid == 0) // Child process
    {
        close(fd1[1]); // Close the write end of fd1 (child only reads from fd1)
        close(fd2[0]); // Close the read end of fd2 (child only writes to fd2)

        char buffer;
        if(read(fd1[0], &buffer, sizeof(char)) != 1)
        {
            printf("Child read failed.\n");
            exit(0);
        }

        printf("%d: received ping\n", getpid()); // Print child's PID with "received ping"

        if(write(fd2[1], &buffer, sizeof(char)) != 1)
        {
            printf("Child write failed.\n");
            exit(0);
        }

        close(fd1[0]);
        close(fd2[1]);
        exit(0);
    }
    else // Parent process
    {
        close(fd1[0]); // Close the read end of fd1 (parent only writes to fd1)
        close(fd2[1]); // Close the write end of fd2 (parent only reads from fd2)

        char buffer = ';'; // Byte to send
        if(write(fd1[1], &buffer, sizeof(char)) != 1)
        {
            printf("Parent write failed.\n");
            exit(0);
        }

        if(read(fd2[0], &buffer, sizeof(char)) != 1)
        {
            printf("Parent read failed.\n");
            exit(0);
        }

        printf("%d: received pong\n", getpid()); // Print parent's PID with "received pong"

        close(fd1[1]);
        close(fd2[0]);
        wait(0); // Wait for child process
        exit(0);
    }
}