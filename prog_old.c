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

#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     exit(EXIT_FAILURE))

#define DEFAULT_PATH "."
#define DEFAULT_CSV_FILENAME "results.csv"
#define DEFAULT_LOG_FILENAME "errors.log"

#define UNDERSCORE_DELIM "_"
#define DOT_DELIM "."
#define COLON_DELIM ":"

#define MAX_ARG_LENGTH 100
#define MSG_BUF_LENGTH 100
#define DATE_LENGTH 16

#define SOLUTION_REGEX "^\w+(.etap\d)?.(tar.gz|tar.bz2|tar.xz|zip)$"
#define INCORRECT_FILENAME_REGEX "^[a-zA-Z0-9_]+(.etap[1-1]).(tar.gz|tar.bz2|tar.xz|zip)$"
#define INCORRECT_PART_REGEX "^[a-zA-Z0-9_]+(.etap[0-9]+).(tar.gz|tar.bz2|tar.xz|zip)$"
#define CORRECT_EXTENSION_REGEX ".(tar.gz|tar.bz2|tar.xz|zip)$"

#define SHORTOPTS "e:s:k:d:n:b:"
#define DATE_REGEX "^[0-9]{2}.[0-9]{2}.[0-9]{4}_[0-9]{2}:[0-9]{2}$"
#define MAX_PARTS_LENGTH 9
#define THREADS_COUNT 3


/* MATCHING REGEX */

int match(const char *string, char *pattern);
void usage(char *pname);
int missing_option(char *pname, char option);
void invalid_argument(char *pname, char option);


/* OPTIONS */

typedef struct student 
{
    char ID[MAX_ARG_LENGTH];
    int parts_send;
    int minutes_late;
    int solving_time[MAX_PARTS_LENGTH];
    struct student *next;
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
    struct dirent *ent;

    pthread_t threads[THREADS_COUNT];
    sigset_t *result_tread_mask;
    bool new_mistake;
    bool work_finished;
} options_t;

void chandle_getopt(options_t *OPT);
void convert_date(options_t *OPT, char *r_time_c, time_t *r_time);


/* FILE ANALYSIS */

#define MAXLINE 4096
#define DEFAULT_STUDENT_COUNT 10

void *file_analysis(void *void_args);
void init_data(options_t *OPT);
void pathcat(char *path, options_t *OPT);
void scan_dir(options_t *OPT, char *path);
void add_part(student_t *stud, int *part, time_t *file_t, time_t *last_t, time_t *final_t);

void append_student(student_t **stud);
void incorrect_file(options_t * OPT, struct dirent *ent);
void free_list(student_t *root);


/* RESULT HANDLING */

void *result_handler(void *void_args);

/* MISTAKES HANDLING */

void *mistake_handler(void *void_args);


int main(int argc, char **argv)
{
    int i;
    options_t *OPT = (options_t *)malloc(sizeof(options_t));
    
    void *(*start_routines[THREADS_COUNT])(void *) = {file_analysis, result_handler, mistake_handler};

    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    if (pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask)) 
        ERR("SIG_BLOCK error");
    OPT->result_tread_mask = &new_mask;

    OPT->argc = argc;
    OPT->argv = argv;
    OPT->PARTS_COUNT = 0;
    OPT->data = NULL;
    OPT->ent = NULL;
    OPT->new_mistake = false;
    OPT->work_finished = false;
    
    if (getcwd(OPT->PATH, MAX_ARG_LENGTH) == NULL) 
        ERR("getcwd");
    strncpy(OPT->CSV_FILENAME, DEFAULT_CSV_FILENAME, sizeof(OPT->CSV_FILENAME));
    strncpy(OPT->LOG_FILENAME, DEFAULT_LOG_FILENAME, sizeof(OPT->LOG_FILENAME));

    chandle_getopt(OPT);
    
    for (i = 0; i < THREADS_COUNT; i++)
    {
        if (pthread_create(&OPT->threads[i], NULL, start_routines[i], OPT)) 
            ERR("pthread_create");
    }

    for (i = 0; i < THREADS_COUNT; i++)
    {
        if(pthread_join(OPT->threads[i], NULL)) 
            ERR("Failed to join with a thread!");
    }

    if (pthread_sigmask(SIG_UNBLOCK, &new_mask, &old_mask)) 
        ERR("SIG_BLOCK error");

    free_list(OPT->data);
    free(OPT);
    exit(EXIT_SUCCESS);
}


/* HANDLING REGEX */

int match(const char *string, char *pattern)
{
    int status;
    regex_t re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB))
        ERR("regcomp");

    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);

    if (status)
        return 0;

    return 1;
}

