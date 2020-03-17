/**
  * SO, 2016
  * Lab #4
  *
  * Task #5, lin
  *
  * Avoid creating zombies using signals
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


#include "utils.h"

#define TIMEOUT		20

/*
 * configure signal handler
 */
static void set_signals(void)
{
	signal(SIGCHLD, SIG_IGN);
}

int main(void)
{
	pid_t pid;
	int done = 100;

	/* create child process */
	pid = fork();
	if (pid < 0) {
		printf("[fork] failed\n");
		return -1;
	}
	set_signals();

	/* sleep */
	while (done--)
		sleep(1);

	return 0;
}
