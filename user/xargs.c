#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    char buffer[512];
    char* newargv[MAXARG];
    int newargc = argc - 1; // Number of arguments excluding program name

    // Copy initial arguments into newargv
    for (int i = 0; i < newargc; ++i) {
        newargv[i] = argv[i + 1];
    }

    int idx = 0;
    // Read input from stdin and process it line by line
    while (read(0, &buffer[idx], sizeof(char)) > 0) {
        if (buffer[idx] == '\n' || idx == sizeof(buffer) - 1)
            {
            buffer[idx] = 0; // Null-terminate the string
            if (idx > 0 && buffer[idx - 1] == '\n') {
                buffer[idx - 1] = 0; // Remove extra newline
            }

            // Add the new argument to newargv
            newargv[newargc] = buffer;
            newargv[newargc + 1] = 0; // Terminate the argument list

            // Fork a new process and execute the command
            if (fork() == 0) {
                if (exec(newargv[0], newargv) < 0) {
                    printf("exec failed\n");
                    exit(1); // Exit child process with error
                }
            }

            wait(0); // Wait for the child process to finish
            idx = 0; // Reset index for next line
        } else {
            ++idx;
        }
    }

    exit(0);
}