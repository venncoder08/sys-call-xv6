#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Prevent program from infinite recursion error.
void sieve(int read_fd) __attribute__((noreturn));

// Implement the Sieve of Eratosthenes algorithm
void sieve(int read_fd)
{
  int p; // The next prime number
  int n; // The number of remaining number read

  // Read the first prime number from the pipe
  if (read(read_fd, &p, sizeof(int)) <= 0)
  {
    printf("Failed to read a prime.\n");
    exit(0);
  }

  printf("prime %d\n", p);

  // File descriptor array for new pipe
  int fd[2];

  // Create a new pipe
  if (pipe(fd) < 0)
  {
    printf("Failed to create pipe.\n"); // Print error if pipe creation fails
    exit(0);
  }

  // Create child process
  int pid = fork();
  if (pid < 0)
  {
    printf("Failed to fork.\n"); // Print error if fork fails
    exit(0);
  }

  if (pid == 0)
  {
    close(fd[1]); // Close the write end of the new pipe
    close(read_fd); // Close the read end of the old pipe
    sieve(fd[0]);
  }
  else
  {
    close(fd[0]); // Close the read end of the new pipe

    // Read the remaining numbers from the old pipe
    while (read(read_fd, &n, sizeof(int)) > 0)
    {
      // If n is not divisible by p, write it to the new pipe
      if (n % p != 0) 
      {
        if (write(fd[1], &n, sizeof(int)) != sizeof(int))
        {
          printf(("Failed to write to pipe.\n")); // Print error if write fails
          exit(0);
        }
      }
    }

    close(fd[1]); // Close the write end of the new pipe
    close(read_fd); // Close the read end of the old pipe

    // Wait for child process to finish
    wait(0);
    exit(0);
  }
}

// Main function
int main(int argc, char* argv[])
{
  int fd[2]; // File descriptor array for the first pipe

  // Create a pipe
  if (pipe(fd) < 0)
  {
    printf("Failed to create pipe.\n"); // Print error if pipe creation fails
    exit(0);
  }

  // Create child process
  int pid = fork();
  if (pid < 0)
  {
    printf("Failed to fork.\n"); // Print error if fork fails
    exit(0);
  }

  // fd[1] - the write end of the pipe
  // fd[0] - the read end of the pipe

  if (pid == 0) // Child process
  {
    close(fd[1]); // Close the write end of the pipe
    sieve(fd[0]); // Call sieve function with the read end of the pipe
  }
  else // Parent process
  {
    close(fd[0]); // Close the read end of the pipe

    // Write numbers from 2 to 280 into the pipe
    for (int i = 2; i <= 280; ++i)
    {
      if (write(fd[1], &i, sizeof(int)) != sizeof(int))
      {
        printf(("Failed to write to pipe.\n")); // Print error if write fails
        exit(0);
      }
    }

    close(fd[1]); // Close the write end of the pipe

    // Wait for child process to finish
    wait(0);
    exit(0);
  }

  return 0;
}
