#include <stdio.h>

int main (int argc, char *argv[]) 
{
	// Print out argc and arg[v] (to use for debugging)
	/*printf("Argc: %d \n Argv: \n", argc);
	for (int x=0;x<argc;x++){
		printf(" %s \n", argv[x]);
	}*/
	if (argc != 3) {
		printf("Usage: <arguments> <directory> \n");
		return 0; 
	}

	printf("Proper arguments passed. \n");
	return 0; 
}