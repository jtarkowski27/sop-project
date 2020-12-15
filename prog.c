#define __USE_XOPEN_EXTENDED 
#define _GNU_SOURCE 500
#define _XOPEN_SOURCE 500
#include <ftw.h>
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
#include <stdint.h>

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

#define PART_INDEX_IN_STRING 4

#define MAXFD 20


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
    time_t timestamps[MAX_PARTS_LENGTH + 1];
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

    // pthread_mutex_t *mx_path;

    student_t *data;
    struct dirent *ent;

    pthread_t threads[THREADS_COUNT];
    sigset_t *result_tread_mask;

    bool new_mistake;
    bool work_finished;

    char *wrong_filename;
} options_t;

void chandle_getopt(options_t *OPT);
void convert_date(options_t *OPT, char *r_time_c, time_t *r_time);


/* FILE ANALYSIS */

#define MAXLINE 4096
#define DEFAULT_STUDENT_COUNT 10

static options_t *OPT_global;

void *file_scanner(void *void_args);
void init_data(options_t *OPT);
void pathcat(char *path, options_t *OPT);
void scan_dir(options_t *OPT, char *path);
void add_part(student_t *stud, time_t *file_t, time_t *last_t, time_t *final_t);

void append_student(student_t **stud, time_t start_t);
void update_student(student_t *stud, time_t part_t, time_t final_t, int part);
void incorrect_file(options_t * OPT);
void free_list(student_t *root);


/* RESULT HANDLING */

void *result_writer(void *void_args);

/* MISTAKES HANDLING */

#define INCORRECT_PART_MESSAGE "Niepoprawny numer etapu\n"
#define INCORRECT_FILENAME_MESSAGE "Niepoprawna nazwa pliku\n"
#define LOG_MESSAGE_LENGTH 24

void *mistake_writer(void *void_args);


