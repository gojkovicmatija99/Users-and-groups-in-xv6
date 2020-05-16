#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "usergroups.h"

/*
	Sintax: passwd [USER]
*/

int
main(int argc, char *argv[])
{
	int procUid=getuid();
	struct user* user=getUserFromUid(procUid);

	if(argc>1) {
		if(procUid!=ROOT) {									// only root can change password for other users
			printf("Permision denied!\n");
			exit();
		}
		else {
			user=getUserFromUsername(argv[1]);
			if(user==NULL) {
				printf("User '%s' does not exits!\n",argv[1]);
				exit();
			}
		}
	}

	char oldPassword[32];
	if(procUid!=ROOT) {									// root user can skip verification of old password
		printf("Old password: ");

		echoOnOff();									// turn off echo
		gets(oldPassword, 32);
		echoOnOff();									// turn on echo
		printf("\n");
		oldPassword[strlen(oldPassword)-1]='\0';		// remove new line from old password

		int valid=authenticateOldPassword(user, oldPassword);
		if(!valid) {
			printf("Wrong password!\n");
			exit();
		}					
	}				

	char newPassword[32];
	char retypePassword[32];
	printf("New password: ");
		
	echoOnOff();
	gets(newPassword, 32);
	echoOnOff();
	printf("\n");
	newPassword[strlen(newPassword)-1]='\0';	

	if(strlen(newPassword)<=6) {
		printf("Password needs to be longer than 6 characters!\n");
		exit();
	}

	printf("Retype new password: ");

	echoOnOff();									
	gets(retypePassword, 32);
	echoOnOff();									
	printf("\n");
	retypePassword[strlen(retypePassword)-1]='\0';

	if(strcmp(newPassword, retypePassword)!=0) {
		printf("Passwords do not match!\n");
		exit();
	}

	updatePasswordForUser(user, newPassword);

	exit();
}
