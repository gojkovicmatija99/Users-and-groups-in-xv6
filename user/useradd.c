#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

/*
	Sintax: useradd [-d homedir] [-u uid] [-c realname] username

	arguments[0] -> value of homedir
	arguments[1] -> value of uid
	arguments[2] -> value of realname
	arguments[3] -> value of username
*/	

int getPositionInArguments(char* command) {
	if(!strcmp(command, "-d")) return 0;
	if(!strcmp(command, "-u")) return 1;
	if(!strcmp(command, "-c")) return 2;
	return 3;
}


int parseCommandLineArguments(char arguments[4][64],int argc,char* argv[])
{
	int i=1;
	while(i<argc) {
		int position=getPositionInArguments(argv[i]);			// get the command
 
		if(position==3 && i!=argc-1)							// if username isn't last, return error
			return 0;

		if(position!=3)											// if the argument isn't username, the value for the optional command is in i+1
			i++;												// else this is the value for username

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
		printf("Error while creating user");
	}
	else {
		struct user* newUser=createUser(arguments[0], arguments[1], arguments[2], arguments[3]);
		if(newUser==NULL) {
			printf("Error while creating user");
		}
		//else
		//	addNewUserToPasswdFile(newUser);
	}
	exit();	
}
