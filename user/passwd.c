#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

int
main(int argc, char *argv[])
{
	int procUid=getuid();
	struct user* user=getUserFromUid(procUid);

	if(argc>1) {
		if(procUid!=0) {
			printf("Permision denied!\n");
			exit();
		}
		else {
			user=getUserFromUsername(argv[1]);
			if(user==NULL) {
				printf("User does not exits!\n");
				exit();
			}
		}
	}

	char oldPassword[32];
	if(procUid!=0) {										// root user can skip verification of old password
		while(1) {
			printf("Old password: ");

			echoOnOff();									// turn off echo
			gets(oldPassword, 32);
			echoOnOff();									// turn on echo
			printf("\n");
			oldPassword[strlen(oldPassword)-1]='\0';		// remove new line from old password

			int valid=authenticateOldPassword(user, oldPassword);
			if(valid)
				break;
			else
				printf("Wrong password!\n");
		}						
	}				

	char newPassword[32];
	char retypePassword[32];
	while(1) {
		printf("New password: ");
		
		echoOnOff();									// turn off echo
		gets(newPassword, 32);
		echoOnOff();									// turn on echo
		printf("\n");
		newPassword[strlen(newPassword)-1]='\0';

		if(strlen(newPassword)<=6) {
			printf("Password needs to be longer than 6 characters!\n");
			continue;
		}

		printf("Retype new password: ");

		echoOnOff();									// turn off echo
		gets(retypePassword, 32);
		echoOnOff();									// turn on echo
		printf("\n");
		retypePassword[strlen(retypePassword)-1]='\0';

		if(!strcmp(newPassword, retypePassword))
			break;
		else
			printf("Passwords do not match!\n");
	}

	updatePasswordForUserInPasswdFile(user, newPassword);

	exit();
}
