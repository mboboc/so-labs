/**
  * SO, 2016
  * Lab #4
  *
  * Task #5, lin
  *
  * Creating zombies
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define TIMEOUT		20


int main(void)
{
	pid_t pid;
  int done = 100;

	/* create child process without waiting */
    pid = fork();
  if (pid < 0) {
    printf("[fork] failed\n");
    return -1;
  }

	/* sleep */
  while (done--)
    sleep(1);

	return 0;
}
