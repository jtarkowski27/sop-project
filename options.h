
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef OPTIONS_H
#define OPTIONS_H

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "regex_match.h"

#define SHORTOPTS "e:s:k:d::n::b::"

// #define DEFAULTPATH "e:s:k:d::n::b::"
#define DATE_REGEX "^[0-9]{2}.[0-9]{2}.[0-9]{4}_[0-9]{2}:[0-9]{2}$"

#define MAX_ARG_LENGTH 100
#define MSG_BUF_LENGTH 100
#define DATE_LENGTH 16

#define DEFAULT_PATH "."
#define DEFAULT_CSV_FILENAME "results.csv"
#define DEFAULT_LOG_FILENAME "errors.log"

#define UNDERSCORE_DELIM "_"
#define DOT_DELIM "."
#define COLON_DELIM ":"


#define CASE(N) case N: 

typedef struct options 
{
    int argc; 
    char **argv;
    int c;

    int PARTS_COUNT;
    struct tm *START_DATE;
    struct tm *FINAL_DATE;

    char PATH[MAX_ARG_LENGTH + 1];
    char CSV_FILENAME[MAX_ARG_LENGTH + 1];
    char LOG_FILENAME[MAX_ARG_LENGTH + 1];
} options_t;

void option_e(options_t *OPT);
void option_s(options_t *OPT);
void option_k(options_t *OPT);
void option_d(options_t *OPT);
void option_n(options_t *OPT);
void option_b(options_t *OPT);

void usage(char *pname);

void chandle_getopt(options_t *OPT);

int missing_option(char *pname, char option);

void invalid_argument(char *pname, char option);

void convert_date(options_t *OPT, struct tm **tm_date);

#endif