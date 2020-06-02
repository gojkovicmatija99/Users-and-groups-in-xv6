#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

/*
	Sintax: usermod [-l newUsername] [-u UID] [-c realname] [-d dir] [-m] [-G groups…] [-a] username

	arguments[0] -> value for newUsername
	arguments[1] -> value for uid
	arguments[2] -> value for realname
	arguments[3] -> value for homedir
	arguments[4] -> value for m (move)
	arguments[5] -> value for groups
	arguments[6] -> value for a (append)
	arguments[7] -> value for username
*/	

int getPositionInArguments(char* command) {
	if(!strcmp(command, "-l")) return 0;
	if(!strcmp(command, "-u")) return 1;
	if(!strcmp(command, "-c")) return 2;
	if(!strcmp(command, "-d")) return 3;
	if(!strcmp(command, "-m")) return 4;
	if(!strcmp(command, "-G")) return 5;
	if(!strcmp(command, "-a")) return 6;
	return -1;
}

int parseCommandLineArguments(char arguments[8][STRING_SIZE], int argc,char* argv[])
{
	if(argc<2)													// if no username is suplied, return error
			return 0;

	strcpy(arguments[7], argv[argc-1]);							// username is mandatory last argument

	int i=1;	
	while(i<argc-1) {
		int position=getPositionInArguments(argv[i]);			// get the command
		i++;
 
		if(position==-1)										// if command is not supported, return error
			return 0;

		if(position==4)											// these commands are only flags
		{
			strcpy(arguments[4], "1");
			continue;
		}
		if(position==6)
		{
			strcpy(arguments[6], "1");
			continue;
		}

		strcpy(arguments[position],argv[i]);					// get the command value
		i++;
	}

	return 1;
}

int
main(int argc, char *argv[])
{
	char arguments[8][STRING_SIZE];

	int valid=parseCommandLineArguments(arguments, argc, argv);
	if(!valid) {
		printf("Error while modifying user!\n");
		exit();
	}

	struct user* currUser=getUserFromUsername(arguments[7]);
	if(currUser==NULL) {
		printf("Error while modifying user!\n");
		exit();
	}
		
	struct user* modUser=modifyUser(currUser, arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6]);
	if(modUser==NULL)
		printf("Error while modifying user!\n");
	else
		updateUserInfo(currUser, modUser);

	exit();	
}
