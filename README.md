Unix systems have user and user group concepts, and each file has a set of permissions (i.e., a mode) that describes which rights have which users and members of which user group over the file.

The code that deals with user management is divided into a part in the kernel and a part in the user space. In the kernel, users and groups are strictly numerical concepts. Each process has a UID (user id) and an EUID (effective user id) and each inode has a UID, GID and mode parameter. The kernel does not track which users and which groups exist. In other words, all UID and GID numbers are valid. In the user space, there is a library with routines for translating UIDs (ie GIDs) into text names of users (ie groups), and the user database is a text file / etc / passwd (and / etc / group for groups).

The library of authentication routines should be implemented in a new source file that will be added to the xv6 standard ULIB library which is linked to all user programs. New user programs should use these functions instead of each implementing user manipulation, etc.

The file format / etc / passwd is as follows:

username: password: UID: GID: Real_Name: homedir

Example file:

root: password: 0: 0: Superuser: / home / root
ken: abcd1234: 1000: 1000: Ken_Thompson: / home / ken
dmr: password1: 1001: 1001: Dennis_Ritchie: / home / dmr
bwk: 123456: 1002: 1002: Brian_Kernighan: / home / bwk

The format of the / etc / group file is as follows:

groupname: GID: user1, user2, user3

Example file:

root: 0: root
ken: 1000: ken
dmr: 1001: dmr
bwk: 1002: bwk
admin: 1: root, ken
users: 2: ken, dmr, bwk

Each user has a symbolic name, typically identical UID and GID values, a real name (which is just secondary data), and the location of the home directory. Note that for each user there is a trivial group of the same name.

A user with UID 0 is special. It is typically called root (also called superuser in the literature). Root is the system administrator and can read and write from any file regardless of permissions (it cannot execute a program without execute permissions, but it can set execute permissions). In addition, only processes belonging to the root can make some system calls such as setuid () and chown ().

The / etc directory is normally used for system configuration files, of which there are many on real systems. For the project, in addition to / etc / passwd and / etc / group, the files / etc / issue and / etc / motd must be added. They are plain text files that contain text that will be printed by the getty program. / etc / issue is printed before the “login:” message, and / etc / motd, ie. The "Message of The Day" is printed after the user successfully logs in, but before the shell is launched. Typically, the issue is a single line, and the motd is a short paragraph.

Added system calls:
-getuid and getuid
-setuid
-modified stat
-chmod, chown

Added user programs:
-getty
-passwd
-useradd
-groupadd
-usermod
-chmod
