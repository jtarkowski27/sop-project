#include "options.h"

void usage(char *pname)
{
    fprintf(stderr, "usage: %s -e <NUM>=[1,9] -s <DD.MM.RRRR_GG:mm>\n\t      -k <DD.MM.RRRR_GG:mm> [-d <PATH>{=$(CWD)}]\n\t      [-n <NAME>{=\"results.csv\"}] [-b <NAME>{=\"errors.log\"}]\n", pname);
    exit(EXIT_FAILURE);
}

int missing_option(char *pname, char option)
{
    fprintf(stderr, "%s: missing required option -- '%c'\n", pname, option);
    return 0;
}

void chandle_getopt(options_t *options)
{
    int c;
    int err = 1;

    while ((c = getopt(options->argc, options->argv, SHORTOPTS)) != -1)
    {
        switch (c)
        {
            case 'e':
            option_e(options);
            break;
                
            case 's':
            option_s(options);
            break;

            case 'k':
            option_k(options);
            break;

            case 'd':
            option_d(options);
            break;

            case 'n':
            option_n(options);
            break;

            case 'b':
            option_b(options);
            break;

            case ':':
            usage(options->argv[0]);
            break;

            case '?':
            usage(options->argv[0]);
            break;
        }
    }

    if (!(options->PARTS_COUNT)) 
        err = missing_option(options->argv[0], 'e');
    if (!(options->START_DATE)) 
        err = missing_option(options->argv[0], 's');
    if (!(options->FINAL_DATE)) 
        err = missing_option(options->argv[0], 'k');
    
    if (!err) usage(options->argv[0]);
}

void option_e(options_t *options)
{
    options->PARTS_COUNT = atoi(optarg);
    if (options->PARTS_COUNT < 1 || options->PARTS_COUNT > 9)
    {
        usage(options->argv[0]);
    }
}

void option_s(options_t *options)
{

}

void option_k(options_t *options)
{

}

void option_d(options_t *options)
{

}

void option_n(options_t *options)
{

}

void option_b(options_t *options)
{

}
