#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "usergroups.h"

/*
	Sintax1: chmod mode file...
	Sintax2: chmod mode_fmt file...

	arguments[0] -> value for mode
	arguments[1] -> value for file
*/	

int parseCommandLineArguments(char arguments[2][32], int argc, char* argv[])
{
	strcpy(arguments[0], argv[1]);
	strcpy(arguments[1], argv[2]);

	return 1;
}

int
main(int argc, char *argv[])
{
	char arguments[2][32];

	int valid=parseCommandLineArguments(arguments, argc, argv);

	int mode=atoi(arguments[0]);
	chmod(arguments[1], mode);
	exit();	
}
