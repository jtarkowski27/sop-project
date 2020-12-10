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
#include "file_analysis.h"

int main(int argc, char **argv)
{
    options_t *OPT = (options_t *)malloc(sizeof(options_t));
    pthread_mutex_t mx_data_root = PTHREAD_MUTEX_INITIALIZER;
    pthread_t threads[3];

    OPT->argc = argc;
    OPT->argv = argv;
    
    OPT->PARTS_COUNT = 0;
    
    if (getcwd(OPT->PATH, MAX_ARG_LENGTH) == NULL) 
        ERR("getcwd");
    strncpy(OPT->CSV_FILENAME, DEFAULT_CSV_FILENAME, MAX_ARG_LENGTH + 1);
    strncpy(OPT->LOG_FILENAME, DEFAULT_LOG_FILENAME, MAX_ARG_LENGTH + 1);

    chandle_getopt(OPT);
    OPT->data_root = NULL;
    OPT->mx_data_root = &mx_data_root;
    if (pthread_create(&threads[0], NULL, file_analyzer, OPT)) ERR("pthread_create");

    free(OPT);
    exit(EXIT_SUCCESS);
}