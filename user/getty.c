#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

char *argv[] = { "sh", 0 };

struct user* login()
{
	char username[STRING_SIZE];
	char password[STRING_SIZE];

	while(1) {
		printf("Username: ");
		gets(username, STRING_SIZE);
		username[strlen(username)-1]='\0';			// remove new line from username

		printf("Password: ");
		echoOnOff();								// turn off echo
		gets(password, STRING_SIZE);
		echoOnOff();								// turn on echo
		printf("\n");
		password[strlen(password)-1]='\0';

		struct user* currUser=authenticateUser(username, password);
		if(currUser)
			return currUser;
		else
			printf("Login incorrect!\n");
	}
}

int
main(int argc, char *argv[])
{
	clear();
	printEtcFile("issue");
	struct user* currUser=login();
	printEtcFile("motd");	
	chdir(currUser->homedir);
	setuid(currUser->uid);
	exec("/bin/sh", argv);
	exit();
}
