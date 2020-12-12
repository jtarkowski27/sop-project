#ifndef FILE_ANALYSIS_H
#define FILE_ANALYSIS_H

#include "regex_match.h"
#include "options.h"
// #include "avl_tree.h"

#define MAXLINE 4096

#define DEFAULT_STUDENT_COUNT 100

void *file_analysis(void *void_args);

void scan_dir(options_t *OPT, char *path);

#endif