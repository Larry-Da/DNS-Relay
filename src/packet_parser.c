#include <stdlib.h>
#include <string.h>
#include "dns.h"
#include <stdio.h>
#include "debugging_info.h"
void convert_string(char src[], char des[])
{
    int resnum = src[0];
    int i;
    int len = strlen(src);
    for(i = 0; i < len - 1; i++)
    {
        if(resnum == 0)
        {
            des[i] = '.';
            resnum = src[i + 1];
        }
        else
        {
            des[i] = src[i + 1];
            resnum--;
        }

    }
    des[i] = 0;    
}
int parse_query(char packet[], DNSHeader* header, DNSQuestion* question)
{
    char *buf = packet;
    short times = header->qdCnt;
    convertEndian((char *)&times, 2);
    int pos = 12;
    for(int i = 0; i < times; i++)
    {
        char *temp = malloc(sizeof(char) * 100);
        strcpy(temp, &buf[pos]);
        pos += 1 + strlen(temp);
        question->qName = temp;
        question->qType = *(short *)(&buf[pos]);
        pos += 2;
        question->qClass = *(short *)(&buf[pos]);
        pos += 2;
        
        if(i == times - 1)
            question->next = NULL;
        else
        {
            question->next = malloc(sizeof(DNSQuestion));
            question = question->next;
        }
    }
    return pos;
}
int parse_response(char packet[], Response* response)
{
    
    char * buf = packet;
    response->question = malloc(sizeof(DNSQuestion));
    response->header = (DNSHeader *)packet;
    int pos = parse_query(packet, response->header, response->question);
    short anCnt = response->header->anCnt;
    short nsCnt = response->header->nsCnt;
    short arCnt = response->header->arCnt;
    convertEndian((char *)&anCnt, sizeof(anCnt));
    convertEndian((char *)&nsCnt, sizeof(nsCnt));
    convertEndian((char *)&arCnt, sizeof(arCnt));
    int times = anCnt + nsCnt + arCnt;
    response->firstRR = malloc(sizeof(DNSRR));
    DNSRR* p = response->firstRR;
    for(int i = 0 ; i < times; i++)
    {
        p->rName = malloc(sizeof(char) * 100);
        unsigned char prefix = buf[pos];
        if(prefix == 0xC0)
        {
            p->rName[0] = 0xC0;
            p->rName[1] = buf[pos + 1];
            pos += 2;
        }
        else
        {
            strcpy(p->rName, &buf[pos]);

            pos += 1 + strlen(p->rName);
        }
        p->rType = *(short *)(&buf[pos]);
        pos += 2;
        p->rClass = *(short *)(&buf[pos]);
        pos += 2;
        p->ttl = *(int *)(&buf[pos]);
        pos += 4;
        p->rdLen = *(short *)(&buf[pos]);
        short rdLen = p->rdLen;
        convertEndian((char *)&rdLen, sizeof(rdLen));
        pos += 2;

        p->rData = malloc(sizeof(char) * rdLen);
        for(int j = 0; j < rdLen; j++)
        {
            p->rData[j] = buf[pos];
            pos++;
        }

        if(i == times - 1)
            p->next = NULL;
        else
        {
            p->next = malloc(sizeof(DNSRR));
            p = p->next;
        }
    }
    return pos;

}
