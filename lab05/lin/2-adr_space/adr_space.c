/**
  * SO, 2016
  * Lab #5
  *
  * Task #2, lin
  *
  * Variables in process address space viewed with objdump
  */
#include <stdlib.h>

int var_a;              /* .bss */
int var_b = 2;          /* .data */
char var_c[] = "so";    /* .data */

int main(void)
{
        int var_d;
        static int var_e;       /* .bss */
        char *var_f = "rulz";
        char *var_g = malloc(10);

        return 0;
}
