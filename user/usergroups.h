#define NULL 0

struct user {
	char username[20];
	char password[20];
	int uid;
	int gid;
	char realName[20];
	char homedir[20]; 
	struct user* next;
};

struct group {
	char groupname[20];
	int gid;
	struct user* users[20];
	struct group* next;
};

char* uidToUsername(int uid);
char* gidToGroupname(int gid);
struct user* getAllUsers();
struct user* authenticateUser(char* username,char* password);
void printEtcFile(char* file);