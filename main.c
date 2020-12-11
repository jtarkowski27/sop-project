#include "regex_match.h"
#include "options.h"
#include "file_analysis.h"
#include "result_handler.h"
#include "mistake_handler.h"

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

int main(int argc, char **argv)
{
    options_t *OPT = (options_t *)malloc(sizeof(options_t));
    pthread_mutex_t mx_data = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mx_ent = PTHREAD_MUTEX_INITIALIZER;
    sigset_t *old_masks[3];
    int signals[] = {SIGUSR1, SIGCONT, SIGUSR2};

    errno = 0;

    OPT->argc = argc;
    OPT->argv = argv;
    
    OPT->PARTS_COUNT = 0;
    
    if (getcwd(OPT->PATH, MAX_ARG_LENGTH) == NULL) 
        ERR("getcwd");
    strncpy(OPT->CSV_FILENAME, DEFAULT_CSV_FILENAME, sizeof(OPT->CSV_FILENAME));
    strncpy(OPT->LOG_FILENAME, DEFAULT_LOG_FILENAME, sizeof(OPT->LOG_FILENAME));

    chandle_getopt(OPT);
    OPT->data = NULL;
    OPT->ent = NULL;
    OPT->mx_data = &mx_data;
    OPT->mx_ent = &mx_ent;
    OPT->new_mistake = OPT->work_finished = false;

    for (int i = 0; i < 2; i++)
    {
        sigset_t old_mask, new_mask;
        OPT->masks[i] = &new_mask;
        old_masks[i] = &old_mask;
    } 
    
    sigaddset(OPT->masks[0], SIGUSR1);
    sigaddset(OPT->masks[1], SIGCONT);

    for (int i = 0; i < 2; i++)
    {
        if (pthread_sigmask(SIG_BLOCK, OPT->masks[i], old_masks[i])) 
            ERR("SIG_BLOCK error");
    } 
    
    if (pthread_create(&OPT->threads[0], NULL, file_analysis, OPT)) 
        ERR("pthread_create");
    if (pthread_create(&OPT->threads[1], NULL, result_handler, OPT)) 
        ERR("pthread_create");
    if (pthread_create(&OPT->threads[2], NULL, mistake_handler, OPT)) 
        ERR("pthread_create");

    if(pthread_join(OPT->threads[0], NULL)) 
        ERR("Failed to join with a student thread!");
    if(pthread_join(OPT->threads[1], NULL)) 
        ERR("Failed to join with a student thread!");
    if(pthread_join(OPT->threads[2], NULL)) 
        ERR("Failed to join with a student thread!");

    for (int i = 0; i < 3; i++)
    {
        if (pthread_sigmask(SIG_UNBLOCK, OPT->masks[i], old_masks[i])) 
            ERR("SIG_BLOCK error");
    }

    free(OPT->data);
    free(OPT);
    exit(EXIT_SUCCESS);
}