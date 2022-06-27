# DNS-Relay
A DNS relay server for DNS lookup cache. It is a course project instead of commercial software. LIMITED VERIFIED! 



### Features

1. DNS lookup. It supports IPv4/IPv6 IP lookup.
2. Different debugging level, which is used to record the DNS request information.
3. Memory cache for DNS table. It uses AVL tree for DNS record CRUD.





### Quick Start

1. Set default DNS Server IP and debug_level

eg.

```bash
export net_ip=192.168.3.1
export debug_level=2
```

> You have to change the net_ip to you local DNS Server IP!

2. Compile 

```bash
make dnsrelay
```

3. Run

```bash
make deploy
```



Then you can use the DNS Relay like this:

```bash
$ nslookup www.bupt.edu.cn 127.0.0.1 
Server:		127.0.0.1
Address:	127.0.0.1#53

Non-authoritative answer:
www.bupt.edu.cn	canonical name = vn46.bupt.edu.cn.
Name:	vn46.bupt.edu.cn
Address: 211.68.69.240
```

The DNS relay will show the info:

```bash
-------------------------------------------------------
1 : Mon Jun 27 11:44:21 2022
[ID : 9692]
127.0.0.1	www.bupt.edu.cn	TYPE 1	CLASS 1

RECV from 127.0.0.1 (33 bytes) [ Client ]
96 92 01 00 00 01 00 00 00 00 00 00 03 77 77 77 04 62 75 70 74 03 65 64 75 02 63 6e 00 00 01 00 01 
QR: 0	Opcode: 0	AA: 0	TC: 0	RD: 1	RA: 0	Z: 0	Rcode: 0	
Question count: 1	Answer count: 0	Authority count: 0	Additional count: 0	
SEND to 192.168.3.1 (33 bytes) [ Default DNS Server ]
96 92 01 00 00 01 00 00 00 00 00 00 03 77 77 77 04 62 75 70 74 03 65 64 75 02 63 6e 00 00 01 00 01 
QR: 0	Opcode: 0	AA: 0	TC: 0	RD: 1	RA: 0	Z: 0	Rcode: 0	
Question count: 1	Answer count: 0	Authority count: 0	Additional count: 0	
SEND to 192.168.3.1 (199 bytes) [ Default DNS Server ]
96 92 81 80 00 01 00 02 00 02 00 04 03 77 77 77 04 62 75 70 74 03 65 64 75 02 63 6e 00 00 01 00 01 c0 0c 00 05 00 01 00 00 0d 5b 00 07 04 76 6e 34 36 c0 10 c0 2d 00 01 00 01 00 00 1b f9 00 04 d3 44 45 f0 c0 10 00 02 00 01 00 00 02 7e 00 0d 02 6e 73 07 62 75 70 74 6e 65 74 c0 15 c0 10 00 02 00 01 00 00 02 7e 00 06 03 67 75 73 c0 53 c0 50 00 01 00 01 00 00 65 a2 00 04 ca 70 0a 25 c0 69 00 01 00 01 00 02 9d cf 00 04 ca 70 0a 24 c0 50 00 1c 00 01 00 00 56 8a 00 10 20 01 0d a8 02 02 00 10 00 00 00 00 00 00 00 37 c0 69 00 1c 00 01 00 00 56 8a 00 10 20 01 0d a8 02 02 00 10 00 00 00 00 00 00 00 36 
QR: 1	Opcode: 0	AA: 0	TC: 0	RD: 1	RA: 1	Z: 0	Rcode: 0	
Question count: 1	Answer count: 2	Authority count: 2	Additional count: 4	
SEND to 127.0.0.1 (199 bytes) [ Client ]
96 92 81 80 00 01 00 02 00 02 00 04 03 77 77 77 04 62 75 70 74 03 65 64 75 02 63 6e 00 00 01 00 01 c0 0c 00 05 00 01 00 00 0d 5b 00 07 04 76 6e 34 36 c0 10 c0 2d 00 01 00 01 00 00 1b f9 00 04 d3 44 45 f0 c0 10 00 02 00 01 00 00 02 7e 00 0d 02 6e 73 07 62 75 70 74 6e 65 74 c0 15 c0 10 00 02 00 01 00 00 02 7e 00 06 03 67 75 73 c0 53 c0 50 00 01 00 01 00 00 65 a2 00 04 ca 70 0a 25 c0 69 00 01 00 01 00 02 9d cf 00 04 ca 70 0a 24 c0 50 00 1c 00 01 00 00 56 8a 00 10 20 01 0d a8 02 02 00 10 00 00 00 00 00 00 00 37 c0 69 00 1c 00 01 00 00 56 8a 00 10 20 01 0d a8 02 02 00 10 00 00 00 00 00 00 00 36 
QR: 1	Opcode: 0	AA: 0	TC: 0	RD: 1	RA: 1	Z: 0	Rcode: 0	
Question count: 1	Answer count: 2	Authority count: 2	Additional count: 4	
```

