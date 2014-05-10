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
#define MAX_BUFFER_SIZE 256

void printTypeandPermissions(struct stat buf, char *filetype);
void printUsernameandGroupName(struct stat buf);
void printSize(struct stat buf);
void printTime(struct stat buf, char *flagtype);

int main (int argc, char *argv[])
{
	if (argc !=3) {
		printf("Usage: <arguments> <directory> \n");
		exit(1);
	}
	
	char *flag = argv[1];
	const char *parent_dir_name = argv[2];
	struct stat buf;

	printf("%s\n",argv[2]);

	DIR *p_dir;
	struct dirent *p_dirent;

	if (!(p_dir = opendir(parent_dir_name))){
		printf("opendir(%s) failed!\n",parent_dir_name);
		exit(1);
	}

	while ((p_dirent = readdir(p_dir))){
		char *file_name = p_dirent->d_name;
		
		if(!file_name){
			printf("Null pointer found!\n");
			exit(1);
		}
		
		char *full_path_name = malloc(strlen(parent_dir_name) + strlen(file_name) + 2);
		char filetype = '-';
		strcpy(full_path_name,parent_dir_name);
		strcat(full_path_name,file_name);

		if (lstat(full_path_name, &buf) < 0){
			perror("lstat error");
			exit(1);
		}
		printTypeandPermissions(buf,&filetype);
		printUsernameandGroupName(buf);
		printSize(buf);
		printTime(buf,flag);
		free(full_path_name);
		printf("\t%s",file_name);

		if (filetype == 'l'){
			static char sym_link_name[MAX_BUFFER_SIZE];
			ssize_t len;
			if ((len = readlink(full_path_name, sym_link_name, MAX_BUFFER_SIZE-1)) != -1)
			{
				sym_link_name[len] = '\0';
				printf("->%s",sym_link_name);
			}
			else {
				printf("readlink(%s) failed\n",full_path_name);
			}
			
		}
		printf("\n");

	}
	exit(0);
}

void printTypeandPermissions(struct stat buf, char *filetype)
{
	char str[] = "---------\0";

	if (S_ISDIR(buf.st_mode)){*filetype = 'd';}
	else if (S_ISLNK(buf.st_mode)){*filetype = 'l';}
	
	mode_t mode = buf.st_mode;

	str[0] = (mode & S_IRUSR) ? 'r' : '-';
	str[1] = (mode & S_IWUSR) ? 'w' : '-';
	str[2] = (mode & S_IXUSR) ? 'x' : '-';
	str[3] = (mode & S_IRGRP) ? 'r' : '-';
	str[4] = (mode & S_IWGRP) ? 'w' : '-';
	str[5] = (mode & S_IXGRP) ? 'x' : '-';

	printf("%c%s",*filetype,str);
	return;
}

void printUsernameandGroupName(struct stat buf)
{
	struct group *groupStruct = getgrgid(buf.st_gid);
	struct passwd *userStruct = getpwuid(buf.st_uid);
	
	if (!groupStruct || !userStruct){
		printf("Username or Groupname Error\n");
		exit(1);
	}
	printf("\t%s\t%s",userStruct->pw_name,groupStruct->gr_name);
	return;
}

void printSize(struct stat buf)
{
	printf("\t%ld",(long)buf.st_size);
	return;
}
void printTime(struct stat buf, char *flagtype)
{
	struct tm *timeinfo;
	char buffer[256];
	if (!strcmp(&flagtype[1],"l")){
		timeinfo = localtime(&buf.st_mtime);
	}
	else if (!strcmp(&flagtype[1],"u")){
		timeinfo = localtime(&buf.st_atime);
	}
	else if (!strcmp(&flagtype[1],"c")){
		timeinfo = localtime(&buf.st_ctime);
	}
	else {
		timeinfo = localtime(&buf.st_mtime);
	}

	strftime(buffer,80,"%a %b %d %Y %I:%M",timeinfo);
	printf("\t %s ",buffer);
	return;
}















