#ifndef FILE_ANALYSIS_H
#define FILE_ANALYSIS_H

#include <unistd.h>
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include "options.h"
#include "avl_tree.h"

#define MAXLINE 4096
#define SOLUTION_REGEX "^\w+(\.etap\d)?\.(tar\.gz|tar\.bz2|tar\.xz|zip)$"
#define INCORRECT_FILENAME_REGEX "^\w+(\.etap\d)?\.(tar\.gz|tar\.bz2|tar\.xz|zip)$"
#define INCORRECT_PART_REGEX "^\w+(\.etap\d)?\.(tar\.gz|tar\.bz2|tar\.xz|zip)$"

void file_analizer(void *void_args);

#endif