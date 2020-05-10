#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

char *argv[] = { "sh", 0 };

struct user* login()
{
	char username[20];
	char password[20];

	while(1) {
		printf("Username: ");
		gets(username,20);
		username[strlen(username)-1]='\0';			// remove new line from username

		printf("Password: ");
		echoOnOff();								// turn off echo
		gets(password,20);
		echoOnOff();								// turn on echo
		printf("\n");
		password[strlen(password)-1]='\0';			// remove new line from password

		struct user* currUser=authenticateUser(username, password);
		if(currUser)
			return currUser;
		else
			printf("Login incorrect\n");
	}
}

int
main(int argc, char *argv[])
{
	clear();
	printEtcFile("issue");
	struct user* currUser=login();
	printEtcFile("motd");
	setuid(currUser->uid);
	exec("/bin/sh", argv);
	exit();
}
