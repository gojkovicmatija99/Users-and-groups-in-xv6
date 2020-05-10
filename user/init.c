// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
	int pid, wpid;

	if(getpid() != 1){
		fprintf(2, "init: already running\n");
		exit();
	}

	if(open("/dev/console", O_RDWR) < 0){
		mknod("/dev/console", 1, 1);
		open("/dev/console", O_RDWR);
	}
	dup(0);  // stdout
	dup(0);  // stderr

	for(;;){
		printf("init: starting getty\n");
		pid = fork();
		if(pid < 0){
			printf("init: fork failed\n");
			exit();
		}
		if(pid == 0){
			while(1)
				exec("/bin/getty", argv);		// execute getty to ask for username and password, when getty is finised ask again
			printf("init: exec getty failed\n");
			exit();
		}
		while((wpid=wait()) >= 0 && wpid != pid)
			printf("zombie!\n");
	}
}
