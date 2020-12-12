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

void convert_date(options_t *OPT, char *r_time_c, time_t *r_time)
{
    char date[MAX_ARG_LENGTH];
    struct tm tm = {0};
    strncpy(date, optarg, MAX_ARG_LENGTH);
    strncpy(r_time_c, optarg, MAX_ARG_LENGTH);

    if (!match(r_time_c, DATE_REGEX)) invalid_argument(OPT->argv[0], OPT->c);
	if (strptime(r_time_c, "%d.%m.%Y_%H:%M", &tm) == NULL) ERR("strptime");

	*r_time = mktime(&tm);
    
    printf("time: %ld\n", *r_time);
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

    if (!err)
        usage(OPT->argv[0]);
}

void option_e(options_t *OPT)
{
    OPT->PARTS_COUNT = strtol(optarg, NULL, 0);
    if (errno != 0) ERR("strtol");
    
    if (OPT->PARTS_COUNT < 1 || OPT->PARTS_COUNT > 9)
        invalid_argument(OPT->argv[0], OPT->c);
}

void option_s(options_t *OPT)
{
    convert_date(OPT, OPT->START_DATE_c, &(OPT->START_DATE));
}

void option_k(options_t *OPT)
{
    convert_date(OPT, OPT->FINAL_DATE_c, &(OPT->FINAL_DATE));
}

void option_d(options_t *OPT)
{
    strncpy(OPT->PATH, optarg, sizeof(OPT->PATH));
}

void option_n(options_t *OPT)
{
    strncpy(OPT->CSV_FILENAME, optarg, sizeof(OPT->CSV_FILENAME));
}

void option_b(options_t *OPT)
{
    strncpy(OPT->LOG_FILENAME, optarg, sizeof(OPT->LOG_FILENAME));
}
