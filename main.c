#include <unistd.h>
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "options.h"

int main(int argc, char **argv)
{
    options_t *options = (options_t *)malloc(sizeof(options_t));
    options->argc = argc;
    options->argv = argv;
    
    options->PARTS_COUNT = 0;
    options->START_DATE = NULL;
    options->FINAL_DATE = NULL;

    chandle_getopt(options);

    free(options);
    exit(EXIT_SUCCESS);
}