#include <stdio.h>
#include "dns.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include "debugging_info.h"
#include "packet_parser.h"
#include "AVL_Cache.h"
#include <pthread.h>
#include <sys/wait.h>

extern pthread_mutex_t mutex_lock;
extern char *serverIP;
int get_local_success = 0;
void *communicate_with_local_DNS(void * buffer);

enum TYPE normal_handling(Response **response, DNSHeader *header, DNSQuestion *question, char buf[], int *response_length, int recv_num)
{
    char* query_name = malloc(sizeof(char) * 100);
    
    convert_string(question->qName, query_name);
    
    char **resultIP = malloc(sizeof(char *) * 10);
    for(int i = 0 ; i < 10; i++)
    {
        resultIP[i] = malloc(sizeof(char) * 50);
    }
    
    char type[50];
    int resnum;
    enum TYPE status = search_database(query_name, resultIP, question->qType, &resnum);
    free(query_name);
//    enum TYPE status = A;

    if(status == A || status == AAAA) //case 2
    {
        (*response)->header = header;
        (*response)->header->flags = 0x8081;
        short anCnt = resnum;
        convertEndian((char *)&anCnt, 2);
        (*response)->header->anCnt = anCnt;
        (*response)->header->qdCnt = 0x0100;
        (*response)->header->nsCnt = 0x0000;
        (*response)->header->arCnt = 0x0000;
        (*response)->question = question;
        DNSRR **pRR = &((*response)->firstRR); // pRR is DNSRR **
        for(int i = 0 ; i < resnum; i++)
        {
            *pRR = malloc(sizeof(DNSRR)); // *pRR is DNSRR *
            (*pRR)->rName = question->qName;
            (*pRR)->rType = question->qType;
            (*pRR)->rClass = question->qClass;
            (*pRR)->ttl = 0x00040000;
            if((*pRR)->rType == A)
                (*pRR)->rdLen = 0x0400;
            else
                (*pRR)->rdLen = 0x1000;
            (*pRR)->rData = resultIP[i];
            pRR = &((*pRR)->next);
        }
        *pRR = NULL;
        
        return A;
    }
    else if(status == BLOCK)
    {
        (*response)->header = header;
        (*response)->header->flags = 0x8381;
        (*response)->header->anCnt = 0x0000;
        (*response)->header->qdCnt = 0x0100;
        (*response)->header->nsCnt = 0x0000;
        (*response)->header->arCnt = 0x0000;
        (*response)->question = question;
        (*response)->firstRR = NULL;
        return BLOCK;
    }
    else if(status == NOT_EXIST)
    {
        pthread_mutex_lock(&mutex_lock);
        CacheResponse *response_cache;
        if(cacheRoot == NULL)
            response_cache = NULL;
        else
            response_cache = searchCache(cacheRoot, question->qName);

        int need_local_DNS = 0;
        if(response_cache != NULL)
        {
            time_t curtime;
            time(&curtime);
            time_t pass = curtime - response_cache->timestamp;
            DNSRR * rr = response_cache->response->firstRR;
            while(rr)
            {
                int ttl = rr->ttl;
                convertEndian((char *)(&ttl), 4);
                ttl -= pass;
                if(ttl < 0)
                {
                    need_local_DNS = 1;
                    cacheRoot = deleteCache(cacheRoot, response_cache);
                    break;
                }
                response_cache->timestamp = curtime;
                convertEndian((char *)(&ttl), 4);
                rr->ttl = ttl;
                rr = rr->next;
            }
        }
        else
            need_local_DNS = 1;

        if(need_local_DNS == 0 && (response_cache->response->question->qType != question->qType))
        {
            need_local_DNS = 1;
        }
        if(!need_local_DNS)
            printf("Local cache!\n");

        if(need_local_DNS)
        {
            pthread_t tid;
            char copy_buffer_for_debug[1024];
            for(int i = 0; i < 1024; i++)
            {
                copy_buffer_for_debug[i] = buf[i];
            }
            DNSHeader *copy_header_for_debug = malloc(sizeof(DNSHeader));
            for(int i = 0; i < 12; i++)
            {
                ((char *)copy_header_for_debug)[i] = ((char *)header)[i];
            }
            pthread_create( &tid, NULL, communicate_with_local_DNS, buf);
            //communicate_with_local_DNS(buf);
            for(int i = 0; i < 1e6; i++)
            {
//                if(i == 5e5 - 1)
//                    get_local_success = 1;
                if(get_local_success == 1)
                    break;
                usleep(1);
            }
            debug_show_local_DNS(copy_header_for_debug, question, serverIP, recv_num, copy_buffer_for_debug, 1, 0);
            if(get_local_success == 0)
            {
                (*response)->header = header;
                (*response)->header->flags = 0x8381;
                (*response)->header->anCnt = 0x0000;
                (*response)->header->qdCnt = 0x0100;
                (*response)->header->nsCnt = 0x0000;
                (*response)->header->arCnt = 0x0000;
                (*response)->question = question;
                (*response)->firstRR = NULL;
                return BLOCK;
            }
            else
                get_local_success = 0;
            
            *response_length = parse_response(buf, (*response));
            if(cacheRoot == NULL)
            {
                cacheRoot = getRoot((*response));
            }
            else
                cacheRoot = insertCache(cacheRoot, (*response));
            
        }
        else
        {
            (*response) = response_cache->response;
            (*response)->header->transID = header->transID;

        }
        if(need_local_DNS)
            debug_show_local_DNS((*response)->header, (*response)->question, serverIP, *response_length, buf, 1, 0);


        pthread_mutex_unlock(&mutex_lock);
        return NOT_EXIST;
    }
    return A;
}
void *communicate_with_local_DNS(void * buffer)
{
    char *buf = buffer;
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in localDNS_addr;
    localDNS_addr.sin_family = AF_INET;
    localDNS_addr.sin_port = htons(53);
    localDNS_addr.sin_addr.s_addr = inet_addr(serverIP);
    socklen_t len = sizeof(localDNS_addr);
    sendto(sockfd, buf, sizeof(char) * 1024, 0, (struct sockaddr *)&localDNS_addr, sizeof(localDNS_addr));
    recvfrom(sockfd, buf, sizeof(char) * 1024, 0, (struct sockaddr*)&localDNS_addr, &len);

    get_local_success = 1;
    close(sockfd);
    return NULL;
}
