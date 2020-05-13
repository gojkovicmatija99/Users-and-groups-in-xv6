#define NULL 0

struct user {
	char username[32];
	char password[32];
	int uid;
	int gid;
	char realName[32];
	char homedir[64]; 
	struct user* next;
};

struct group {
	char groupname[32];
	int gid;
	struct user* users[64];
	struct group* next;
};

struct user* getUserFromString(char* userString);
struct user* createUser(char* homedir, char* uid, char* realname,char* username);
struct user* addUserToList(struct user* userList, struct user* currUser);
struct user* getAllUsersFromPasswdFile();
int checkUsernamePasswordForCurrUser(char* username, char* password, struct user* currUser);
struct user* authenticateUser(char* username, char* password);
void printEtcFile(char* file);
int isUidAvailable(int uid);
int getNextAvailableUid();
void addNewUserToPasswdFile(newUser);
char* getStringFromUser(struct user* currUser);