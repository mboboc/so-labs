/**
 * SO, 2017
 * Lab #6
 *
 * Task #5, lin
 *
 * Changing page access protection
 */
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "utils.h"

static int page_size;
static struct sigaction old_action;
char *p;
int how[3] = { PROT_NONE, PROT_READ, PROT_WRITE };

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	char *addr = NULL, *page_addr = NULL;
	int rc = 0;
	
	/* Old signal handler */
	// old_action.sa_sigaction(signum, info, context);

	/* check if the signal is SIGSEGV */
	if (signum == SIGSEGV) {
		perror("I received SEGFAULT.\n");
	}

	/* Get the memory location which caused the page fault */
	addr = info->si_addr;

	/* 
     * Obtain the page which caused the page fault
	 * Hint: use the address returned by mmap
	 */
	*page_addr = *addr / page_size * page_size;

	/* increase protection for that page */
	rc = mmap(page_addr, page_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANON, -1, 0);
	if (rc < 0) {
		printf("[mprotect] failed\n");
		return;
	}

}

static void set_signal(void)
{
	struct sigaction action;
	int rc;

	action.sa_sigaction = segv_handler;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGSEGV);
	action.sa_flags = SA_SIGINFO;

	rc = sigaction(SIGSEGV, &action, &old_action);
	DIE(rc == -1, "sigaction");
}

static void restore_signal(void)
{
	struct sigaction action;
	int rc;

	action.sa_sigaction = old_action.sa_sigaction;
	sigemptyset(&action.sa_mask);
	sigaddset(&action.sa_mask, SIGSEGV);
	action.sa_flags = SA_SIGINFO;

	rc = sigaction(SIGSEGV, &action, NULL);
	DIE(rc == -1, "sigaction");
}

int main(void)
{
	int rc;
	char ch = 1;

	page_size = getpagesize();

	p = mmap(NULL, 3 * page_size, PROT_NONE, MAP_ANON | MAP_SHARED, -1, 0);
	if (p == MAP_FAILED) {
		printf("[mmap] failed\n");
		return -1;
	}

	rc = mprotect(p, page_size, how[0]);
	if (rc < 0) {
		printf("[mprotect] failed\n");
		return -1;
	}

	rc = mprotect(p + page_size, page_size, how[2]);
	if (rc < 0) {
		printf("[mprotect] failed\n");
		return -1;
	}

	rc = mprotect(p + 2 * page_size, page_size, how[1]);
	if (rc < 0) {
		printf("[mprotect] failed\n");
		return -1;
	}

	set_signal();

	/* Access these pages for read and write */
	p[1] = ch;
	// p[2 * page_size + 1] = ch;
	// p[3 * page_size + 1] = ch;

	// printf("p[1] %c\n", p[1]);
	// printf("p[page_size + 1] %c\n", p[page_size + 1]);
	// printf("p[2 * page_size + 1] %c\n", p[2 * page_size + 1]);

	restore_signal();

	/* unmap */
	rc = munmap(p, 3 * page_size);
	if (rc < 0) {
		printf("[munmap] failed\n");
		return -1;
	}

	return 0;
}
