#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "usergroups.h"

struct user* getUserFromString(char* userString)
{
	struct user* currUser=(struct user*)malloc(sizeof(struct user));
	char tmp[6][64];	
   char buf[64];

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
   strcpy(currUser->realname,tmp[4]);
   strcpy(currUser->homedir,tmp[5]);
   currUser->next=NULL;

   return currUser;
}

char* getStringFromUser(struct user* currUser, char* userString)
{
   strcpy(userString, currUser->username);
   strcat(userString, ":");

   strcat(userString, currUser->password);
   strcat(userString, ":");

   char uidString[32];
   itoa(currUser->uid, uidString, 10);
   strcat(userString, uidString);
   strcat(userString, ":");

   char gidString[32];
   itoa(currUser->gid, gidString, 10);
   strcat(userString, gidString);
   strcat(userString, ":");

   strcat(userString, currUser->realname);
   strcat(userString, ":");

   strcat(userString, currUser->homedir);
   strcat(userString, "\n");

   return userString;
}

struct user* createUser(char* homedir, char* uidString, char* realname,char* username)
{
   struct user* newUser=(struct user*)malloc(sizeof(struct user));

   strcpy(newUser->homedir, "/home/");                    // all users are stored in "/home/"
   if(!isEmptyString(homedir))                                   // if user has entered homedir
      strcat(newUser->homedir, homedir);
   else
      strcat(newUser->homedir, username);

   if(mkdir(newUser->homedir)==-1)                       // if dir isn't created, return error
      return NULL;

   if(!isEmptyString(uidString)) {                              // if user has entered uid
      int uid=atoi(uidString);
      if(uid==0)                                               // if uidString isn't a number, return error
         return NULL;
      if(isUidAvailable(uid))                                  // if uid isn't available, return error
         newUser->uid=uid;
      else
         return NULL;
   }
   else {
      int uid=getNextAvailableUid();                     // else get available uid that is greater than 1000
      newUser->uid=uid;
   }

   strcpy(newUser->realname, realname);
   strcpy(newUser->username, username);

   return newUser;

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

struct user* selectAllUsersFromPasswdFile()
{  
   struct user* userList=NULL;

	int fd=open("/etc/passwd",O_RDONLY);
	int size=fsize(fd);
	char fileContent[size];

	read(fd,fileContent,size);

   char* token = strtok(fileContent, "\n");
   while( token != NULL ) {
      struct user* currUser=getUserFromString(token);
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

struct user* authenticateUser(char* username, char* password)
{
   struct user* userList=selectAllUsersFromPasswdFile();

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
   char path[64];
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

int isUidAvailable(int uid)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   while(userList!=NULL) {
      if(userList->uid==uid)
         return 0;

      userList=userList->next;
   }

   return 1;
}

int getNextAvailableUid() 
{
   struct user* userList=selectAllUsersFromPasswdFile();
   int uid=1000;

   while(userList!=NULL) {
      if(userList->uid==uid)
         uid++;

      userList=userList->next;
   }

   return uid;
}

struct user* getUserFromUid(int uid)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   while(userList!=NULL) {
      if(uid==userList->uid)
         return userList;

      userList=userList->next;
   }

   return NULL;
}

struct user* getUserFromUsername(char* username)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   while(userList!=NULL) {
      if(!strcmp(userList->username, username))
         return userList;

      userList=userList->next;
   }

   return NULL;
}

int authenticateOldPassword(struct user* currUser, char* oldPassword)
{
   if(!strcmp(currUser->password, oldPassword))
      return 1;

   return 0;
}

void updatePasswordForUser(struct user* currUser, char* newPassword)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   struct user* tmpUser=userList;
   while(tmpUser!=NULL) {
      if(tmpUser->uid==currUser->uid) {
         strcpy(tmpUser->password, newPassword);
         break;
      }

      tmpUser=tmpUser->next;
   }

   updatePasswdFile(userList);
}

void updatePasswdFile(struct user* userList)
{
   int fd=open("/etc/passwd",O_WRONLY);

   while(userList!=NULL) {
      char userString[192];
      getStringFromUser(userList, userString);
      write(fd, userString, strlen(userString));

      userList=userList->next;
   }

   close(fd);
}

void addNewUser(struct user* newUser)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   struct user* tmpUser=userList;
   while(tmpUser->next!=NULL)
      tmpUser=tmpUser->next;
   
   tmpUser->next=newUser;

   updatePasswdFile(userList);
}
