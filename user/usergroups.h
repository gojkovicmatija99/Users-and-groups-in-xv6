#define NULL 0
#define ROOT 0   // uid of the root user

struct user {
	char username[32];
	char password[32];
	int uid;
	int gid;
	char realname[32];
	char homedir[64]; 
	struct user* next;
};

struct group {
	char groupname[32];
	int gid;
	struct user* users[64];
	struct group* next;
};

struct user* selectAllUsersFromPasswdFile();
void updatePasswdFile(struct user* userList);

struct user* getUserFromString(char* userString);
struct user* getUserFromUid(int uid);
struct user* getUserFromUsername(char* username);
struct user* createUser(char* homedir, char* uid, char* realname,char* username);
struct user* addUserToList(struct user* userList, struct user* currUser);
struct user* authenticateUser(char* username, char* password);
int checkUsernamePasswordForCurrUser(char* username, char* password, struct user* currUser);
int isUidAvailable(int uid);
int getNextAvailableUid();
int authenticateOldPassword(struct user* currUser, char* oldPassword);
void updatePasswordForUser(struct user* user, char* newPassword);
void addNewUser(struct user* newUser);
void printEtcFile(char* file);
char* getStringFromUser(struct user* currUser, char* userString);