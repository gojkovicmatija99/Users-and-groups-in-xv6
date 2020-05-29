#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "usergroups.h"

struct user* getUserFromString(char* userString)
{
	struct user* currUser=(struct user*)malloc(sizeof(struct user));
	char tmp[6][STRING_SIZE];	
   char buf[STRING_SIZE];

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

void getStringFromUser(struct user* currUser, char* userString)
{
   strcpy(userString, currUser->username);
   strcat(userString, ":");

   strcat(userString, currUser->password);
   strcat(userString, ":");

   char uidString[STRING_SIZE];
   itoa(currUser->uid, uidString, 10);
   strcat(userString, uidString);
   strcat(userString, ":");

   char gidString[STRING_SIZE];
   itoa(currUser->gid, gidString, 10);
   strcat(userString, gidString);
   strcat(userString, ":");

   strcat(userString, currUser->realname);
   strcat(userString, ":");

   strcat(userString, currUser->homedir);

   strcat(userString, "\n");
}

struct user* createUser(char* homedir, char* uidString, char* realname, char* username)
{
   struct user* newUser=(struct user*)malloc(sizeof(struct user));

   if(!isEmptyString(uidString)) {                       // if user has entered uid
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

   int gid=getNextAvailableGid();
   newUser->gid=gid;

   strcpy(newUser->homedir, "/home/");                    // all users are stored in "/home/"
   if(!isEmptyString(homedir))
      strcat(newUser->homedir, homedir);
   else
      strcat(newUser->homedir, username);

   int valid=mkdir(newUser->homedir);
   if(valid==-1)                                          // if dir isn't created, return error
      return NULL;
   else
      chown(newUser->homedir, newUser->uid, newUser->gid);

   strcpy(newUser->realname, realname);
   strcpy(newUser->username, username);

   return newUser;
}

struct user* addUserToListSorted(struct user* userList, struct user* currUser)
{
   if(currUser==NULL)
      return userList;

   if(userList==NULL)
      return currUser;

   if(compareUsers(userList, currUser)>0) {
      currUser->next=userList;
      return currUser;
   }

   struct user* tmpUser=userList;
   while(tmpUser->next!=NULL && (compareUsers(tmpUser->next, currUser)<=0))
      tmpUser=tmpUser->next;

   currUser->next=tmpUser->next;
   tmpUser->next=currUser;
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
   while( token != NULL) {
      struct user* currUser=getUserFromString(token);
      userList=addUserToListSorted(userList, currUser);

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

   struct user* tmpUser=userList;
   while(tmpUser!=NULL) {
      int valid=checkUsernamePasswordForCurrUser(username, password, tmpUser);
      if(valid)
         return tmpUser;

      tmpUser=tmpUser->next;
   }

   freeUserList(userList);
   return NULL;
}

int isUidAvailable(int uid)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   struct user* tmpUser=userList;
   while(tmpUser!=NULL) {
      if(tmpUser->uid==uid) {
         freeUserList(userList);
         return 0;
      }

      tmpUser=tmpUser->next;
   }

   freeUserList(userList);
   return 1;
}

int getNextAvailableUid() 
{
   struct user* userList=selectAllUsersFromPasswdFile();
   int uid=1000;

   struct user* tmpUser=userList;
   while(tmpUser!=NULL) {
      if(tmpUser->uid==uid)
         uid++;

      tmpUser=tmpUser->next;
   }

   freeUserList(userList);
   return uid;
}

struct user* getUserFromUid(int uid)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   struct user* tmpUser=userList;
   while(tmpUser!=NULL) {
      if(uid==tmpUser->uid) {
         tmpUser->next=NULL;             // return only this user, not the whole list
         return tmpUser;
      }

      tmpUser=tmpUser->next;
   }

   freeUserList(userList);
   return NULL;
}

struct user* getUserFromUsername(char* username)
{
   struct user* userList=selectAllUsersFromPasswdFile();

   struct user* tmpUser=userList;
   while(tmpUser!=NULL) {
      if(!strcmp(tmpUser->username, username)) {
         tmpUser->next=NULL;          // return only this user, not the whole list
         return tmpUser;
      }

      tmpUser=tmpUser->next;
   }

