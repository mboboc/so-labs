/**
 * SO, 2017
 * Lab #2, Operatii I/O simple
 *
 * Task #3, Linux
 *
 * cat/cp applications
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "xfile.h"

#define BUFSIZE		32

int main(int argc, char **argv)
{
	int fd_src, fd_dst, bytesRead = -1;
	char buffer[BUFSIZE];

	if (argc < 2 || argc > 3) {
		printf("Usage:\n\t%s source_file [destination_file]\n",
		       argv[0]);
		return 0;
	}

	/* open source file for reading */
	fd_src = open(argv[1], O_RDWR, 0644);
	if (fd_src < 0) {
		printf("[open failed]");
		return ERRO;
	}


	if (argc == 3) {
		/* redirect stdout to destination file */
		fd_dst = open(argv[2], O_RDWR | O_CREAT, 0644);
		if (fd_dst < 0) {
			printf("[open] failed\n");
			return ERRO;
		}

		/*
		rc = ftruncate(fd_dst, 0);
		if (rc < 0) {
			printf("[ftruncate] failed");
			return ERRO;
		}

		rc = lseek(fd_dst, 0, SEEK_SET);
		if (rc < 0) {
			printf("[lseek] failed");
			return ERRO;
		}
		*/

		dup2(fd_dst, STDIN_FILENO);
	} else {
		fd_dst = STDOUT_FILENO;
	}

	/* read from file and print to stdout
	 * use _only_ read and write functions
	 * for writing to output use write(STDOUT_FILENO, buffer, bytesRead);
	 */
	while (bytesRead != 0) {
		bytesRead = xread(fd_src, buffer, BUFSIZE);
		xwrite(fd_dst, buffer, bytesRead);
	}

	/* close file */
	close(fd_src);

	return 0;
}
