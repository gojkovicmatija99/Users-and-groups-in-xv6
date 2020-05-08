#include "usergroups.h"
#include "kernel/fcntl.h"

struct user* readUser(char* userString)
{
	struct user* currUser=(struct user*)malloc(sizeof(struct user));
	char tmp[6][50];	

	char* token;
	token= strtok(userString, ":");
	int curr=0;
   	while( token != NULL ) {	
      strcpy(tmp[curr++],token);
      token = strtok(NULL, ":");
   }

   strcpy(currUser->username,tmp[0]);
   strcpy(currUser->password,tmp[1]);
   currUser->uid=atoi(tmp[2]);
   currUser->gid=atoi(tmp[3]);
   strcpy(currUser->realName,tmp[4]);
   strcpy(currUser->homedir,tmp[5]);

   return currUser;
}

int checkUsernamePassword(char* username, char* password, struct user* currUser)
{
	if(strcmp(username,currUser->username)==0 && strcmp(password,currUser->password)==0)
		return 1;
	return 0;
}

int loginUser(char* username, char* password)
{
	int fd=open("/etc/passwd",O_RDONLY);
	int size=fsize(fd);
	char fileContent[size];

	read(fd,fileContent,size);

	/*char* token = strtok2(fileContent, ENTER);
   	while( token != NULL ) {
      struct user* currUser=readUser(token);
      int valid=checkUsernamePassword(username,password,currUser);
      if(valid)
      	return 1;
      token = strtok2(NULL, ENTER);
   }*/

	struct user* currUser=readUser(fileContent);
	int valid=checkUsernamePassword(username,password,currUser);
	if(valid)
		return 1;

   close(fd);

   return 0;
}

void printEtcFile(char* file)
{
   char path[50];
   strcpy(path,"/etc/");
   strcat(path,file);

   int fd=open(path,O_RDONLY);
   int size=fsize(fd);
   char fileContent[size];

   read(fd,fileContent,size);
   
   printf("%s\n", fileContent);
}