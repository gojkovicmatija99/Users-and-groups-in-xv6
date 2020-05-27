#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user.h"
#include "usergroups.h"

struct group* getGroupFromString(char* groupString) 
{
   struct group* currGroup=(struct group*)malloc(sizeof(struct group));
   char tmp[12][STRING_SIZE];
   char buf[STRING_SIZE];

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
      currGroup->userList=addUserToListSorted(currGroup->userList, currUser);
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

   char gidString[STRING_SIZE];
   itoa(currGroup->gid, gidString, 10);
   strcat(groupString, gidString);
   strcat(groupString, ":");

   int isFirstUser=1;
   struct user* tmpUser=currGroup->userList;
   while(tmpUser!=NULL) {
      if(!isFirstUser)                         // the first user doesn't have an ',' before it, other users do
         strcat(groupString, ",");
      else
         isFirstUser=0;
      strcat(groupString, tmpUser->username);

      tmpUser=tmpUser->next;
   }
   strcat(groupString, "\n");
}

struct group* createGroup(char* groupname, char* gidString, int addUserWithSameGroupname)
{
   struct group* newGroup=(struct group*)malloc(sizeof(struct group));

   strcpy(newGroup->groupname, groupname);
   if(!isEmptyString(gidString)) {
      int gid=atoi(gidString);                      // if user has entered gid
      if(gid==0)                          
         return NULL;                                    // if gidString isn't a number, return error
      if(isGidAvailable(gid))                            // if gid isn't available, return error
         newGroup->gid=gid;
      else
         return NULL;
   }
   else {
      int gid=getNextAvailableGid();                // else get available gid that is greater than 1000
      newGroup->gid=gid;
   }
   
   if(addUserWithSameGroupname==ADD_USER) {
      struct user* currUser=getUserFromUsername(groupname);
      newGroup->userList=currUser;
   }
   else if(addUserWithSameGroupname==DONT_ADD_USER)
      newGroup->userList=NULL;

   return newGroup;
}

int compareGroups(struct group* group1, struct group* group2)
{
	if(group1->gid>group2->gid) return 1;
	if(group1->gid<group2->gid) return -1;
	return 0;
}

struct group* addGroupToListSorted(struct group* groupList, struct group* currGroup)
{ 
   if(groupList==NULL)
   		return currGroup;

   if(compareGroups(groupList, currGroup)>0) {
   	currGroup->next=groupList;
   	return currGroup;
   }

   struct group* tmpGroup=groupList;
   while(tmpGroup->next!=NULL && (compareGroups(tmpGroup->next, currGroup)<=0))
   	tmpGroup=tmpGroup->next;

   currGroup->next=tmpGroup->next;
   tmpGroup->next=currGroup;
   return groupList;
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
      groupList=addGroupToListSorted(groupList, currGroup);

      token = strtok2(NULL, "\n");
   }

   close(fd);
   return groupList;
}

void printEtcFile(char* file)
{
   char path[STRING_SIZE];
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

int isGidAvailable(int gid)
{
   struct group* groupList=selectAllGroupsFromGroupFile();

   struct group* tmpGroup=groupList;
   while(tmpGroup!=NULL) {
      if(tmpGroup->gid==gid) {
         freeGroupList(groupList);
         return 0;
      }

      tmpGroup=tmpGroup->next;
   }

   freeGroupList(groupList);
   return 1;
}

int getNextAvailableGid() 
{
   struct group* groupList=selectAllGroupsFromGroupFile();
   int gid=1000;

   struct group* tmpGroup=groupList;
   while(tmpGroup!=NULL) {
      if(tmpGroup->gid==gid)
         gid++;

      tmpGroup=tmpGroup->next;
   }

   freeGroupList(groupList);
   return gid;
}

void updateGroupFile(struct group* groupList)
{
   unlink("/etc/group");

   open("/etc/group",O_CREATE);
   int fd=open("/etc/group",O_WRONLY);

   while(groupList!=NULL) {
      char groupString[STRING_SIZE*16];
      getStringFromGroup(groupList, groupString);
      write(fd, groupString, strlen(groupString));

      groupList=groupList->next;
   }

   close(fd);
}

void addNewGroup(struct group* newGroup)
{
   struct group* groupList=selectAllGroupsFromGroupFile();
   groupList=addGroupToListSorted(groupList, newGroup);
   updateGroupFile(groupList);
   freeGroupList(groupList);
}

void freeGroupList(struct group* groupList)
{
   struct group* tmpGroup;
   while(groupList!=NULL) {
      tmpGroup=groupList;
      groupList=groupList->next;
      free(tmpGroup);
   }
}

struct group* getGroupFromGroupname(char* groupname)
{
	struct group* groupList=selectAllGroupsFromGroupFile();

   struct group* tmpGroup=groupList;
	while(tmpGroup!=NULL) {
		if(!strcmp(tmpGroup->groupname, groupname)) {
			tmpGroup->next=NULL;               // return only this group, not the whole list
			return tmpGroup;
		}

		tmpGroup=tmpGroup->next;
	}

   freeGroupList(groupList);
	return NULL;
}

struct group* getGroupFromGid(int gid)
{
   struct group* groupList=selectAllGroupsFromGroupFile();

   struct group* tmpGroup=groupList;
   while(tmpGroup!=NULL) {
      if(gid==tmpGroup->gid) {
         tmpGroup->next=NULL;               // return only this group, not the whole list
         return tmpGroup;
      }

      tmpGroup=tmpGroup->next;
   }

   freeGroupList(groupList);
   return NULL;
}

struct group* getMultipleGroupsFromString(char* groupsString) 
{
   struct group* groupList=NULL;
   char buf[STRING_SIZE];

   int pnt=0;              // pnt points to the absolute position in the string
   while(1) {              // parses groups with delimiter ','
      int curr=0;          // curr points to the relative position in the string
      while(groupsString[curr+pnt]!=',' && groupsString[curr+pnt]!='\0') {
         buf[curr]=groupsString[curr+pnt];
         curr++;
      }
      buf[curr]='\0';
      
      struct group* currGroup=getGroupFromGroupname(buf);
      if(currGroup!=NULL)                                   // Implement: Report error
         groupList=addGroupToListSorted(groupList, currGroup);
      if(groupsString[curr+pnt]=='\0')
         break;
      else
         pnt+=curr+1;
   }
   return groupList;
}