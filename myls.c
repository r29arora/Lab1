#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>          // for printf().  man 3 printf
#include <stdlib.h>         // for exit().    man 3 exit
#include <pwd.h>            // for getpwuid()
#include <grp.h>            // for getgrgid()
#include <time.h>           // for ctime()
#include <string.h>         // for strlen()

#define STRLEN1 128
#define STRLEN2 64

void process(const char *dir_name);
void fperm(char *dir_name);
char *ftype(char *dir_name);
char *getUserName(char *dir_name);
char *getGroupName(char *dir_name);

int main (int argc, char *argv[]) 
{
	// Print out argc and arg[v] (to use for debugging)
	/*printf("Argc: %d \n Argv: \n", argc);
	for (int x=0;x<argc;x++){
		printf(" %s \n", argv[x]);	}*/

	/*	
		desired argv[] paramters
		argv[0] -> command   (ex. ./myls.out)
		argv[1] -> argument  (ex. -l)
		argv[2] -> directory (ex. random/)
	*/
		
	if (argc != 3) {
		printf("Usage: <arguments> <directory> \n");
		return 0; 
	}

	printf("Proper arguments passed. \n");
	process(argv[2]);

	return 0; 
}

void process(const char *dir_name)
{
	DIR *p_dir;
	char *temp;
	struct dirent *p_dirent;
	if ((p_dir = opendir(dir_name)) == NULL) {
		printf("opendir(%s) failed\n",dir_name);
		return;
	}

	while ((p_dirent = readdir(p_dir)) != NULL) {
		char *str_path = p_dirent->d_name;	// relative path name!

		if (str_path == NULL) {
			printf("Null pointer found!"); 
			return;
		} else {
			temp = (char *) malloc(strlen(dir_name));
			strcat(temp, dir_name);
			strcat(temp, str_path);
			char *type = ftype(temp);
			printf("%c",type[0]);
			fperm(temp);
			printf(" %s ",str_path);
			printf("%s \n",getUserName(temp));
		}
	}
	free(temp);
	return;
}

void fperm(char *dir_name) 
{
	char str[] = "---------\0";
	struct stat buf;
	
	//printf("Owner permission of %s: ", dir_name);
	if (lstat(dir_name, &buf) < 0) {
		perror("lstat error");
		return;
	}   

	mode_t mode = buf.st_mode;

	str[0] = (mode & S_IRUSR) ? 'r' : '-';
	str[1] = (mode & S_IWUSR) ? 'w' : '-';
	str[2] = (mode & S_IXUSR) ? 'x' : '-';
	str[3] = (mode & S_IRGRP) ? 'r' : '-';
	str[4] = (mode & S_IWGRP) ? 'w' : '-';
	str[5] = (mode & S_IXGRP) ? 'x' : '-';

	printf("%s", str);
}

char *ftype(char *dir_name)
{
	char *ptr;
	struct stat buf;

	if (lstat(dir_name, &buf) < 0) {
		perror("lstat error");
		return "lstat error";
	}

	if (S_ISDIR(buf.st_mode))  		ptr = "d";
	else if (S_ISLNK(buf.st_mode))  ptr = "l";
	else                            ptr = "-";
	return ptr;
}

char *getUserName(char *dir_name)
{
	struct stat file;
	if (lstat(dir_name, &file) < 0){
		perror("lstat error");
		return "lstat error"; 
	}
	struct passwd *conversion = getpwuid(file.st_uid);
	char* temp = conversion->pw_name;
	//printf(" %s ",temp);
	return temp;
} 
char *getGroupName(char *dir_name)
{
	
}