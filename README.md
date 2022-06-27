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
export net_ip=xxx.xxx.xxx.xxx
export debug_level=2
```

2. Compile 

```bash
make dnsrelay
```

3. Run

```bash
make deploy
```



