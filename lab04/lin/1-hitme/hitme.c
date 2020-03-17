/**
  * SO, 2016
  * Lab #4
  *
  * Task #1, lin
  *
  * Catching signals
  */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "utils.h"

static int must_leave;

static int signals_to_mask[] =
{ SIGSEGV, SIGTSTP, SIGTTIN, SIGTTOU, SIGURG, SIGXCPU, SIGXFSZ };

static void sig_handler(int signum)
{
	switch (signum) {
	case SIGSEGV:
		printf("No! Do NOT access bad memory! Bad programmer!\n");
		break;
	case SIGTSTP:
		printf("H");
		break;
	case SIGTTIN:
		printf("e");
		break;
	case SIGTTOU:
		printf("l");
		break;
	case SIGURG:
		printf("l");
		break;
	case SIGXCPU:
		printf("o");
		break;
	case SIGXFSZ:
		printf("\n");
		    must_leave = 1;
		break;
	default:
		printf("Go back and send the following signals: "
		       "20, 21, 22, 23, 24");
		must_leave = 1;
	}
	fflush(stdout);
}

int main(void)
{
	struct sigaction signals;
	void *mem = NULL;
	sigset_t mask;
	unsigned int i;
	int rc;
	char *magic_string = "Please don't change me!";
	sigemptyset(&mask);

	memset(&signals, 0, sizeof(struct sigaction));
	signals.sa_flags = SA_RESETHAND;
	signals.sa_mask = mask;

	signals.sa_handler = sig_handler;

	for (i = 0; i < sizeof(signals_to_mask) / sizeof(int); i++) {
		rc = sigaction(signals_to_mask[i], &signals, NULL);
		DIE(rc == -1, "sigaction");
	}

	magic_string[0] = 'a';

	while (!must_leave)
		sleep(1);

	return 0;
}