void usage(char *pname)
{
    fprintf(stderr, "usage: %s -e <NUM>[1-9] -s <DD.MM.RRRR_GG:mm>\n\t      -k <DD.MM.RRRR_GG:mm> [-d <PATH>{=$(CWD)}]\n\t      [-n <NAME>{=\"results.csv\"}] [-b <NAME>{=\"errors.log\"}]\n", pname);
    exit(EXIT_FAILURE);
}

int missing_option(char *pname, char option)
{
    fprintf(stderr, "%s: missing required option -- '%c'\n", pname, option);
    return 0;
}

void invalid_argument(char *pname, char option)
{
    fprintf(stderr, "%s: invalid argument for option -- '%c'\n", pname, option);
    usage(pname);
}


/* OPTIONS */

void convert_date(options_t *OPT, char *r_time_c, time_t *r_time)
{
    char date[MAX_ARG_LENGTH];
    struct tm tm = {0};
    strncpy(date, optarg, MAX_ARG_LENGTH);
    strncpy(r_time_c, optarg, MAX_ARG_LENGTH);

    if (!match(r_time_c, DATE_REGEX)) invalid_argument(OPT->argv[0], OPT->c);
	if (strptime(r_time_c, "%d.%m.%Y_%H:%M", &tm) == NULL) ERR("strptime");

	*r_time = mktime(&tm);
}

void chandle_getopt(options_t *OPT)
{
    int err = 1;

    while (-1 != (OPT->c = getopt(OPT->argc, OPT->argv, SHORTOPTS)))
    {
        switch (OPT->c)
        {
        case 'e':
            OPT->PARTS_COUNT = strtol(optarg, NULL, 0);
            if (errno != 0) ERR("strtol");
            
            if (OPT->PARTS_COUNT < 1 || OPT->PARTS_COUNT > 9)
                invalid_argument(OPT->argv[0], OPT->c);
            break;

        case 's':
            convert_date(OPT, OPT->START_DATE_c, &(OPT->START_DATE));
            break;

        case 'k':
            convert_date(OPT, OPT->FINAL_DATE_c, &(OPT->FINAL_DATE));
            break;

        case 'd':
            strncpy(OPT->PATH, optarg, sizeof(OPT->PATH));
            break;

        case 'n':
            strncpy(OPT->CSV_FILENAME, optarg, sizeof(OPT->CSV_FILENAME));
            break;

        case 'b':
            strncpy(OPT->LOG_FILENAME, optarg, sizeof(OPT->LOG_FILENAME));
            break;

        default:
            usage(OPT->argv[0]);
            break;
        }
    }

    if (!(OPT->PARTS_COUNT))
        err = missing_option(OPT->argv[0], 'e');
    if (!(OPT->START_DATE))
        err = missing_option(OPT->argv[0], 's');
    if (!(OPT->FINAL_DATE))
        err = missing_option(OPT->argv[0], 'k');

    if (!err)
        usage(OPT->argv[0]);
}


/* FILE ANALYSIS */

void *file_analysis(void *void_args)
{
	char path[MAX_ARG_LENGTH] = "";
    options_t *OPT = void_args;

	init_data(OPT);
	
	pathcat(path, OPT);
	if(chdir(path)) ERR("chdir");

	scan_dir(OPT, path);
	OPT->work_finished = true;

	pthread_kill(OPT->threads[1], SIGUSR1);

    return NULL;
}

void init_data(options_t *OPT)
{
	OPT->data = (student_t *)calloc(1, sizeof(student_t));
	if (!(OPT->data)) ERR("calloc");
	strncpy(OPT->data->ID, "", sizeof(OPT->data->ID));
}

void pathcat(char *path, options_t *OPT)
{
	if (getcwd(path, MAX_ARG_LENGTH) == NULL) 
        ERR("getcwd");

	strncat(path, "/", 2);
	strncat(path, OPT->PATH, MAX_ARG_LENGTH);
}

