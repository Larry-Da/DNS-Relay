#ifndef dns_h
#define dns_h
enum TYPE{
    AAAA = 0x1c00,
    A = 0x0100,
    BLOCK = -1,
    NOT_EXIST = -2
};
typedef struct DNSHeader {
    unsigned short transID;
    unsigned short flags;
    unsigned short qdCnt;
    unsigned short anCnt;
    unsigned short nsCnt;
    unsigned short arCnt;
}DNSHeader;

typedef struct DNSRR {
    char *rName;
    short rType;
    short rClass;
    int ttl;
    short rdLen;
    char *rData;
    struct DNSRR *next;
}DNSRR;

typedef struct DNSQuestion {
    char *qName;
    short qType;
    short qClass;
    struct DNSQuestion *next;
}DNSQuestion;

typedef struct Response {
    DNSHeader *header;
    DNSQuestion *question;
    DNSRR *firstRR;
}Response;

extern void copy_string(char *des, char *src, int *cpy_cnt);
extern void* service_thread(void *arg);
extern enum TYPE normal_handling(Response **response, DNSHeader *header, DNSQuestion *question, char buf[], int *response_length, int recv_num);
extern enum TYPE search_database(char name[], char *resultIP[], enum TYPE requestType, int *resnum);
extern int ipv6convert(const char *point, unsigned int result[4]);


#endif /* dns_h */
