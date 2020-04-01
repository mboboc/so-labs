/**
 * SO, 2017
 * Lab #6
 *
 * Task #4, lin
 *
 * Implementing 'cp' with mmap
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char *argv[])
{
	int fdin, fdout, rc;
	char *src, *dst;
	struct stat statbuf;

	DIE(argc != 3, "Usage: ./mycp <from_file> <to_file>");

	/* Open input and output files */
	fdin = open(argv[1], O_RDONLY);
	DIE(fdin == -1, "open fdin");

	/* Open/create the output file */
	fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0644);
	DIE(fdout == -1, "open fdout");

	/* Get file info */
	rc = fstat(fdin, &statbuf);
	if (rc < 0) {
		printf("[fstat] failed\n");
		return -1;
	}

	/* Truncate the output file to the input file size */
	rc = ftruncate(fdout, statbuf.st_size);
	if (rc < 0) {
		printf("[ftrunctate] failed\n");
		return -1;
	}

	/* mmap the input file*/
	src = mmap(NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0);
	if (src == MAP_FAILED) {
		printf("[mmap] failed\n");
		return -1;
	}

	/* mmap the output file*/
	dst = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);
	if (dst == MAP_FAILED) {
		printf("[mmap] failed\n");
		return -1;
	}
	/* Copy the input file to the output file */
	memcpy(dst, src, statbuf.st_size);

	/* Clean up */
	rc = munmap(src, statbuf.st_size);
	if (rc < 0) {
		printf("[munmap] failed\n");
		return -1;
	}

	rc = munmap(dst, statbuf.st_size);
	if (rc < 0) {
		printf("[munmap] failed\n");
		return -1;
	}

	/* Close files */
	rc = close(fdin);
	DIE(rc == -1, "close source");

	rc = close(fdout);
	DIE(rc == -1, "close destination");

	return 0;
}