void scan_dir(options_t *OPT, char *path)
{
	// Zmienne podrzebne 
	DIR *dir;
	struct dirent *ent;
	struct stat filestat;

	// Zmienne związane z czasem
	time_t start_t;
	time_t last_t = start_t = OPT->START_DATE;
	time_t final_t = OPT->FINAL_DATE;
	time_t file_t;

	// Zmienne związane z nazwami plików
	char ID_buffer[MAX_ARG_LENGTH] = {0};
	char *ID = NULL;
	char regex[MAX_ARG_LENGTH] = "";

	// Zmienne związane z danyi studentów
	int part_n = 0;
	student_t *stud = OPT->data;

	strncpy(regex, INCORRECT_FILENAME_REGEX, sizeof(regex));
	regex[23] = '0' + OPT->PARTS_COUNT;
	
	if (!(dir = opendir(path))) ERR("opendir");

	errno = 0;
	while ((ent = readdir(dir)) != NULL && errno == 0)
	{
		if (match(ent->d_name, regex) == 1)
		{
			if (lstat(ent->d_name, &filestat))
				ERR("lstat");

			file_t = filestat.st_mtime;

			strncpy(ID_buffer, ent->d_name, sizeof(ID_buffer));
			if ((ID = strtok(ID_buffer, DOT_DELIM)) == NULL)
				ERR("strtok");

			if (strncmp(ID, stud->ID, MAX_ARG_LENGTH) == 0)
				add_part(stud, &part_n, &file_t, &last_t, &final_t);
			else
			{
				last_t = start_t;
				part_n = 0;
				append_student(&stud);
				strncpy(stud->ID, ID, sizeof(stud->ID));
				add_part(stud, &part_n, &file_t, &last_t, &final_t);
			}
		}
		else
		{
			incorrect_file(OPT, ent);
		}
	}

	if (errno != 0) ERR("readdir");
	if (closedir(dir)) ERR("closedir");
}

void add_part(student_t *stud, int *part, time_t *file_t, time_t *last_t, time_t *final_t)
{
	stud->parts_send++;
	stud->solving_time[(*part)++] = (*file_t - *last_t) / 60;
	stud->minutes_late = *final_t > *file_t ? 0 : (*file_t - *final_t) / 60;
	*last_t = *file_t;
}

void append_student(student_t **stud)
{
	(*stud)->next = (student_t *)calloc(1, sizeof(student_t));
	if ((*stud)->next == NULL)
		ERR("malloc");
	(*stud) = (*stud)->next;
	(*stud)->next = NULL;
	(*stud)->parts_send = 0;
}

void incorrect_file(options_t * OPT, struct dirent *ent)
{
	OPT->ent = ent;
	OPT->new_mistake = true;	// Informacja dla wątku [2] o nowym zauważonym błędzie

	// Wątek [0] oczekuje na informację zwrotną od wątku [2] o zapisaniu błędu
	while (1)
	{
        if (OPT->new_mistake == false)
			break;
	}
}

void free_list(student_t *head)
{
    student_t *curr;
	while ((curr = head) != NULL) 
    { 
        head = head->next;          
        free(curr);                
    }
}

/* RESULT HANDLING */

void *result_handler(void *void_args)
{
    options_t *OPT = void_args;
    FILE* f;
    int signo;
    student_t *stud;

    if((f = fopen(OPT->CSV_FILENAME, "w+")) == NULL) ERR("fopen");

    while (1)
    {
        if(sigwait(OPT->result_tread_mask, &signo)) 
            ERR("sigwait failed.");

        if (signo == SIGUSR1)
        {
            stud = OPT->data->next;
            printf("thread[1] got a signal\n");
            while (stud != NULL)
            {
                fprintf(f, "%s, %d, ", stud->ID, stud->parts_send);
                fprintf(f, "%d, ", stud->minutes_late);
                for (int k = 0; k < stud->parts_send; k++)
                {
                    fprintf(f, "%d, ", stud->solving_time[k]);
                }
                fprintf(f, "\n");
                stud = stud->next;
            }
            break;
        }
    }

    if(fclose(f)) ERR("fclose");
    return NULL;
}


/* MISTAKES HANDLING */

void *mistake_handler(void *void_args)
{
    options_t *OPT = void_args;
    FILE* f;

    time_t t = time(NULL);
	struct tm tm = *localtime(&t);

    if((f = fopen(OPT->LOG_FILENAME, "w+")) == NULL) ERR("fopen");

    while (1)
    {
        if (OPT->new_mistake == true)
        {
            if (match(OPT->ent->d_name, INCORRECT_PART_REGEX) == 1)
            {
                fprintf(f, "[%02d.%02d.%d_%02d:%02d] ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
                fprintf(f, "(%s) ", OPT->ent->d_name);
                fprintf(f, "Niepoprawny numer etapu\n");
            }
            else if (match(OPT->ent->d_name, CORRECT_EXTENSION_REGEX) == 1)
            {
                fprintf(f, "[%02d.%02d.%d_%02d:%02d] ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
                fprintf(f, "(%s) ", OPT->ent->d_name);
                fprintf(f, "Niepoprawna nazwa pliku\n");
            }

            OPT->new_mistake = false;
        }

        if (OPT->work_finished == true)
        {
            printf("End of thread[2]\n");
            break;
        }
    }

    if(fclose(f)) ERR("fclose");
    return NULL;
}