#include "file_analysis.h"

void append_node(student_t *root, student_t *new_node)
{
	root->next = new_node;
	new_node->next = NULL;
}

void print_list(student_t *root)
{
	while (root != NULL)
	{
		printf(" %s ", root->ID);
		root = root->next;
	}
}

void free_list(student_t *root)
{
	if (root != NULL)
	{
		free_list(root->next);
		free(root);
	}
}

void pathcat(char *path, options_t *OPT)
{
	if (getcwd(path, MAX_ARG_LENGTH) == NULL) 
        ERR("getcwd");

	strncat(path, "/", 2);
	strncat(path, OPT->PATH, MAX_ARG_LENGTH);
}

void reallocate_data(options_t *OPT, int new_length)
{
	OPT->data = (student_t *)realloc(OPT->data, sizeof(student_t) * new_length);
	if (OPT->data == NULL)
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
	OPT->data = (student_t *)calloc(1, sizeof(student_t));
	if (!(OPT->data)) ERR("calloc");
	strncpy(OPT->data->ID, "", sizeof(OPT->data->ID));
}

void incorrect_file(options_t * OPT, struct dirent *ent)
{
	int signo;
	OPT->ent = ent;
	// OPT->new_mistake = true;	// Informacja dla wątku [2] o nowym błędzie

	// Wątek [0] oczekuje na wiadomość zwrotną od wątku [2]
	// while (1)
	// {
	// 	if(sigwait(OPT->masks[0], &signo)) 
    //         ERR("sigwait failed.");

    //     if (signo == SIGCONT)
	// 	{
	// 		printf("thread[0] got SIGCONT\n");
	// 		break;
	// 	}
	// }
}

// void scan_dir(options_t *OPT, char *path)
// {
// 	char regex[MAX_ARG_LENGTH] = "";
// 	DIR *dir;
// 	struct dirent *ent;
//
// 	time_t start_t;
// 	time_t last_t = start_t = OPT->START_DATE;
// 	time_t final_t = OPT->FINAL_DATE;
//
// 	char ID_buffer[MAX_ARG_LENGTH] = {0};
// 	struct stat filestat;
// 	char *ID = NULL;
// 	int last_part = 0;
// 	student_t *curr = OPT->data;
//
// 	strncpy(regex, INCORRECT_FILENAME_REGEX, sizeof(regex));
// 	regex[23] = '0' + OPT->PARTS_COUNT;
//
// 	if (!(dir = opendir(path))) ERR("opendir");
//
// 	errno = 0;
// 	while ((ent = readdir(dir)) != NULL && errno == 0)
// 	{
// 		if (match(ent->d_name, regex) == 1)
// 		{
// 			if (lstat(ent->d_name, &filestat))
// 				ERR("lstat");
//
// 			time_t mtime = filestat.st_mtime;
//
// 			strncpy(ID_buffer, ent->d_name, sizeof(ID_buffer));
// 			if ((ID = strtok(ID_buffer, DOT_DELIM)) == NULL)
// 				ERR("strtok");
//
// 			if (strncmp(ID, curr->ID, MAX_ARG_LENGTH) == 0)
// 				add_part(curr, &last_part, &mtime, &last_t, &final_t);
// 			else
// 			{
// 				printf("lol");
// 				student_t *new_node = (student_t *)calloc(1, sizeof(student_t));
// 				append_node(curr, new_node);
// 				curr = curr->next;
// 				strncpy(curr->ID, ID, sizeof(curr->ID));
// 				curr->parts_send = 1;
// 				add_part(curr, &last_part, &mtime, &last_t, &final_t);
// 				print_list(OPT->data);
// 			}
// 		}
// 		else
// 		{
// 			incorrect_file(OPT, ent);
// 		}
// 	}
//
// 	if (errno != 0) ERR("readdir");
// 	if (closedir(dir)) ERR("closedir");
// }


void scan_dir(options_t *OPT, char *path)
{
	char regex[MAX_ARG_LENGTH] = "";
	DIR *dir;
	struct dirent *ent;

	time_t start_t;
	time_t last_t = start_t = OPT->START_DATE;
	time_t final_t = OPT->FINAL_DATE;

	char ID_buffer[MAX_ARG_LENGTH] = {0};
	struct stat filestat;
	char *ID = NULL;
	int last_part = 0;
	student_t *curr = OPT->data;

	strncpy(regex, INCORRECT_FILENAME_REGEX, sizeof(regex));
	regex[23] = '0' + OPT->PARTS_COUNT;
	
	if (!(dir = opendir(path))) ERR("opendir");

	errno = 0;
	while ((ent = readdir(dir)) != NULL && errno == 0)
	{
		printf("filename: %s\n", ent->d_name);
		if (match(ent->d_name, regex) == 1)
		{
			if (lstat(ent->d_name, &filestat))
				ERR("lstat");

			time_t mtime = filestat.st_mtime;

			strncpy(ID_buffer, ent->d_name, sizeof(ID_buffer));
			if ((ID = strtok(ID_buffer, DOT_DELIM)) == NULL)
				ERR("strtok");

			printf("ID: %s\n", ID);

			if (strncmp(ID, curr->ID, MAX_ARG_LENGTH) == 0)
				add_part(curr, &last_part, &mtime, &last_t, &final_t);
			else
			{
				curr->next = (student_t *)calloc(1, sizeof(student_t));
				if (curr->next == NULL) ERR("malloc");
				curr = curr->next;
				curr->next = NULL;
				curr->parts_send = 0;
				strncpy(curr->ID, ID, sizeof(curr->ID));
				
				printf("list: ");
				print_list(OPT->data->next);
				printf("\n");
				add_part(curr, &last_part, &mtime, &last_t, &final_t);
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


void *file_analysis(void *void_args)
{
	char path[MAX_ARG_LENGTH] = "";
    options_t *OPT = void_args;

    pthread_mutex_lock(OPT->mx_data);

	// init_data(OPT);
	OPT->data = (student_t *)calloc(1, sizeof(student_t));
	if (!(OPT->data)) ERR("calloc");
	OPT->data->next = NULL;
	
	pathcat(path, OPT);
	if(chdir(path)) ERR("chdir");

	scan_dir(OPT, path);
	OPT->work_finished = true;

	pthread_kill(OPT->threads[1], SIGUSR1);

	printf("done\n");

	pthread_mutex_unlock(OPT->mx_data);

    return NULL;
}