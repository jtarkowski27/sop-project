#ifndef RESULT_HANDLER_H
#define RESULT_HANDLER_H

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
#include <fcntl.h>
#include <errno.h>


#include <ftw.h>
#define MAXFD 20

#include "options.h"

void *result_handler(void *void_args);

#endif