   freeUserList(userList);
   return NULL;
}

int authenticateOldPassword(struct user* currUser, char* oldPassword)
{
   if(!strcmp(currUser->password, oldPassword))
      return 1;

   return 0;
}

void updatePasswdFile(struct user* userList)
{
   unlink("/etc/passwd");

   open("/etc/passwd",O_CREATE);
   int fd=open("/etc/passwd",O_WRONLY);

   while(userList!=NULL) {
      char userString[STRING_SIZE*4];
      getStringFromUser(userList, userString);
      write(fd, userString, strlen(userString));

      userList=userList->next;
   }

   close(fd);
}

int compareUsers(struct user* user1, struct user* user2)
{
   if(user1->uid>user2->uid) return 1;
   if(user1->uid<user2->uid) return -1;
   return 0;
}

void addNewUser(struct user* newUser)
{
   struct user* userList=selectAllUsersFromPasswdFile();
   userList=addUserToListSorted(userList, newUser);
   updatePasswdFile(userList);
   freeUserList(userList);
}

int isUserInGroup(struct user* currUser, struct group* currGroup) 
{
   if(currGroup->userList==NULL)
      return 0;

   struct user* userList=currGroup->userList;
   while(userList!=NULL) {
      if(!compareUsers(userList, currUser))
         return 1;

      userList=userList->next;
   }

   return 0;
}

void addUserToGroups(struct group* groupsToAddUser, struct user* currUser)
{
   struct group* groupList=selectAllGroupsFromGroupFile();

   struct group* tmpGroup=groupList;
   while(tmpGroup!=NULL && groupsToAddUser!=NULL) {
      if(!compareGroups(groupsToAddUser, tmpGroup)) {
         if(!isUserInGroup(currUser, tmpGroup))
            tmpGroup->userList=addUserToListSorted(tmpGroup->userList, currUser);
         groupsToAddUser=groupsToAddUser->next;
      }

      tmpGroup=tmpGroup->next;
   }

   updateGroupFile(groupList);
   freeGroupList(groupList);
}

struct user* removeUserFromCurrGroup(struct group* currGroup, struct user* currUser)
{
   if(currGroup->userList==NULL)
      return NULL;

   if(!strcmp(currGroup->groupname, currUser->username))             // if this group is the users default group, exit
      return currGroup->userList;

   if(!compareUsers(currGroup->userList, currUser))
      return currGroup->userList->next;

   struct user* tmpUser=currGroup->userList;
   while(tmpUser->next!=NULL) {
      if(!compareUsers(tmpUser->next, currUser))
         tmpUser->next=tmpUser->next->next;

      tmpUser=tmpUser->next;
   }

   return currGroup->userList;
}

void removeUserFromAllGroups(struct user* currUser)
{
   struct group* groupList=selectAllGroupsFromGroupFile();

   struct group* tmpGroup=groupList;
   while(tmpGroup!=NULL) {
      tmpGroup->userList=removeUserFromCurrGroup(tmpGroup, currUser);

      tmpGroup=tmpGroup->next;
   }

   updateGroupFile(groupList);
   freeGroupList(groupList);
}

void freeUserList(struct user* userList)
{
   struct user* tmpUser;
   while(userList!=NULL) {
      tmpUser=userList;
      userList=userList->next;
      free(tmpUser);
   }
}

struct user* modifyUser(struct user* currUser, char* username, char* uidString, char* realname, char* homedir, char* m, char* groups, char* a)
{
   struct user* modUser=(struct user*)malloc(sizeof(struct user));

   strcpy(modUser->username, currUser->username);
   strcpy(modUser->password, currUser->password);
   modUser->uid=currUser->uid;
   modUser->gid=currUser->gid;
   strcpy(modUser->realname, currUser->realname);
   strcpy(modUser->homedir, currUser->homedir);
   modUser->next=NULL;

   if(!isEmptyString(username)) {
      if(getUserFromUsername(username)==NULL)               // check if username is available
         strcpy(modUser->username, username);
      else
         return NULL;
   }

   if(!isEmptyString(uidString)) {
      int uid=atoi(uidString);
      if(uid==0)                                            // if uidString isn't a number, return error
         return NULL;
      if(isUidAvailable(uid))                               // if uid isn't available, return error
         modUser->uid=uid;
      else
         return NULL;

      updateDirOwner(modUser->homedir, modUser->uid, modUser->gid);
   }

   if(!isEmptyString(realname))
      strcpy(modUser->realname, realname);

   if(!isEmptyString(groups)) {
      if(isEmptyString(a))
         removeUserFromAllGroups(currUser);

      struct group* groupsToAddUser=getMultipleGroupsFromString(groups);
      addUserToGroups(groupsToAddUser, currUser);
   }

   if(!isEmptyString(homedir)) {
      strcpy(modUser->homedir, "/home/");
      strcat(modUser->homedir, homedir);

      if(mkdir(modUser->homedir)==-1)                       // if dir isn't created, return error
         return NULL;

      int valid=chown(modUser->homedir, modUser->uid, modUser->gid);
      if(valid<0)
         return NULL;
   }

   return modUser;
}

void updateUserInfo(struct user* currUser, struct user* modUser) {
   struct user* userList=selectAllUsersFromPasswdFile();

   struct user* tmpUser=userList;
   while(tmpUser!=NULL) {
      if(!compareUsers(tmpUser, currUser)) {
         strcpy(tmpUser->username, modUser->username);
         strcpy(tmpUser->password, modUser->password);
         tmpUser->uid=modUser->uid;
         tmpUser->gid=modUser->gid;
         strcpy(tmpUser->realname, modUser->realname);
         strcpy(tmpUser->homedir, modUser->homedir);
         break;
      }

      tmpUser=tmpUser->next;
   }
   
   updatePasswdFile(userList);
   freeUserList(userList);
}