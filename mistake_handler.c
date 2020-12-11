#include "mistake_handler.h"

void *mistake_handler(void *void_args)
{
    options_t *OPT = void_args;
    FILE* f;

    time_t t = time(NULL);
	struct tm tm = *localtime(&t);
    
    int signo = 0;

    if((f = fopen(OPT->LOG_FILENAME, "w+")) == NULL) ERR("fopen");

    while (1)
    {
        if (OPT->new_mistake == true)
        {
            OPT->new_mistake = false;

            if (match(OPT->ent->d_name, INCORRECT_PART_REGEX) == 1)
            {
                fprintf(f, "[%02d.%02d.%d_%02d:%02d] ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
                fprintf(f, "(%s) ", OPT->ent->d_name);
                fprintf(f, "Niepoprawny numer etapu\n");

                fprintf(stdout, "[%02d.%02d.%d_%02d:%02d] ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
                fprintf(stdout, "(%s) ", OPT->ent->d_name);
                fprintf(stdout, "Niepoprawny numer etapu\n");
            }
            else if (match(OPT->ent->d_name, CORRECT_EXTENSION_REGEX) == 1)
            {
                fprintf(f, "[%02d.%02d.%d_%02d:%02d] ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
                fprintf(f, "(%s) ", OPT->ent->d_name);
                fprintf(f, "Niepoprawna nazwa pliku\n");

                fprintf(stdout, "[%02d.%02d.%d_%02d:%02d] ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
                fprintf(stdout, "(%s) ", OPT->ent->d_name);
                fprintf(stdout, "Niepoprawna nazwa pliku\n");
            }

            pthread_kill(OPT->threads[0], SIGCONT);
        }

        if (OPT->work_finished == true)
        {
            printf("End of thread[2]\n");
            break;
        }
    }

    if(fclose(f)) ERR("fclose");
    return NULL;
}