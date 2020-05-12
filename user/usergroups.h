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