#ifndef REGEX_MATCH_H
#define REGEX_MATCH_H

#define _XOPEN_SOURCE  
#define __USE_XOPEN_EXTENDED 
#define _GNU_SOURCE

#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <regex.h>

#define DEFAULT_PATH "."
#define DEFAULT_CSV_FILENAME "results.csv"
#define DEFAULT_LOG_FILENAME "errors.log"

#define UNDERSCORE_DELIM "_"
#define DOT_DELIM "."
#define COLON_DELIM ":"

#define MAX_ARG_LENGTH 100
#define MSG_BUF_LENGTH 100
#define DATE_LENGTH 16


#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))

#define SOLUTION_REGEX "^\w+(.etap\d)?.(tar.gz|tar.bz2|tar.xz|zip)$"
#define INCORRECT_FILENAME_REGEX "^[a-zA-Z0-9_]+(.etap[1-1]).(tar.gz|tar.bz2|tar.xz|zip)$"
#define INCORRECT_PART_REGEX "^[a-zA-Z0-9_]+(.etap[0-9]+).(tar.gz|tar.bz2|tar.xz|zip)$"
#define CORRECT_EXTENSION_REGEX ".(tar.gz|tar.bz2|tar.xz|zip)$"

int match(const char *string, char *pattern);

#endif