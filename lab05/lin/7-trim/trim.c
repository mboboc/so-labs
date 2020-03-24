/**
  * SO, 2016
  * Lab #5
  *
  * Task #7, lin
  *
  * Use of mcheck
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

char *first_name = "  Harry";
char *last_name  = "    Potter";

static char *trim(char *s)
{

	char *a = malloc(strlen(s)+1);
	char *p = a;

	strcpy(p, s);

	while (*p == ' ')
		p++;

	strcpy(s, p);
	free(a);

	return s;
}

int main(void)
{

	printf("%s %s is learning SO!\n",
			trim(first_name), trim(last_name));

	return 0;
}