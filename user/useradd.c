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


int parseCommandLineArguments(char arguments[4][64],int argc,char* argv[])
{
	strcpy(arguments[3], argv[argc-1]);							// username is mandatory last argument

	int i=1;
	while(i<argc-1) {
		int position=getPositionInArguments(argv[i]);			// get the command
 
		if(position==-1)										// if command is not supported, return error
			return 0;

		strcpy(arguments[position],argv[i]);
		i++;
	}

	return 1;
}

int
main(int argc, char *argv[])
{
	char arguments[4][64];

	int valid=parseCommandLineArguments(arguments, argc, argv);
	if(!valid) {
		printf("Error while creating user!\n");
	}
	else {
		struct user* newUser=createUser(arguments[0], arguments[1], arguments[2], arguments[3]);
		if(newUser==NULL) {
			printf("Error while creating user!\n");
		}
		else 
			addNewUser(newUser);

		struct group* newGroup=createGroup(newUser->username, newUser->gid);
		addNewGroup(newGroup);
	}
	exit();	
}
