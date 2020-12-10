#include "file_analysis.h"


int walk(const char *name, const struct stat *s, int type, struct FTW *f)
{
    printf("lol");
    return 0;
}

void scan_dir (){
    printf("scan_dir\n");
	DIR *dirp;
	struct dirent *dp;
	struct stat filestat;
	int dirs=0,files=0,links=0,other=0;
	if(NULL == (dirp = opendir("test"))) ERR("opendir");
	do {
		errno = 0;
		if ((dp = readdir(dirp)) != NULL) {
			if (lstat(dp->d_name, &filestat)) ERR("lstat");
            
            printf("filename: %s", dp->d_name);
			if (S_ISDIR(filestat.st_mode)) dirs++;
			else if (S_ISREG(filestat.st_mode)) files++;
			else if (S_ISLNK(filestat.st_mode)) links++;
			else other++;
		}
	} while (dp != NULL);
	if (errno != 0) ERR("readdir");
	if(closedir(dirp)) ERR("closedir");
	printf("Files: %d, Dirs: %d, Links: %d, Other: %d\n",files,dirs,links,other);
}

void *file_analyzer(void *void_args)
{
    options_t *args = void_args;
    pthread_mutex_lock(args->mx_data_root);
    printf("hours: %d\n", args->START_DATE->tm_hour);

    // if(nftw(args->PATH, walk, MAXFD, FTW_PHYS)==0);

    // if(chdir(args->PATH)) ERR("chdir");
	// printf("%s:\n",args->PATH);
	// scan_dir();

    
    if (nftw(args->PATH, walk, MAXFD, FTW_PHYS)) ERR("nftw");

    
    pthread_mutex_unlock(args->mx_data_root);

    return NULL;
}