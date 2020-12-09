#include "options.h"

void usage(char *pname)
{
    fprintf(stderr, "usage: %s -e <NUM>[1-9] -s <DD.MM.RRRR_GG:mm>\n\t      -k <DD.MM.RRRR_GG:mm> [-d <PATH>{=$(CWD)}]\n\t      [-n <NAME>{=\"results.csv\"}] [-b <NAME>{=\"errors.log\"}]\n", pname);
    exit(EXIT_FAILURE);
}

int missing_option(char *pname, char option)
{
    fprintf(stderr, "%s: missing required option -- '%c'\n", pname, option);
    return 0;
}

void invalid_argument(char *pname, char option)
{
    fprintf(stderr, "%s: invalid argument for option -- '%c'\n", pname, option);
    usage(pname);
}

void print_time(struct tm *tm_date)
{
    printf("date: ");
    printf("%d.", tm_date->tm_mday);
    printf("%d.", tm_date->tm_mon);
    printf("%d_", tm_date->tm_year);
    printf("%d:", tm_date->tm_hour);
    printf("%d\n", tm_date->tm_min);
}

void convert_date(options_t *OPT, struct tm **tm_date)
{
    char date[DATE_LENGTH + 1];
    char *day, *hour;


    strncpy(date, optarg, DATE_LENGTH + 1);
    
    if (!match(date, DATE_REGEX))
        invalid_argument(OPT->argv[0], OPT->c);

    day = strtok(date, UNDERSCORE_DELIM);
    hour = strtok(NULL, UNDERSCORE_DELIM);

    *tm_date = (struct tm *)malloc(sizeof(struct tm));

    if (!(*tm_date)) 
        ERR("malloc");

    (*tm_date)->tm_mday = strtol(strtok(day, DOT_DELIM), (char **)NULL, 0);
    (*tm_date)->tm_mon = strtol(strtok(NULL, DOT_DELIM), (char **)NULL, 0);
    (*tm_date)->tm_year = strtol(strtok(NULL, DOT_DELIM), (char **)NULL, 0);

    (*tm_date)->tm_hour = strtol(strtok(hour, COLON_DELIM), (char **)NULL, 0);
    (*tm_date)->tm_min = strtol(strtok(NULL, COLON_DELIM), (char **)NULL, 0);

    print_time(*tm_date);
}

void chandle_getopt(options_t *OPT)
{
    int err = 1;

    while (-1 != (OPT->c = getopt(OPT->argc, OPT->argv, SHORTOPTS)))
    {
        switch (OPT->c)
        {
            case 'e':
            option_e(OPT);
            break;
                
            case 's':
            option_s(OPT);
            break;

            case 'k':
            option_k(OPT);
            break;

            case 'd':
            option_d(OPT);
            break;

            case 'n':
            option_n(OPT);
            break;

            case 'b':
            option_b(OPT);
            break;

            default:
            usage(OPT->argv[0]);
            break;
        }
    }

    if (!(OPT->PARTS_COUNT)) 
        err = missing_option(OPT->argv[0], 'e');
    if (!(OPT->START_DATE)) 
        err = missing_option(OPT->argv[0], 's');
    if (!(OPT->FINAL_DATE)) 
        err = missing_option(OPT->argv[0], 'k');

    printf("PATH: %s\n", OPT->PATH);
    
    if (!err) 
        usage(OPT->argv[0]);
}

void option_e(options_t *OPT)
{
    OPT->PARTS_COUNT = atoi(optarg);
    if (OPT->PARTS_COUNT < 1 || OPT->PARTS_COUNT > 9)
        invalid_argument(OPT->argv[0], OPT->c);
}

void option_s(options_t *OPT)
{
    convert_date(OPT, &(OPT->START_DATE));
}

void option_k(options_t *OPT)
{
    convert_date(OPT, &(OPT->FINAL_DATE));
}

void option_d(options_t *OPT)
{
    strncpy(OPT->PATH, optarg, MAX_ARG_LENGTH + 1);
}

void option_n(options_t *OPT)
{
    strncpy(OPT->CSV_FILENAME, optarg, MAX_ARG_LENGTH + 1);
}

void option_b(options_t *OPT)
{
    strncpy(OPT->LOG_FILENAME, optarg, MAX_ARG_LENGTH + 1);
}
