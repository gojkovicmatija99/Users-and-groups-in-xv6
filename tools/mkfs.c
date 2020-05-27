#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat xv6_stat  // avoid clash with host struct stat
#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/usergroups.h"

#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

int nbitmap = FSSIZE/(BSIZE*8) + 1;
int ninodeblocks = NINODES / IPB + 1;
int nlog = LOGSIZE;
int nmeta;    // Number of meta blocks (boot, sb, nlog, inode, bitmap)
int nblocks;  // Number of data blocks

int fsfd;
struct superblock sb;
char zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

uint rootino;
uint homeino;
uint binino;
uint devino;
uint etcino;
uint homeRootino;

void balloc(int);
void wsect(uint, void*);
void winode(uint, struct dinode*);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type, int uid, int gid);
void iappend(uint inum, void *p, int n);

// convert to intel byte order
ushort
xshort(ushort x)
{
	ushort y;
	uchar *a = (uchar*)&y;
	a[0] = x;
	a[1] = x >> 8;
	return y;
}

uint
xint(uint x)
{
	uint y;
	uchar *a = (uchar*)&y;
	a[0] = x;
	a[1] = x >> 8;
	a[2] = x >> 16;
	a[3] = x >> 24;
	return y;
}

void getHomeDir(char* line)			// doesn't have a return type because strtok modifies string and sets pointer to the homedir
{
	char* token=strtok(line, ":");
	int i=0;
	while(i<5) {					// get the homedir from line
		token=strtok(NULL, ":");
		i++;
	}

	i=0;
	while(i<3) {					// get the directory name after "/home/"
		token=strtok(NULL, "/");
		i++;
	}
}

