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
	stud->minutes_late = *final_t > *last_t ? 0 : *mtime - *final_t;
	*last_t = *mtime;
}

void init_data(options_t *OPT)
{
	OPT->data_length = 0;
	OPT->data = (student_t *)malloc(sizeof(student_t) * DEFAULT_STUDENT_COUNT);
	if (!(OPT->data)) ERR("malloc");
}

void update_data(options_t * OPT)
{
	static char ID_buffer[MAX_ARG_LENGTH];
}

void scan_dir(options_t *OPT, char *path)
{
	char regex[MAX_ARG_LENGTH];
	char ID_buffer[MAX_ARG_LENGTH];
	strncpy(regex, INCORRECT_FILENAME_REGEX, MAX_ARG_LENGTH);
	DIR *dir;
	struct dirent *ent;
	int stud_count = DEFAULT_STUDENT_COUNT;

	struct stat filestat;
	student_t * data = OPT->data;

	int i = 0;

	time_t start_time;
	time_t last_mtime = start_time = OPT->START_DATE;
	time_t final_time = OPT->FINAL_DATE;

	
	regex[23] = '0' + OPT->PARTS_COUNT;

	int last_part = 0;
	
	if (!(dir = opendir(path))) ERR("opendir");

	while ((ent = readdir(dir)) != NULL)
	{
		if (match(ent->d_name, regex) == 1)
		{
			char *ID;

			if (lstat(ent->d_name, &filestat)) ERR("lstat");
			time_t mtime = filestat.st_mtime;

			strncpy(ID_buffer, ent->d_name, sizeof(ID_buffer));
			ID = strtok(ID_buffer, DOT_DELIM);

			if (strncmp(ID, data[i].ID, MAX_ARG_LENGTH) == 0)
				add_part(&data[i], &last_part, &mtime, &last_mtime, &final_time);
			else
			{
				OPT->data_length++;
				last_part = 0;
				last_mtime = start_time;

				if (++i >= stud_count)
					reallocate_data(data, (stud_count *= 2));

				strncpy(data[i].ID, ID, MAX_ARG_LENGTH);
				add_part(&data[i], &last_part, &mtime, &last_mtime, &final_time);
			}
		}
		else
		{
			if (match(ent->d_name, INCORRECT_PART_REGEX) == 1)
			{
				// printf("(%s) Niepoprawny numer etapu\n", ent->d_name);
			}
			else
			{
				// printf("(%s) Niepoprawna nazwa pliku\n", ent->d_name);
			}
		}
	}

	if (errno != 0) ERR("readdir");
	if (closedir(dir)) ERR("closedir");
	
	OPT->data = data;
}

void *file_analysis(void *void_args)
{
	char path[MAX_ARG_LENGTH];
    options_t *OPT = void_args;

    pthread_mutex_lock(OPT->mx_data);

	init_data(OPT);
	
	pathcat(path, OPT);
	if(chdir(path)) ERR("chdir");

	scan_dir(OPT, path);

	for (int j = 1; j <= OPT->data_length; j++)
	{
		printf("%s, \t%d, \t", OPT->data[j].ID, OPT->data[j].parts_send);
		for (int k = 0; k < OPT->data[j].parts_send; k++)
		{
			printf("%d ", OPT->data[j].solving_time[k]);
		}
		printf("\n");
	}
	
	pthread_mutex_unlock(OPT->mx_data);

    return NULL;
}