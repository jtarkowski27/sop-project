#ifndef OPTIONS_H
#define OPTIONS_H

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

#define SHORTOPTS "e:s:k:d::n::b::"

// #define DEFAULTPATH "e:s:k:d::n::b::"

typedef struct options 
{
    int argc; 
    char **argv;

    int PARTS_COUNT;
    struct tm *START_DATE;
    struct tm *FINAL_DATE;

    char *PATH;
    char *CSV_FILENAME;
    char *LOG_FILENAME;
} options_t;


void option_e(options_t *options);
void option_s(options_t *options);
void option_k(options_t *options);
void option_d(options_t *options);
void option_n(options_t *options);
void option_b(options_t *options);

void usage(char *pname);

void chandle_getopt(options_t *options);

#endif