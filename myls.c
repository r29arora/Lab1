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
#define MAX_BUFFER_SIZE 255

void process(const char *dir_name, const char *type);
void fperm(char *dir_name);
char *ftype(char *dir_name);
char *getUserName(char *dir_name);
char *getGroupName(char *dir_name);
int getSize(char *dir_name);
void getTime(char *dir_name, const char *type);
char *getSymLinkName(const char *dir_name, const char *path_name);

int main (int argc, char *argv[]) 
{
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
	char *type = argv[1];
	process(argv[2], type);

	return 0; 
}

void process(const char *dir_name, const char *type)
{
	DIR *p_dir;
	char *temp;
	struct dirent *p_dirent;
	if ((p_dir = opendir(dir_name)) == NULL) {
		printf("opendir(%s) failed\n",dir_name);
		return;
	}
	int BUF_SIZE = sizeof(dir_name);
	int counter = 1;
	while ((p_dirent = readdir(p_dir)) != NULL) {
		char *str_path = p_dirent->d_name;	// relative path name!

		if (str_path == NULL) {
			printf("Null pointer found!"); 
			return;
		} else {
			temp = (char *) malloc(BUF_SIZE + BUF_SIZE*counter);
			strcat(temp, dir_name);
			strcat(temp, str_path);
			char *type = ftype(temp);
			printf("%c",type[0]);
			fperm(temp);
			printf("\t %s ",getUserName(temp));
			printf("\t %s ",getGroupName(temp));
			printf("\t %d ",getSize(temp));
			getTime(temp,type);
			if (!strcmp(&type[0],"l")) {
				printf("%s->%s\n",str_path,getSymLinkName(dir_name,temp));
			}
			else {
				printf("%s\n",str_path);
			}			
			counter ++;
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
		return "error";
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
		return "error"; 
	}
	struct passwd *conversion = getpwuid(file.st_uid);
	char *temp = conversion->pw_name;
	//printf(" %s ",temp);
	return temp;
} 

char *getGroupName(char *dir_name)
{
	struct stat file;
	if (lstat(dir_name, &file) < 0){
		perror("lstat error");
		return "error"; 
	}
	struct group *conversion = getgrgid(file.st_gid);
	char *temp = conversion->gr_name;
	return temp;
}

int getSize(char *dir_name)
{
	struct stat file;
	if (lstat(dir_name, &file) < 0){
		perror("error");
	}
	return file.st_size;
}

void getTime(char *dir_name, const char *type)
{	
	struct stat file;
	struct tm *timeinfo; 
	char buffer[256];
	if (lstat(dir_name, &file) < 0){
		perror("lstat error");
		return;
	}
	if 		(!strcmp(&type[1],"l"))	timeinfo = localtime(&file.st_mtime);
	else if (!strcmp(&type[1],"u")) timeinfo = localtime(&file.st_atime);
	else if (!strcmp(&type[1],"c")) timeinfo = localtime(&file.st_ctime);
	else						    timeinfo = localtime(&file.st_mtime);

	strftime(buffer,80,"%a %b %d %Y %I:%M",timeinfo);
	printf("\t %s ",buffer);
	return;
}
char *getSymLinkName(const char *dir_name, const char *path_name) 
{
	static char buffer[MAX_BUFFER_SIZE];
	ssize_t len;
	if ((len = readlink(path_name, buffer, MAX_BUFFER_SIZE-1)) != -1)
	{
		buffer[len] = '\0';
	}
	return buffer;
}
