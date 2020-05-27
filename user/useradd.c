#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

/*
	Sintax: useradd [-d homedir] [-u uid] [-c realname] username

	arguments[0] -> value for homedir
	arguments[1] -> value for uid
	arguments[2] -> value for realname
	arguments[3] -> value for username
*/	

int getPositionInArguments(char* command) {
	if(!strcmp(command, "-d")) return 0;
	if(!strcmp(command, "-u")) return 1;
	if(!strcmp(command, "-c")) return 2;
	return -1;
}

int parseCommandLineArguments(char arguments[4][STRING_SIZE], int argc, char* argv[])
{
	if(argc<2)													// if no username is suplied, return error
			return 0;

	strcpy(arguments[3], argv[argc-1]);							// username is mandatory last argument

	int i=1;
	while(i<argc-1) {
		int position=getPositionInArguments(argv[i]);			// get the command
		i++;
 
		if(position==-1)										// if command is not supported, return error
			return 0;

		strcpy(arguments[position],argv[i]);					// get the command value
		i++;
	}

	return 1;
}

int
main(int argc, char *argv[])
{
	char arguments[4][STRING_SIZE];

	int valid=parseCommandLineArguments(arguments, argc, argv);
	if(!valid) {
		printf("Error while creating user!\n");
		exit();
	}
	struct user* newUser=createUser(arguments[0], arguments[1], arguments[2], arguments[3]);
	if(newUser==NULL) {
		printf("Error while creating user!\n");
		exit();
	}
	addNewUser(newUser);

	char gidString[STRING_SIZE];							// is always null because always get next available gid
	struct group* newGroup=createGroup(newUser->username, gidString, ADD_USER);
	addNewGroup(newGroup);
	
	exit();	
}
