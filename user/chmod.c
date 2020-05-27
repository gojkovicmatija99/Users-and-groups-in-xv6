#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "usergroups.h"

/*
	Sintax1: chmod mode file...
	Sintax2: chmod mode_fmt file...

	arguments[0] -> value for mode
	arguments[1] -> value for first file
	...
	...
	arguments[numOfFiles] -> value for n-th file
*/	

int parseCommandLineArguments(int numOfFiles, char arguments[numOfFiles][STRING_SIZE], int argc, char* argv[])
{
	if(argc<3) 
		return 0;

	strcpy(arguments[0], argv[1]);
	
	for(int i=1;i<=numOfFiles;i++)
		strcpy(arguments[i], argv[i+1]);

	return 1;
}

int
main(int argc, char *argv[])
{
	int numOfFiles=argc-2;
	char arguments[numOfFiles][STRING_SIZE];

	int valid=parseCommandLineArguments(numOfFiles, arguments, argc, argv);
	if(!valid) {
		printf("Error while changing mode!\n");
		exit();
	}

	for(int i=1;i<=numOfFiles;i++) {					// check if all files exist
		struct stat* inodeStat=NULL;

		if(stat(arguments[i], inodeStat)==-1) {
			printf("Error while changing mode!\n");
			exit();
		}
	}

	for(int i=1;i<=numOfFiles;i++) {
		struct stat* inodeStat=NULL;
		stat(arguments[i], inodeStat);

		int newMode=atoi(arguments[0]);
		if(newMode==0 && arguments[0][0]!='0')					// if value for mode is string, parse it
			newMode=convertStringToMode(arguments[0], inodeStat->mode);
		else
			newMode=convertOctalToDecimal(newMode);				// else convert octal number to decimal number

		if(newMode<0) {		
			printf("Error while changing mode!\n");
			exit();
		}

		int valid=chmod(arguments[i], newMode);
		if(valid<0) {
			printf("Permision denied!\n");
			exit();
		}
	}
	
	exit();	
}
