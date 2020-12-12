#include "result_handler.h"

void *result_handler(void *void_args)
{
    options_t *OPT = void_args;
    FILE* f;
    int signo;

    if((f = fopen(OPT->CSV_FILENAME, "w+")) == NULL) ERR("fopen");

    while (1)
    {
        if(sigwait(OPT->masks[1], &signo)) 
            ERR("sigwait failed.");

        if (signo == SIGUSR1)
        {
            printf("thread[1] got a signal\n");
            student_t *stud = OPT->data->next;
            while (stud != NULL)
            {
                printf("ID: %s\n", stud->ID);
                fprintf(f, "%s, %d, ", stud->ID, stud->parts_send);
                fprintf(f, "%d, ", stud->minutes_late);
                for (int k = 0; k < stud->parts_send; k++)
                {
                    fprintf(f, "%d, ", stud->solving_time[k]);
                }
                fprintf(f, "\n");
                stud = stud->next;
            }
            break;
        }
    }

    if(fclose(f)) ERR("fclose");
    return NULL;
}