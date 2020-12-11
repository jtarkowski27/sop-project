#include "file_analysis.h"


int walk(const char *name, const struct stat *s, int type, struct FTW *f)
{
    printf("lol");
    return 0;
}

void pathcat(char *path, options_t *OPT)
{
	if (getcwd(path, MAX_ARG_LENGTH) == NULL) 
        ERR("getcwd");

	strncat(path, "/", 2);
	strncat(path, OPT->PATH, MAX_ARG_LENGTH);
}

void reallocate_data(student_t *data, int new_length)
{
	data = (student_t *)realloc(data, sizeof(student_t) * new_length);
	if (data == NULL)
		ERR("realloc");
}

void add_part(student_t *stud, int *part, time_t *mtime, time_t *last_t, time_t *final_t)
{
	stud->parts_send++;
	stud->solving_time[(*part)++] = (*mtime - *last_t) / 60;
	stud->minutes_late = *final_t > *mtime ? 0 : (*mtime - *final_t) / 60;
	*last_t = *mtime;
}

void init_data(options_t *OPT)
{
	OPT->data_length = 0;
	OPT->data = (student_t *)malloc(sizeof(student_t) * DEFAULT_STUDENT_COUNT);
	if (!(OPT->data)) ERR("malloc");
}

void correct_filename(options_t * OPT, struct dirent *ent, time_t *start, time_t *last, time_t *final)
{
	static char ID_buffer[MAX_ARG_LENGTH];
	static struct stat filestat;
	static int i = 0;
	static int reserved_size = DEFAULT_STUDENT_COUNT;
	static char *ID;
	static int last_part = 0;

	if (lstat(ent->d_name, &filestat))
		ERR("lstat");

	time_t mtime = filestat.st_mtime;

	strncpy(ID_buffer, ent->d_name, sizeof(ID_buffer));
	if ((ID = strtok(ID_buffer, DOT_DELIM)) == NULL)
		ERR("strtok");

	if (strncmp(ID, OPT->data[i].ID, MAX_ARG_LENGTH) == 0)
		add_part(&(OPT->data[i]), &last_part, &mtime, last, final);
	else
	{
		OPT->data_length++;
		last_part = 0;
		*last = *start;

		if (++i >= reserved_size)
			reallocate_data(OPT->data, (reserved_size *= 2));

		strncpy(OPT->data[i].ID, ID, sizeof(OPT->data[i].ID));

		add_part(&(OPT->data[i]), &last_part, &mtime, last, final);
	}
}

void incorrect_filename(options_t * OPT, struct dirent *ent)
{
	// while (OPT->ent != NULL);
	OPT->ent = ent;
}

void scan_dir(options_t *OPT, char *path)
{
	char regex[MAX_ARG_LENGTH];
	DIR *dir;
	struct dirent *ent;

	time_t start_time;
	time_t last_time = start_time = OPT->START_DATE;
	time_t final_time = OPT->FINAL_DATE;

	strncpy(regex, INCORRECT_FILENAME_REGEX, sizeof(regex));
	regex[23] = '0' + OPT->PARTS_COUNT;
	
	if (!(dir = opendir(path))) ERR("opendir");

	errno = 0;
	while ((ent = readdir(dir)) != NULL && errno == 0)
	{
		if (match(ent->d_name, regex) == 1)
			correct_filename(OPT, ent, &start_time, &last_time, &final_time);
		else
			incorrect_filename(OPT, ent);
	}

	if (errno != 0) ERR("readdir");
	if (closedir(dir)) ERR("closedir");
}


void *file_analysis(void *void_args)
{
	char path[MAX_ARG_LENGTH];
    options_t *OPT = void_args;

    // pthread_mutex_lock(OPT->mx_ent);
    pthread_mutex_lock(OPT->mx_data);

	init_data(OPT);
	
	pathcat(path, OPT);
	if(chdir(path)) ERR("chdir");

	scan_dir(OPT, path);

	for (int j = 1; j <= OPT->data_length; j++)
	{
		printf("%s, \t%d, \t", OPT->data[j].ID, OPT->data[j].parts_send);
		printf("%d, \t", OPT->data[j].minutes_late);
		for (int k = 0; k < OPT->data[j].parts_send; k++)
		{
			printf("%d ", OPT->data[j].solving_time[k]);
		}
		printf("\n");
	}
	
    // pthread_mutex_unlock(OPT->mx_ent);
	pthread_mutex_unlock(OPT->mx_data);

    return NULL;
}