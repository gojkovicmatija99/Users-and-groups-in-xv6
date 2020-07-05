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
-getty
-passwd
-useradd
-groupadd
-usermod
-chmod
