#include <regex.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>
#include <errno.h>

#ifndef REGEX_MATCH_H
#define REGEX_MATCH_H

#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))

int match(const char *string, char *pattern);

#endif