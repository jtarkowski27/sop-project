#ifndef FILE_ANALYSIS_H

#define _XOPEN_SOURCE 500 
#define __USE_XOPEN_EXTENDED 500
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

#include <ftw.h>
#define MAXFD 20

#include "options.h"
// #include "avl_tree.h"

#define MAXLINE 4096

#define DEFAULT_STUDENT_COUNT 100

void *file_analysis(void *void_args);

int walk(const char *name, const struct stat *s, int type, struct FTW *f);

void scan_dir(options_t *OPT, char *path);

#endif