struct user* getUserFromString(char* userString)
{
	struct user* currUser=(struct user*)malloc(sizeof(struct user));
	char tmp[6][32];	
   char buf[32];

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

void makeUserHomeDirs(struct dirent de)
{
	FILE* file=fopen("passwd","r");
	char line[192];
	uint tmpino;
	struct user* tmpUser;

	while(fgets(line, sizeof(line), file) != NULL) {
		line[strlen(line)-1]='\0';			// remove new line				
		tmpUser=getUserFromString(line);
		getHomeDir(line);

		tmpino=ialloc(T_DIR, tmpUser->uid, tmpUser->gid);
		if(strcmp(line, "root")) {			// if homedir is "root", don't create it because it is already in system
			bzero(&de, sizeof(de));
			de.inum = xshort(tmpino);
			strcpy(de.name, ".");
			iappend(tmpino, &de, sizeof(de));

			bzero(&de, sizeof(de));
			de.inum = xshort(homeino);
			strcpy(de.name, "..");
			iappend(tmpino, &de, sizeof(de));

			bzero(&de, sizeof(de));
			de.inum = xshort(tmpino);
			strcpy(de.name, line);
			iappend(homeino, &de, sizeof(de));
		}
		
	}

	fclose(file);

}

void
makedirs(void)
{
	struct dirent de;

	// /
	rootino = ialloc(T_DIR, ROOT, ROOT);
	assert(rootino == ROOTINO);

	bzero(&de, sizeof(de));
	de.inum = xshort(rootino);
	strcpy(de.name, ".");
	iappend(rootino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(rootino);
	strcpy(de.name, "..");
	iappend(rootino, &de, sizeof(de));

	// /dev
	devino = ialloc(T_DIR, ROOT, ROOT);

	bzero(&de, sizeof(de));
	de.inum = xshort(devino);
	strcpy(de.name, ".");
	iappend(devino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(rootino);
	strcpy(de.name, "..");
	iappend(devino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(devino);
	strcpy(de.name, "dev");
	iappend(rootino, &de, sizeof(de));

	// /bin
	binino = ialloc(T_DIR, ROOT, ROOT);

	bzero(&de, sizeof(de));
	de.inum = xshort(binino);
	strcpy(de.name, ".");
	iappend(binino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(rootino);
	strcpy(de.name, "..");
	iappend(binino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(binino);
	strcpy(de.name, "bin");
	iappend(rootino, &de, sizeof(de));

	// /home
	homeino = ialloc(T_DIR, ROOT, ROOT);

	bzero(&de, sizeof(de));
	de.inum = xshort(homeino);
	strcpy(de.name, ".");
	iappend(homeino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(rootino);
	strcpy(de.name, "..");
	iappend(homeino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(homeino);
	strcpy(de.name, "home");
	iappend(rootino, &de, sizeof(de));

	// /home/root
	homeRootino = ialloc(T_DIR, ROOT, ROOT);

	bzero(&de, sizeof(de));
	de.inum = xshort(homeRootino);
	strcpy(de.name, ".");
	iappend(homeRootino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(homeino);
	strcpy(de.name, "..");
	iappend(homeRootino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(homeRootino);
	strcpy(de.name, "root");
	iappend(homeino, &de, sizeof(de));

	// /etc
	etcino = ialloc(T_DIR, ROOT, ROOT);

	bzero(&de, sizeof(de));
	de.inum = xshort(etcino);
	strcpy(de.name, ".");
	iappend(etcino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(rootino);
	strcpy(de.name, "..");
	iappend(etcino, &de, sizeof(de));

	bzero(&de, sizeof(de));
	de.inum = xshort(etcino);
	strcpy(de.name, "etc");
	iappend(rootino, &de, sizeof(de));

	makeUserHomeDirs(de);
}

int belongsToEtc(char* shortname)
{
	char* etcFiles[]={"passwd","motd","issue","group"};

	for(int i=0;i<4;i++)
		if(strcmp(shortname,etcFiles[i])==0)
			return 1;
	return 0;
}

int
main(int argc, char *argv[])
{
	int i, cc, fd;
	uint dirino, inum;
	struct dirent de;
	char buf[BSIZE];
	char *shortname;

	static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

	if(argc < 2){
		fprintf(stderr, "Usage: mkfs fs.img files...\n");
		exit(1);
	}

	assert((BSIZE % sizeof(struct dinode)) == 0);
	assert((BSIZE % sizeof(struct dirent)) == 0);

	fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(fsfd < 0){
		perror(argv[1]);
		exit(1);
	}

	// 1 fs block = 1 disk sector
	nmeta = 2 + nlog + ninodeblocks + nbitmap;
	nblocks = FSSIZE - nmeta;

	sb.size = xint(FSSIZE);
	sb.nblocks = xint(nblocks);
	sb.ninodes = xint(NINODES);
	sb.nlog = xint(nlog);
	sb.logstart = xint(2);
	sb.inodestart = xint(2+nlog);
	sb.bmapstart = xint(2+nlog+ninodeblocks);

	printf("nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
	        nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

	freeblock = nmeta;     // the first free block that we can allocate

	for(i = 0; i < FSSIZE; i++)
		wsect(i, zeroes);

	memset(buf, 0, sizeof(buf));
	memmove(buf, &sb, sizeof(sb));
	wsect(1, buf);

	makedirs();

	for(i = 2; i < argc; i++){
		// get rid of "user/"
		if(strncmp(argv[i], "user/", 5) == 0)
			shortname = argv[i] + 5;
		else
			shortname = argv[i];

		assert(index(shortname, '/') == 0);

		if((fd = open(argv[i], 0)) < 0){
			perror(argv[i]);
			exit(1);
		}

		dirino = homeRootino;

		// Skip leading _ in name when writing to file system.
		// The binaries are named _rm, _cat, etc. to keep the
		// build operating system from trying to execute them
		// in place of system binaries like rm and cat.
		if(shortname[0] == '_') {
			shortname += 1;
			// Binaries get copied into /bin
			dirino = binino;
		}
		else if(belongsToEtc(shortname))
			// If files belongs to etc, put it there
			// everything else goes into /home/root.
			dirino = etcino;
		
		inum = ialloc(T_FILE, ROOT, ROOT);

		bzero(&de, sizeof(de));
		de.inum = xshort(inum);
		strncpy(de.name, shortname, DIRSIZ);
		iappend(dirino, &de, sizeof(de));

		while((cc = read(fd, buf, sizeof(buf))) > 0)
			iappend(inum, buf, cc);

		close(fd);
	}

	balloc(freeblock);

	exit(0);
}

void
wsect(uint sec, void *buf)
{
	if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
		perror("lseek");
		exit(1);
	}
	if(write(fsfd, buf, BSIZE) != BSIZE){
		perror("write");
		exit(1);
	}
}

void
winode(uint inum, struct dinode *ip)
{
	char buf[BSIZE];
	uint bn;
	struct dinode *dip;

	bn = IBLOCK(inum, sb);
	rsect(bn, buf);
	dip = ((struct dinode*)buf) + (inum % IPB);
	*dip = *ip;
	wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
	char buf[BSIZE];
	uint bn;
	struct dinode *dip;

	bn = IBLOCK(inum, sb);
	rsect(bn, buf);
	dip = ((struct dinode*)buf) + (inum % IPB);
	*ip = *dip;
}

void
rsect(uint sec, void *buf)
{
	if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
		perror("lseek");
		exit(1);
	}
	if(read(fsfd, buf, BSIZE) != BSIZE){
		perror("read");
		exit(1);
	}
}

uint
ialloc(ushort type, int uid, int gid)
{
	uint inum = freeinode++;
	struct dinode din;

	bzero(&din, sizeof(din));
	din.type = xshort(type);
	din.nlink = xshort(1);
	din.size = xint(0);
	din.uid=uid;
	din.gid=gid;
	din.mode=0644;
	winode(inum, &din);
	return inum;
}

void
balloc(int used)
{
	uchar buf[BSIZE];
	int i;

	printf("balloc: first %d blocks have been allocated\n", used);
	assert(used < BSIZE*8);
	bzero(buf, BSIZE);
	for(i = 0; i < used; i++){
		buf[i/8] = buf[i/8] | (0x1 << (i%8));
	}
	printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
	wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(uint inum, void *xp, int n)
{
	char *p = (char*)xp;
	uint fbn, off, n1;
	struct dinode din;
	char buf[BSIZE];
	uint indirect[NINDIRECT];
	uint x;

	rinode(inum, &din);
	off = xint(din.size);
	// printf("append inum %d at off %d sz %d\n", inum, off, n);
	while(n > 0){
		fbn = off / BSIZE;
		assert(fbn < MAXFILE);
		if(fbn < NDIRECT){
			if(xint(din.addrs[fbn]) == 0){
				din.addrs[fbn] = xint(freeblock++);
			}
			x = xint(din.addrs[fbn]);
		} else {
			if(xint(din.addrs[NDIRECT]) == 0){
				din.addrs[NDIRECT] = xint(freeblock++);
			}
			rsect(xint(din.addrs[NDIRECT]), (char*)indirect);
			if(indirect[fbn - NDIRECT] == 0){
				indirect[fbn - NDIRECT] = xint(freeblock++);
				wsect(xint(din.addrs[NDIRECT]), (char*)indirect);
			}
			x = xint(indirect[fbn-NDIRECT]);
		}
		n1 = min(n, (fbn + 1) * BSIZE - off);
		rsect(x, buf);
		bcopy(p, buf + off - (fbn * BSIZE), n1);
		wsect(x, buf);
		n -= n1;
		off += n1;
		p += n1;
	}
	din.size = xint(off);
	winode(inum, &din);
}
