/* --------------------------------------------------------------------------
ECE 254: Lab 1: Simple ls function 

Group Memebers: Rajul Arora & Gautham Kamath
StudentID: 20463068 & 
QuestID: r9arora &

Steps to Use: 
1. make clean
2. make
3 ./myls <flag> <directory>

Flags:
-l -> Last Modified Date
-u -> Last Access Date
-c -> Last Change Date
-------------------------------------------------------------------------- */
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
#define MAX_BUFFER_SIZE 256 // for readlink()

void printTypeandPermissions(struct stat buf, char *filetype);
void printUsernameandGroupName(struct stat buf);
void printSize(struct stat buf);
void printTime(struct stat buf, char *flagtype);

int main (int argc, char *argv[])
{
	// throws an error if there aren't at least two arguments after ./myls
	if (argc !=3) {
		printf("Usage:./myls <arguments> <directory> \n");
		exit(1);
	}
	
	char *flag = argv[1];
	const char *parent_dir_name = argv[2];
	struct stat buf;

	//print the directory name
	printf("%s\n",argv[2]);

	DIR *p_dir;
	struct dirent *p_dirent;

	// throws an error if opening the directory failed
	if (!(p_dir = opendir(parent_dir_name))){
		printf("opendir(%s) failed!\n",parent_dir_name);
		exit(1);
	}

	//goes through each directory and prints out the required columns
	while ((p_dirent = readdir(p_dir))){
		char *file_name = p_dirent->d_name;
		
		// throws an error if there is no file name stored in p_dirent->d_name
		if(!file_name){
			printf("Null pointer found!\n");
			exit(1);
		}
		
		char *full_path_name = malloc(strlen(parent_dir_name) + strlen(file_name) + 2);
		
		// throws an error if there is an error allocating memory with malloc()
		if (!full_path_name){
			perror("malloc error");
			exit(1);
		}
		// initialize with the default filetype
		char filetype = '-';

		//copy the parent directory name and appent the current 
		//filename to full_path_name
		strcpy(full_path_name,parent_dir_name);
		strcat(full_path_name,file_name);

		// throws an error if lstat() fails
		if (lstat(full_path_name, &buf) < 0){
			perror("lstat error");
			exit(1);
		}

		//print the necessary information
		printTypeandPermissions(buf,&filetype);
		printUsernameandGroupName(buf);
		printSize(buf);
		printTime(buf,flag);
		printf("%s",file_name);

		//print the path to the linked file (if it exists)
		if (filetype == 'l'){
			static char sym_link_name[MAX_BUFFER_SIZE];
			ssize_t len;
			if ((len = readlink(full_path_name, sym_link_name, MAX_BUFFER_SIZE-1)) != -1)
			{
				sym_link_name[len] = '\0';
				printf("->%s",sym_link_name);
			}
			//throws an error if readlink() fails
			else {
				printf("readlink(%s) failed\n",full_path_name);
				exit(1);
			}
			
		}
		// free memory
		free(full_path_name);

		printf("\n");

	}
	//free more memory
	free(p_dirent);
	closedir(p_dir);
	exit(0);
}

void printTypeandPermissions(struct stat buf, char *filetype)
{
	char str[] = "---------\0";

	if (S_ISDIR(buf.st_mode)){*filetype = 'd';}
	else if (S_ISLNK(buf.st_mode)){*filetype = 'l';}
	
	mode_t mode = buf.st_mode;

	//stores the permissions in the appropriate location
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
	
	// throws an error if getgrid() or getpwuid() fails
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
	
	//stores the time based on the flag, throws an error
	//if the flag is incorrect
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
		printf("Error: innapropriate flag\n");
		exit(1);		
	}

	strftime(buffer,80,"%b  %d  %I:%M",timeinfo);
	printf("\t %s ",buffer);
	return;
}