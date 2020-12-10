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
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>       
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define _XOPEN_SOURCE 500 
#define __USE_XOPEN_EXTENDED 500
#include <ftw.h>
#define MAXFD 20

#include "options.h"
#include "avl_tree.h"

#define MAXLINE 4096
#define SOLUTION_REGEX "^\w+(\.etap\d)?\.(tar\.gz|tar\.bz2|tar\.xz|zip)$"
#define INCORRECT_FILENAME_REGEX "^\w+(\.etap\d)?\.(tar\.gz|tar\.bz2|tar\.xz|zip)$"
#define INCORRECT_PART_REGEX "^\w+(\.etap\d)?\.(tar\.gz|tar\.bz2|tar\.xz|zip)$"

void *file_analyzer(void *void_args);

int walk(const char *name, const struct stat *s, int type, struct FTW *f);

void scan_dir ();

#endif