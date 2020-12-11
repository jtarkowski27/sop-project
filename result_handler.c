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
            for (int j = 1; j <= OPT->data_length; j++)
            {
                fprintf(f, "%s, %d, ", OPT->data[j].ID, OPT->data[j].parts_send);
                fprintf(f, "%d, ", OPT->data[j].minutes_late);
                for (int k = 0; k < OPT->data[j].parts_send; k++)
                {
                    fprintf(f, "%d, ", OPT->data[j].solving_time[k]);
                }
                fprintf(f, "\n");
            }
            break;
        }
    }

    if(fclose(f)) ERR("fclose");
    return NULL;
}