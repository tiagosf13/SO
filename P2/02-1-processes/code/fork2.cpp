#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "delays.h"
#include "process.h"

int main(void)
{
  printf("Before the fork:\n");
  printf("  PID = %d, PPID = %d\n", getpid(), getppid());
  // The ret variable returns the process ID of the child process to the parent process, and returns 0 to the child process.
  pid_t ret = pfork();

  printf("After the fork:\n");
  printf("  PID = %d, PPID = %d\n", getpid(), getppid());
  bwRandomDelay(0, 10000);
  printf("  [ret = %d] Was I printed by the parent or by the child process? How can I know it?\n", ret); 

  return EXIT_SUCCESS;
}

