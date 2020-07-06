# Standard Unix systems

Unix systems have user and user group concepts, and each file has a set of permissions (i.e., a mode) that describes which rights have which users and members of which user group over the file.

The code that deals with user management is divided into a part in the kernel and a part in the user space. In the kernel, users and groups are strictly numerical concepts. Each process has a UID (user id) and an EUID (effective user id) and each inode has a UID, GID and mode parameter. The kernel does not track which users and which groups exist. In other words, all UID and GID numbers are valid. In the user space, there is a library with routines for translating UIDs (ie GIDs) into text names of users (ie groups), and the user database is a text file /etc/ passwd (and /etc/group for groups).

The library of authentication routines should be implemented in a new source file that will be added to the xv6 standard ULIB library which is linked to all user programs. New user programs should use these functions instead of each implementing user manipulation, etc.

# New user and group files

The file format /etc/passwd is as follows:
username: password: UID: GID: realname: homedir

The format of the /etc/group file is as follows:
groupname: GID: user1, user2, user3

A user with UID 0 is special. It is typically called root (also called superuser in the literature). Root is the system administrator and can read and write from any file regardless of permissions (it cannot execute a program without execute permissions, but it can set execute permissions). In addition, only processes belonging to the root can make some system calls such as setuid () and chown ().

# Permsions

Each file in the inode structure has one int that serves as a bitfield permission that describes the users who have rights to that file. This parameter is often called file mode. In its higher bits we usually store additional special features of the file, in our case only the setuid bit. The file mode is usually illustrated in two ways: in octal form and in drwxrwxrwx form. Examples:

0755 = -rwxr-xr-x

# System calls

- getuid
- getuid 
- setuid 
- modified stat
- chmod
- chown

# User programs

## getty
**getty** user program is the first program that starts after system boot. It asks the user for username and password and validates if the user is in the system. To exit the sh process that getty starts in the user's home directory just press CTRL+D.

## passwd [username]
Passwd user program is used to change the user's password. Ordinary user can only change it's own password, while root user can change password of all users.When the user wants to change it's own password, there is no need to input the username, while when the root user wants to change another users password, the username is mandatory. When changing passwords, the ordinary user needs to input the old password, while the root user can skip this step. The new password needs to be more than 6 characters long.

## useradd [-d dir] [-u uid] [-c realname] username
**useradd** user program adds new users to the system. The last argument is the user's username. With every new user, a group with the same name is added. This program doesn't set the user's password. 
- -d option is used to set the new user's home directory. If this option is left out the default home directory is /home/[username]
- -u option is used to set the user's uid. This value needs to be unique. Default value is the minumum available number that is greater than 1000.
- -c option is to set the user's realname.

## groupadd [-g gid] groupname
**groupadd** user program adds new group to the system. The last argument is the group's groupname.
- -g option is used to set the group's gid. This value needs to be unique. Default value is the minumum available number that is greater than 1000.
## usermod [-l login] [-u UID] [-c realname] [-d dir] [-m] [-G group…] [-a] username
**usermod** user program modifies user's info. The last argument is the user's username.
- -l option is used to change the user's username. This value needs to be unique.
- -u option is used to change the user's uid. This value needs to be unique.
- -c option is used to change the user's realname.
- -d option is used to change the user's home directory.
- -G option is used to add user to one or more groups. Groups are separated by commas. If the user is already in some groups besides it's own, it needs to be removed from those groups. If the option -a is used, then the user stays in his previous groups.

## chmod mode file...
**chmod** user program modifies the file's permision.Mode is composed out of three parts. The first part is to whome the operation is refering to (a - all users, u - file owner, g - groups, o - other users). The second part is what operation is the command using (+ add the privilege, - removes the privilege). The last part is which priviledge is the command refering to (r - read, w - write, x - execute). Examples:
- chmod a+x file1 - adds execute privilege to all users.
- chmod o-w file1 file2 - removes write privilage from other users.
