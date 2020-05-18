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
   for(int i=0;i<6;i++) {     // parses user info with delimiter ':'
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

struct group* getGroupFromString(char* groupString) {
   struct group* currGroup=(struct group*)malloc(sizeof(struct group));
   char tmp[13][64];
   char buf[64];

   int pnt=0;                 // pnt points to the absolute position in the string
   for(int i=0;i<2;i++) {     // parses group info with delimiter ':'
      int curr=0;             // curr points to the relative position in the string
      while(groupString[curr+pnt]!=':' && groupString[curr+pnt]!='\0') {
         buf[curr]=groupString[curr+pnt];
         curr++;
      }
      buf[curr]='\0';
      strcpy(tmp[i],buf);
      pnt+=curr+1;         // pnt points to where curr stoped + 1 to skip ':'
   }

   strcpy(currGroup->groupname, tmp[0]);
   currGroup->gid=atoi(tmp[1]);

   currGroup->userList=NULL;
   while(1) {             // parses users with delimiter ','
      int curr=0;
      while(groupString[curr+pnt]!=',' && groupString[curr+pnt]!='\0') {
         buf[curr]=groupString[curr+pnt];
         curr++;
      }
       buf[curr]='\0';
      
      struct user* currUser=getUserFromUsername(buf);
      currGroup->userList=addUserToList(currGroup->userList, currUser);
      if(groupString[curr+pnt]=='\0')
         break;
      else
         pnt+=curr+1;
   }

   currGroup->next=NULL;

   return currGroup;
}

void getStringFromGroup(struct group* currGroup, char* groupString)
{
   strcpy(groupString, currGroup->groupname);
   strcat(groupString, ":");

   char gidString[32];
   itoa(currGroup->gid, gidString, 10);
   strcat(groupString, gidString);
   strcat(groupString, ":");

   int isFirstUser=1;
   struct user* tmpUser=currGroup->userList;
   while(tmpUser!=NULL) {
      if(!isFirstUser)
         strcat(groupString, ",");
      else
         isFirstUser=0;
      strcat(groupString, tmpUser->username);

      tmpUser=tmpUser->next;
   }

   strcat(groupString, "\n");
}

void getStringFromUser(struct user* currUser, char* userString)
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

   newUser->gid=newUser->uid;
   strcpy(newUser->realname, realname);
   strcpy(newUser->username, username);

   return newUser;

}

struct user* addUserToList(struct user* userList, struct user* currUser)
{
   if(userList==NULL)
      return currUser;

   struct user* tmpUser=userList;
   while(tmpUser->next!=NULL)
      tmpUser=tmpUser->next;

   tmpUser->next=currUser;
   return userList;
}

struct group* addGroupToList(struct group* groupList, struct group* currGroup)
{
   if(groupList==NULL)
      return currGroup;

   struct group* tmpGroup=groupList;
   while(tmpGroup->next!=NULL)
      tmpGroup=tmpGroup->next;

   tmpGroup->next=currGroup;
   return groupList;
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
      userList=addUserToList(userList, currUser);

      token = strtok(NULL, "\n");
   }

   close(fd);
   return userList;
}

struct group* selectAllGroupsFromGroupFile()
{
   struct group* groupList=NULL;

   int fd=open("/etc/group",O_RDONLY);
   int size=fsize(fd);
   char fileContent[size];

   read(fd,fileContent,size);

   char* token = strtok2(fileContent, "\n");
   while( token != NULL ) {
      struct group* currGroup=getGroupFromString(token);
      groupList=addGroupToList(groupList, currGroup);

      token = strtok2(NULL, "\n");
   }

   close(fd);
   return groupList;
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
      if(uid==userList->uid) {
         userList->next=NULL;             // return only this user, not the whole list
         return userList;
      }

      userList=userList->next;
   }

   return NULL;
}

struct user* getUserFromUsername(char* username)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   while(userList!=NULL) {
      if(!strcmp(userList->username, username)) {
         userList->next=NULL;          // return only this user, not the whole list
         return userList;
      }

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

void updateGroupFile(struct group* groupList)
{
   int fd=open("/etc/group",O_WRONLY);

   while(groupList!=NULL) {
      char groupString[512];
      getStringFromGroup(groupList, groupString);
      write(fd, groupString, strlen(groupString));

      groupList=groupList->next;
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

void addNewGroup(struct group* newGroup)
{
   struct group* groupList=selectAllGroupsFromGroupFile();

   struct group* tmpGroup=groupList;
   while(tmpGroup->next!=NULL)
      tmpGroup=tmpGroup->next;

   tmpGroup->next=newGroup;

   updateGroupFile(groupList);
}

struct group* createGroup(char* groupname, int gid)
{
   struct group* newGroup=(struct group*)malloc(sizeof(struct group));

   strcpy(newGroup->groupname, groupname);
   newGroup->gid=gid;
   struct user* currUser=getUserFromUid(gid);
   newGroup->userList=currUser;

   return newGroup;

}