int main(int argc, char **argv)
{
    int i;
    options_t *OPT = (options_t *)malloc(sizeof(options_t));
    char path[MAX_ARG_LENGTH * 2] = "";
	char cwd[MAX_ARG_LENGTH] = "";
    
    void *(*start_routines[THREADS_COUNT])(void *) = {file_scanner, result_writer, mistake_writer};

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


    OPT_global = OPT;

    chandle_getopt(OPT);
    
    
    if (getcwd(cwd, MAX_ARG_LENGTH) == NULL) 
        ERR("getcwd");
    strncpy(path, cwd, MAX_ARG_LENGTH * 2);
	init_data(OPT);
	pathcat(path, OPT);
	if(chdir(path)) ERR("chdir");

    
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
    
	if(chdir(cwd)) ERR("chdir");

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

student_t * search(student_t *head, char *ID, student_t **prev)
{
    student_t *curr;
    int c;
	curr = head->next;
    *prev = head;

	while (curr != NULL && (c = strncmp(ID, curr->ID, MAX_ARG_LENGTH)) > 0)
	{    
        // printf("strncmp(%s,%s)\n", ID, curr->ID);
		*prev = curr;
		curr = curr->next;
	}
	return c == 0 ? curr : NULL;
}

void print_reverse(student_t* head)
{
    if (head == NULL)
       return;
    printf("%s->", head->ID);
    print_reverse(head->next);
}

int walk(const char *name, const struct stat *filestat, int type, struct FTW *f)
{
    if (type != FTW_F)
        return 0;
        
    options_t *OPT = OPT_global;

	time_t start_t = OPT->START_DATE;
	time_t final_t = OPT->FINAL_DATE;
	time_t file_t;

    int i = 0;

	// Zmienne związane z nazwami plików
	static char ID_buffer[MAX_ARG_LENGTH] = "";
	static char *ID = NULL;
    static char *part;
	static char regex[MAX_ARG_LENGTH] = "";

	// Zmienne związane z danymi studentów
	student_t *stud = OPT->data;
	student_t *prev;

    char *filename = (char *)name + f->base;

	strncpy(regex, INCORRECT_FILENAME_REGEX, sizeof(regex));
	regex[23] = '0' + OPT->PARTS_COUNT;


	errno = 0;
    if (match(filename, regex) == 1)
    {
        // printf("%-5s %s\n", (type == FTW_F) ? "f" : "other", filename);
        file_t = filestat->st_mtime;

        strncpy(ID_buffer, filename, sizeof(ID_buffer));
        if ((ID = strtok(ID_buffer, DOT_DELIM)) == NULL)
            ERR("strtok");

        if ((part = strtok(NULL, DOT_DELIM)) == NULL)
            ERR("strtok");
            
        i = part[PART_INDEX_IN_STRING] - '0';
    
        if (NULL == (stud = search(OPT->data, ID, &prev)))
        {
            append_student(&prev, start_t);
            stud = prev;
            strncpy(stud->ID, ID, sizeof(stud->ID));
        }

        update_student(stud, file_t, final_t, i);
    }
    else
    {
        OPT->wrong_filename = filename;
        incorrect_file(OPT);
    }

    return 0;
}

void *file_scanner(void *void_args)
{
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    options_t *OPT = void_args;

    if(nftw(".", walk, MAXFD, FTW_PHYS) != 0) printf("%s: brak dostępu\n", OPT->PATH);

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
	strncat(path, "/", 2);
	strncat(path, OPT->PATH, MAX_ARG_LENGTH * 2);
}

void append_student(student_t **stud, time_t start_t)
{
    student_t *temp = (*stud)->next;
	(*stud)->next = (student_t *)calloc(1, sizeof(student_t));
	if ((*stud)->next == NULL)
		ERR("malloc");
	(*stud) = (*stud)->next;
	(*stud)->next = temp;
	(*stud)->parts_send = 1;
	(*stud)->timestamps[0] = start_t;
}

void update_student(student_t *stud, time_t part_t, time_t final_t, int part)
{
	stud->timestamps[part] = part_t;
	stud->parts_send++;
	stud->minutes_late = final_t > part_t ? 0 : (part_t - final_t) / 60;
}

void incorrect_file(options_t * OPT)
{
    if (match(OPT->wrong_filename, CORRECT_EXTENSION_REGEX) == 1)
    {
        OPT->new_mistake = true;	// Informacja dla wątku [2] o nowym zauważonym błędzie

        // Wątek [0] oczekuje na informację zwrotną od wątku [2] o zapisaniu błędu
        while (1)
        {
            if (OPT->new_mistake == false)
                break;
        }
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

void csv_cleanup_handler(void *void_args)
{
    int *f = void_args;
    close(*f);
}

void *result_writer(void *void_args)
{
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    options_t *OPT = void_args;
    int f;
    int signo;
    student_t *stud;
    char str_buffer[MAX_ARG_LENGTH * 2] = "";
    time_t curr;

    if((f = open(OPT->CSV_FILENAME, O_CREAT|O_RDWR|O_TRUNC, 0644)) == -1) 
        ERR("open");

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
                write(f, stud->ID, strlen(stud->ID)); 
                sprintf(str_buffer, ", %d, %d", stud->parts_send - 1, stud->minutes_late);
                write(f, str_buffer, strlen(str_buffer));

                curr = OPT->START_DATE;
                for (int k = 1; k < stud->parts_send; k++)
                {
                    sprintf(str_buffer, ", %ld", (stud->timestamps[k] - curr) / 60);
                    write(f, str_buffer, strlen(str_buffer));
                    curr = stud->timestamps[k];
                }
                write(f, "\n", 1);
                stud = stud->next;
            }
            break;
        }
    }

    if(-1 == close(f)) 
        ERR("close");
    return NULL;
}


/* MISTAKES HANDLING */

void log_cleanup_handler(void *void_args)
{
    int *f = void_args;
    close(*f);
}

void *mistake_writer(void *void_args)
{
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    options_t *OPT = void_args;
    int f;

    time_t t = time(NULL);
	struct tm *curr_t;
    char time_buffer[DATE_LENGTH + 1];
    

    if((f = open(OPT->LOG_FILENAME, O_CREAT|O_RDWR|O_TRUNC, 0644)) == -1) 
        ERR("open");

    // pthread_cleanup_push(log_cleanup_handler, &f);
    while (1)
    {
        if (OPT->new_mistake == true)
        {
            curr_t = localtime(&t);
            strftime(time_buffer, sizeof(time_buffer), "%d.%m.%Y_%H:%M", curr_t);

            write(f, "[", 1); 
            write(f, time_buffer, strlen(time_buffer)); 
            write(f, "] (", 3); 
            write(f, OPT->wrong_filename, strlen(OPT->wrong_filename)); 
            write(f, ") ", 2); 

            if (match(OPT->wrong_filename, INCORRECT_PART_REGEX) == 1)
                write(f, INCORRECT_PART_MESSAGE, LOG_MESSAGE_LENGTH);
            else
                write(f, INCORRECT_FILENAME_MESSAGE, LOG_MESSAGE_LENGTH);

            OPT->new_mistake = false;
        }

        if (OPT->work_finished == true)
        {
            break;
        }
    }

    if(-1 == close(f)) ERR("close");
    
    // pthread_cleanup_pop(1);
    return NULL;
}