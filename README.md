# Standard Unix systems

Unix systems have user and user group concepts, and each file has a set of permissions (i.e., a mode) that describes which rights have which users and members of which user group over the file.

The code that deals with user management is divided into a part in the kernel and a part in the user space. In the kernel, users and groups are strictly numerical concepts. Each process has a UID (user id) and an EUID (effective user id) and each inode has a UID, GID and mode parameter. The kernel does not track which users and which groups exist. In other words, all UID and GID numbers are valid. In the user space, there is a library with routines for translating UIDs (ie GIDs) into text names of users (ie groups), and the user database is a text file / etc / passwd (and / etc / group for groups).

The library of authentication routines should be implemented in a new source file that will be added to the xv6 standard ULIB library which is linked to all user programs. New user programs should use these functions instead of each implementing user manipulation, etc.

# New user and group files

The file format /etc/passwd is as follows:
username: password: UID: GID: Real_Name: homedir

The format of the / etc / group file is as follows:
groupname: GID: user1, user2, user3

A user with UID 0 is special. It is typically called root (also called superuser in the literature). Root is the system administrator and can read and write from any file regardless of permissions (it cannot execute a program without execute permissions, but it can set execute permissions). In addition, only processes belonging to the root can make some system calls such as setuid () and chown ().

# System calls

-getuid and getuid
-setuid
-modified stat
-chmod, chown

# User programs

**getty** user program is the first program that starts after system boot. It asks the user for username and password and validates if the user is in the system. To exit the sh process that getty starts in the user's home directory just press CTRL+D.

**passwd** user program is used to change the user's password. Ordinary user can only change it's own password, while root user can change password of all users. The new password needs to be more than 6 characters long.
Syntax: passwd [username]

**useradd** user program adds new users to the system. The last argument is the user's username. With every new user, a group with the same name is added. This program doesn't set the user's password. Syntax: useradd [-d dir] [-u uid] [-c realname] username

**groupadd** user program adds new group to the system. Syntax: groupadd [-g gid] groupname

**usermod** user program modifies user's info. Syntax: usermod [-l login] [-u UID] [-c realname] [-d dir] [-m] [-G group…] [-a] username.

**chmod** user program modifies the file's permision.
