#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include "dns.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "packet_parser.h"
#include "garbage_collection.h"
#include "debugging_info.h"

void copy_consecutive_bytes(char *src, char* des, int num, int *cpy_cny);
void copy_string(char *des, char *src, int *cpy_cnt);
void generate_response_packet(int *cpy_cnt, Response *response, DNSHeader* header, DNSQuestion* question, char response_buff[], int status);

void* service_thread(void *arg)
{
    int sockfd = *(int *)arg;
    while(1)
    {
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli); //这里你的cli和len都是不需要填充的，recvfrom 会在接收到请求之后，将这两个字段填充
        char buf[1024];
        char *deep_copy_header = malloc(sizeof(char) * 1024);
        
        recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&cli, &len);
        char deep_copy_buf[1024];
        for(int i = 0; i < 1024; i++)
        {
            deep_copy_buf[i] = buf[i];
            deep_copy_header[i] = buf[i];
        }

        DNSHeader* header = (DNSHeader*)deep_copy_header;
        DNSQuestion* question = (DNSQuestion *)malloc(sizeof(DNSQuestion));
        
        int recv_num = parse_query(buf, header, question);
        
        char response_buff[2048];
        Response* response = (Response*)malloc(sizeof(Response));
        
        int cpy_cnt = 0;
        debug_show(header, question, inet_ntoa(cli.sin_addr), recv_num, deep_copy_header);

        int response_length = -1;
        if(header->flags == 0x0001)
        {
            enum TYPE status = normal_handling(&response, header, question, deep_copy_header, &response_length, recv_num);
            if(status == A || status == AAAA)
            {
                generate_response_packet(&cpy_cnt, response, header, question, response_buff, A);
                sendto(sockfd, &response_buff, sizeof(char) * cpy_cnt, 0, (struct sockaddr *)&cli, sizeof(cli));
            }
            else if(status == NOT_EXIST)
            {
                generate_response_packet(&cpy_cnt, response, header, question, response_buff, NOT_EXIST);
                sendto(sockfd, &response_buff, sizeof(char) * cpy_cnt, 0, (struct sockaddr *)&cli, sizeof(cli));
                //sendto(sockfd, deep_copy_buf, sizeof(char) * 1024, 0, (struct sockaddr *)&cli, sizeof(cli));
            }
            else if(status == BLOCK)
            {
                generate_response_packet(&cpy_cnt, response, header, question, response_buff, BLOCK);
                sendto(sockfd, &response_buff, sizeof(char) * cpy_cnt, 0, (struct sockaddr *)&cli, sizeof(cli));
            }

        }
        if(response_length != -1)
            debug_show_local_DNS((response)->header, (response)->question, inet_ntoa(cli.sin_addr), response_length, response_buff, 1, 1);
        else
            debug_show_local_DNS((response)->header, (response)->question, inet_ntoa(cli.sin_addr), cpy_cnt, response_buff, 1, 1);
        fflush(stdout);
        //free_question_list(question);
        //free_response(response);
        
    }
        return NULL;
    
}

void copy_consecutive_bytes(char *des, char* src, int num, int *cpy_cnt)
{
    for(int i = 0; i < num; i++)
    {
        des[*cpy_cnt] = src[i];
        (*cpy_cnt)++;
    }
}
void copy_name(char *des, char *src, int *cpy_cnt)
{
    char *p_str = src;
    if((unsigned char)(*p_str) == 0xC0)
    {
        // des[*cpy_cnt] = 0xC0;
        // (*cpy_cnt)++;
        // des[*cpy_cnt] = 0x0C;
        des[*cpy_cnt] = *p_str;
        p_str++;
        (*cpy_cnt)++;
        des[*cpy_cnt] = *p_str;
        (*cpy_cnt)++;
    }
    else
    {
        while(*p_str)
        {
            des[*cpy_cnt] = *p_str;
            p_str++;
            (*cpy_cnt)++;
        }
        des[*cpy_cnt] = 0;
        (*cpy_cnt)++;
    }
}

void copy_string(char *des, char *src, int *cpy_cnt)
{
    char *p_str = src;
    while(*p_str)
    {
        des[*cpy_cnt] = *p_str;
        p_str++;
        (*cpy_cnt)++;
    }
    des[*cpy_cnt] = 0;
    (*cpy_cnt)++;
}
void generate_response_packet(int *cpy_cnt, Response *response, DNSHeader* header, DNSQuestion* question, char response_buff[], int status)
{
    //header
    copy_consecutive_bytes(response_buff, (char *)(response->header), 12, cpy_cnt);
    
    //question qName
    copy_name(response_buff, response->question->qName, cpy_cnt);
    
    //question qType qClass
    copy_consecutive_bytes(response_buff, (char *)(&(response->question->qType)), 4, cpy_cnt);
    
    DNSRR *RR_pointer = response->firstRR;
    while(RR_pointer)
    {
        //DNSRR rName
        copy_name(response_buff, RR_pointer->rName, cpy_cnt);
        
        /*
         short rType;
         short rClass;
         int ttl;
         short rdLen;
         */
        copy_consecutive_bytes(response_buff, (char *)(&(RR_pointer->rType)), 10, cpy_cnt);
        
        // rData
        if(RR_pointer->rType == A)
        {
            char * ia;
            struct in_addr* data_addr = malloc(sizeof(struct in_addr));
            if(!inet_aton(RR_pointer->rData, data_addr))
            {
                ia = RR_pointer->rData;
            }
            else
            {
                int ip_address = data_addr->s_addr;
                ia = (char *)(&ip_address);
            }

            for(int i = 0; i < 4; i++)
            {
                response_buff[*cpy_cnt] = ia[i];
                (*cpy_cnt)++;
            }
            free(data_addr);
        }
        else if(RR_pointer->rType == AAAA)
        {
            if(status == NOT_EXIST)
            {
                short rdLen = RR_pointer->rdLen;
                convertEndian((char *)&rdLen, 2);
                for(int i = 0; i < rdLen; i++)
                {
                    response_buff[*cpy_cnt] = RR_pointer->rData[i];
                    (*cpy_cnt)++;
                }
            }
            else
            {
                char *v6 = RR_pointer->rData;
                unsigned int result[4];
                ipv6convert(v6, result);

                for(int i = 0; i < 4; i++)
                {
                    char * ia = (char *)(&result[i]);

                    for(int j = 0; j < 4; j++)
                    {
                        response_buff[*cpy_cnt] = ia[i + 4 - j - 1];
                        (*cpy_cnt)++;
                    }
                    
                }
            }

        }
        else
        {
            short rdLen = RR_pointer->rdLen;
            convertEndian((char *)&rdLen, 2);
            for(int i = 0; i < rdLen; i++)
            {
                response_buff[*cpy_cnt] = RR_pointer->rData[i];
                (*cpy_cnt)++;
            }
        }
        RR_pointer = RR_pointer->next;
    }


}
