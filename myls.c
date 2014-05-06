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

void fname(const char *dir_name);

int main (int argc, char *argv[]) 
{
	// Print out argc and arg[v] (to use for debugging)
	/*printf("Argc: %d \n Argv: \n", argc);
	for (int x=0;x<argc;x++){
		printf(" %s \n", argv[x]);
	}*/

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
	fname(argv[2]);
	return 0; 
}

void fname(const char *dir_name)
{
	DIR *p_dir;
	struct dirent *p_dirent;
	printf("%s\n",dir_name);
	if ((p_dir = opendir(dir_name)) == NULL) {
		printf("opendir(%s) failed\n",dir_name);
		return;
	}

}