#include "usergroups.h"
#include "kernel/fcntl.h"

struct user* getUser(char* userString)
{
	struct user* currUser=(struct user*)malloc(sizeof(struct user));
	char tmp[6][50];	
   char buf[50];

   int pnt=0;                 // pnt points to the absolute position in the string
   for(int i=0;i<6;i++) {
      int curr=0;             // curr points to the relative position in the string
      while(userString[curr+pnt]!=':' && userString[curr+pnt]!='\0') {
         buf[curr]=userString[curr+pnt];
         curr++;
      }
      buf[curr]='\0';
      strcpy(tmp[i],buf);
      pnt+=curr+1;         // pnt points to where curr stoped + 1 to skip ':'
   }
   
   strcpy(currUser->username,tmp[0]);
   strcpy(currUser->password,tmp[1]);
   currUser->uid=atoi(tmp[2]);
   currUser->gid=atoi(tmp[3]);
   strcpy(currUser->realName,tmp[4]);
   strcpy(currUser->homedir,tmp[5]);
   currUser->next=NULL;

   return currUser;
}

struct user* addUserToList(struct user* userList, struct user* currUser)
{
   if(userList==NULL)
      return currUser;

   struct user* tmp=userList;
   while(tmp->next!=NULL)
      tmp=tmp->next;

   tmp->next=currUser;
   return userList;
}

struct user* getAllUsers()
{  
   struct user* userList=NULL;

	int fd=open("/etc/passwd",O_RDONLY);
	int size=fsize(fd);
	char fileContent[size];

	read(fd,fileContent,size);

   char* token = strtok(fileContent, "\n");
   while( token != NULL ) {
      struct user* currUser=getUser(token);
      userList=addUserToList(userList,currUser);

      token = strtok(NULL, "\n");
   }

   close(fd);
   return userList;
}

int checkUsernamePasswordForCurrUser(char* username, char* password, struct user* currUser)
{
   
   if(strcmp(username,currUser->username)==0 && strcmp(password,currUser->password)==0)
      return 1;
   return 0;
}

// TODO: Sometimes user can't login when he fails to login the first time
struct user* authenticateUser(char* username, char* password)
{
   struct user* userList=getAllUsers();

   while(userList!=NULL) {
      int valid=checkUsernamePasswordForCurrUser(username,password,userList);
      if(valid)
         return userList;

      userList=userList->next;
   }

   return NULL;
}

void printEtcFile(char* file)
{
   char path[50];
   strcpy(path,"/etc/");
   strcat(path,file);

   int fd=open(path,O_RDONLY);

   if(fd!=-1) {
      int size=fsize(fd);
      char fileContent[size];

      read(fd,fileContent,size);
      printf("%s\n", fileContent);
   }
   
}

