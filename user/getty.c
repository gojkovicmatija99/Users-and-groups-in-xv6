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
		username[strlen(username)-1]='\0';			//removes new line from username

		printf("Password: ");
		gets(password,20);
		password[strlen(password)-1]='\0';			//removes new line from password

		int valid=loginUser(username, password);
		if(valid)
			break;
	}
	printf("hello");
}

int
main(int argc, char *argv[])
{
	login();
	exit();
}
