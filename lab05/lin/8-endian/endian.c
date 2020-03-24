/**
  * SO, 2016
  * Lab #5
  *
  * Task #8, lin
  *
  * Endianess
  */

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

int main(void)
{
  int i;
  unsigned int n = 0xDEADBEEF;
  unsigned char *w = (unsigned char *)&n;

  /* use w to show all bytes of n in order */
  for (i = 0; i < (int)sizeof(n); i++)
    printf("%p %.2x\n", &w[i], w[i]);

  return 0;
}

