### How to find the IP of the VM ?

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