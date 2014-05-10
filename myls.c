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
void fperm(const char *dir_name);
void ftype(const char *dir_name, char **type);
void getUserName(const char * dir_name);
void getGroupName(const char *dir_name);
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
	printf("%s\n",argv[2]);
	process(argv[2], type);
	return 0; 
}

void process(const char *dir_name, const char *parameter)
{

	DIR *p_dir;
	char *type = malloc(sizeof(char));
	char *str_path = malloc(sizeof(char));
	struct dirent *p_dirent;
	
	if ((p_dir = opendir(dir_name)) == NULL) {
		printf("opendir(%s) failed\n",dir_name);
		exit(1);
	}
	
	int BUF_SIZE = sizeof(dir_name);
	int STR_PATH_SIZE;
	int counter = 1;
	while ((p_dirent = readdir(p_dir)) != NULL) {
		
		str_path = p_dirent->d_name;	// relative path name!
		STR_PATH_SIZE = sizeof(str_path);
		
		if (str_path == NULL) {
			printf("Null pointer found!"); 
			exit(1);
		} 

		else {
			
			char* temp = malloc(BUF_SIZE + STR_PATH_SIZE+1);
			temp[0] = '\0';
			strcat(temp, dir_name);
			strcat(temp, str_path);
			ftype(temp, &type);
			
			printf("%s",type);
			
			fperm(temp);
			strcat(newTemp, temp);
			getUserName(temp);
			getGroupName(temp);
			printf("\t%d\t",getSize(temp));
			
			getTime(temp,parameter);
			
			if (!strcmp(&type[0],"l")) {
				printf("%s->%s\n",str_path,getSymLinkName(dir_name,temp));
			}
			
			else {
				printf("%s\n",str_path);
			}			
			
			counter ++;
			temp[0] = '\0';
			free(temp);
		}
	}
	free(p_dirent);
	closedir(p_dir);
	return;
}

void fperm(const char *dir_name) 
{
	char str[] = "---------\0";
	struct stat buf;
	
	if (lstat(dir_name, &buf) < 0) {
		perror("lstat error");
		printf("\nlstat error with dir_name: %s\n",dir_name );
		exit(1);
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

void ftype(const char *dir_name, char **type)
{
	struct stat buf;
	if (lstat(dir_name, &buf) < 0) {
		perror("lstat error");
		printf("\nlstat error with dir_name: %s\n",dir_name );
		exit(1);
	}
	if (S_ISDIR(buf.st_mode))  		*type = "d";
	else if (S_ISLNK(buf.st_mode))  *type = "l";
	else                            *type = "-";
	return;
}

void getUserName(const char *dir_name)
{
	struct stat file;
	if (lstat(dir_name, &file) < 0){
		perror("lstat error");
		printf("\nlstat error with dir_name: %s\n",dir_name );
		exit(1); 
	}

	struct passwd *pwd;

	int userID = file.st_uid;
	pwd = getpwuid(userID);
	printf("\t%s",pwd->pw_name);
	return;
} 

void getGroupName(const char *dir_name)
{
	struct stat file;
	
	if (lstat((const char *) dir_name, &file) < 0){
		perror("lstat error");
		printf("\nlstat error with dir_name: %s\n",dir_name );
		exit(1);
	}
	
	struct group *conversion = getgrgid(file.st_gid);
	
	if (!conversion) {
		printf("Error getting username from directory: %s\n", dir_name);
		exit(1);
	
	}
	printf("\t %s ",conversion->gr_name);
	return;
}

int getSize(char *dir_name)
{
	struct stat file;
	if (lstat(dir_name, &file) < 0){
		perror("lstat error");
		printf("\nlstat error with dir_name: %s\n",dir_name );
		exit(1);
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
		printf("\nlstat error with dir_name: %s\n",dir_name );
		exit(1);
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
