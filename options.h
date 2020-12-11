
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef OPTIONS_H
#define OPTIONS_H

#include <getopt.h>
#include <regex.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "regex_match.h"

#define SHORTOPTS "e:s:k:d:n:b:"
#define DATE_REGEX "^[0-9]{2}.[0-9]{2}.[0-9]{4}_[0-9]{2}:[0-9]{2}$"

#define CASE(N) case N: 

typedef struct student 
{
    char ID[MAX_ARG_LENGTH];
    int parts_send;
    int minutes_late;
    int solving_time[9];
} student_t;

typedef struct mistake 
{
    struct tm *date;
    char filename[MAX_ARG_LENGTH];
    int type;
} mistake_t;


typedef struct options 
{
    int argc; 
    char **argv;
    int c;

    int PARTS_COUNT;
    time_t START_DATE;
    time_t FINAL_DATE;

    char START_DATE_c[DATE_LENGTH + 1];
    char FINAL_DATE_c[DATE_LENGTH + 1];

    char PATH[MAX_ARG_LENGTH + 1];
    char CSV_FILENAME[MAX_ARG_LENGTH + 1];
    char LOG_FILENAME[MAX_ARG_LENGTH + 1];

    student_t *data;
    pthread_mutex_t *mx_data;

    struct dirent *ent;
    pthread_mutex_t *mx_ent;
    int data_length;

    pthread_t threads[3];
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

// void convert_date(options_t *OPT, struct tm **tm_date);

void convert_date(options_t *OPT, char *r_time_c, time_t *r_time);

#endif