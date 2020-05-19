#define NULL 0
#define ROOT 0   // uid of the root user

#define ADD_USER 1
#define	DONT_ADD_USER 2

struct user {
	char username[32];
	char password[32];
	int uid;
	int gid;
	char realname[32];
	char homedir[32]; 
	struct user* next;
};

struct group {
	char groupname[32];
	int gid;
	struct user* userList;
	struct group* next;
};

//	Working with etc dir (passwd, group, issue, motd)
struct user* selectAllUsersFromPasswdFile();
struct group* selectAllGroupsFromGroupFile();
void updatePasswdFile(struct user* userList);
void updateGroupFile(struct group* groupList);
void printEtcFile(char* file);

// Working with lists
struct user* addUserToList(struct user* userList, struct user* currUser);
struct group* addGroupToList(struct group* groupList, struct group* currGroup);

//	Working with users
struct user* getUserFromString(char* userString);
struct user* getUserFromUid(int uid);
struct user* getUserFromUsername(char* username);
struct user* createUser(char* homedir, char* uid, char* realname, char* username);
struct user* authenticateUser(char* username, char* password);
int checkUsernamePasswordForCurrUser(char* username, char* password, struct user* currUser);
int authenticateOldPassword(struct user* currUser, char* oldPassword);
int isUidAvailable(int uid);
int getNextAvailableUid();
void updatePasswordForUser(struct user* user, char* newPassword);
void addNewUser(struct user* newUser);
void getStringFromUser(struct user* currUser, char* userString);
struct user* modifyUser(struct user* currUser, char* username, char* uidString, char* realname, char* homedir, char* m, char* groups, char* a);
void updateUserInfo(struct user* currUser, struct user* modUser);

//	Working with groups
struct group* createGroup(char* groupname, char* gidString, int addUserWithSameGroupname);
struct group* getGroupFromString(char* groupString);
void getStringFromGroup(struct group* currGroup, char* groupString);
void addNewGroup(struct group* newGroup);
int isGidAvailable(int gid);
int getNextAvailableGid();