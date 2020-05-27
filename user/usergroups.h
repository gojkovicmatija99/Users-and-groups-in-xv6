#define NULL 0
#define ROOT 0   // uid of the root user

#define ADD_USER 		2
#define	DONT_ADD_USER 	1

#define SETUID 		8
#define READ 		4
#define WRITE 		2
#define EXECUTE 	1

#define STRING_SIZE 32

struct user {
	char username[STRING_SIZE];
	char password[STRING_SIZE];
	int uid;
	int gid;
	char realname[STRING_SIZE];
	char homedir[STRING_SIZE]; 
	struct user* next;
};

struct group {
	char groupname[STRING_SIZE];
	int gid;
	struct user* userList;
	struct group* next;
};

//	Working with users
struct user* selectAllUsersFromPasswdFile();
struct user* getUserFromString(char* userString);
struct user* getUserFromUid(int uid);
struct user* getUserFromUsername(char* username);
struct user* createUser(char* homedir, char* uid, char* realname, char* username);
struct user* authenticateUser(char* username, char* password);
struct user* addUserToListSorted(struct user* userList, struct user* currUser);
struct user* removeUserFromGroup(struct group* currGroup, struct user* currUser);
struct user* modifyUser(struct user* currUser, char* username, char* uidString, char* realname, char* homedir, char* m, char* groups, char* a);
int checkUsernamePasswordForCurrUser(char* username, char* password, struct user* currUser);
int authenticateOldPassword(struct user* currUser, char* oldPassword);
int isUidAvailable(int uid);
int getNextAvailableUid();
int compareUsers(struct user* user1, struct user* user2);
int isUserInGroup(struct user* currUser, struct group* currGroup);
void updatePasswdFile(struct user* userList);
void getStringFromUser(struct user* currUser, char* userString);
void updateUserInfo(struct user* currUser, struct user* modUser);
void addUserToGroups(struct group* groupsToAddUser, struct user* currUser);
void addNewUser(struct user* newUser);
void removeUserFromAllGroups(struct user* currUser);
void freeUserList(struct user* userList);

//	Working with groups
struct group* selectAllGroupsFromGroupFile();
struct group* createGroup(char* groupname, char* gidString, int addUserWithSameGroupname);
struct group* getGroupFromString(char* groupString);
struct group* addGroupToListSorted(struct group* groupList, struct group* currGroup);
struct group* getMultipleGroupsFromString(char* groupString);
struct group* getGroupFromGid(int gid);
void updateGroupFile(struct group* groupList);
void getStringFromGroup(struct group* currGroup, char* groupString);
void addNewGroup(struct group* newGroup);
void printEtcFile(char* file);
void freeGroupList(struct group* groupList);
int isGidAvailable(int gid);
int getNextAvailableGid();
int compareGroups(struct group* group1, struct group* group2);

//	Working with permision
void getPermisionsString(int permisions, short type, char* permisionsString);
int convertOctalToDecimal(int octalNumber);
int convertStringToMode(char* modeString, int oldMode);