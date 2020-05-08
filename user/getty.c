#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

void login()
{
	char username[20];
	char password[20];

	while(1) {
		printf("Username: ");
		gets(username,20);
		username[strlen(username)-1]='\0';			// removes new line from username

		printf("Password: ");
		echoOnOff();								// turn off echo
		gets(password,20);
		echoOnOff();								// turn on echo
		printf("\n");
		password[strlen(password)-1]='\0';			// removes new line from password

		int valid=loginUser(username, password);
		if(valid)
			break;
		else
			printf("Login incorrect\n");
	}
	printf("hello");
}

int
main(int argc, char *argv[])
{
	login();
	exit();
}
