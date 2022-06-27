#include <stdio.h>
#include <time.h>
#include "dns.h"
#include <stdlib.h>
#include <string.h>
#include "packet_parser.h"
int debug_num = 1;
int debug_level = 0;
void convertEndian(char *data, int size);

void getFlags(short flags, int *flag_array)
{
    
    convertEndian((char *)(&flags), 2);
    unsigned short mask = 1 << 15;
    flag_array[0] = (flags & mask) >> 15;
    mask = 0b1111 << 11;
    flag_array[1] = (flags & mask) >> 11;
    mask = 1 << 10;
    flag_array[2] = (flags & mask) >> 10;
    mask = 1 << 9;
    flag_array[3] = (flags & mask) >> 9;
    mask = 1 << 8;
    flag_array[4] = (flags & mask) >> 8;
    mask = 1 << 7;
    flag_array[5] = (flags & mask) >> 7;
    mask = 0b111 << 4;
    flag_array[6] = (flags & mask) >> 4;
    mask = 0b1111;
    flag_array[7] = flags & mask;

}
void convertEndian(char *data, int size)
{
    int i = 0;
    int j = size - 1;
    while(i < j)
    {
        char temp = data[i];
        data[i] = data[j];
        data[j] = temp;
        i++;
        j--;
    }
}
void set_up(int level)
{
    debug_level = level;
}

void debug_show_local_DNS(DNSHeader *header, DNSQuestion *question, char IP[], int send_num, char *send_buffer, int isSend, int isClient)
{
    if(debug_level == 2)
    {
        if(isSend)
        {
            printf("SEND to %s (%d bytes) ", IP, send_num);
        }
        else
        {
            printf("RECV from %s (%d bytes) ", IP, send_num);

        }
        if(isClient)
        {
            printf("[ Client ]\n");
        }
        else
        {
            printf("[ Default DNS Server ]\n");
        }
        for(int i = 0; i < send_num; i++)
        {
            printf("%02x ", (unsigned char)(send_buffer[i]));
        }
        printf("\n");
        int flag_array[8];
        getFlags(header->flags, flag_array);
        char *prompt[] = {"QR", "Opcode", "AA", "TC", "RD", "RA", "Z", "Rcode"};
        for(int i = 0; i < 8; i++)
        {
            printf("%s: %d\t", prompt[i], flag_array[i]);
        }
        printf("\n");

        short qdCnt = header->qdCnt;
        convertEndian((char *)(&qdCnt), 2);
        printf("Question count: %d\t", qdCnt);

        short anCnt = header->anCnt;
        convertEndian((char *)(&anCnt), 2);
        printf("Answer count: %d\t", anCnt);

        short nsCnt = header->nsCnt;
        convertEndian((char *)(&nsCnt), 2);
        printf("Authority count: %d\t", nsCnt);

        short arCnt = header->arCnt;
        convertEndian((char *)(&arCnt), 2);
        printf("Additional count: %d\t", arCnt);

        
        printf("\n");
        fflush(stdout);


    }

}
void debug_show(DNSHeader *header, DNSQuestion *question, char IP[], int recv_num, char *rev_buffer)
{
    char* query_name = malloc(sizeof(char) * (strlen(question->qName) + 1));
    
    convert_string(question->qName, query_name);

    if(debug_level == 1 || debug_level == 2)
    {
        printf("\n-------------------------------------------------------\n");

        time_t curtime;
        time(&curtime);
        printf("%d : %s", debug_num++, ctime(&curtime));
        short ID = header->transID;
        convertEndian((char *)&ID, 2);
        printf("[ID : %02x%02x]\n", *((unsigned char*)&ID + 1), *(unsigned char*)&ID);
        printf("%s", IP);
        printf("\t%s", query_name);
        short qType = question->qType;
        convertEndian((char *)&qType, 2);
        printf("\tTYPE %d", qType);
        short qClass = question->qClass;
        convertEndian((char *)&qClass, 2);
        printf("\tCLASS %d", qClass);
        printf("\n");
    }
    if(debug_level == 2)
    {
        printf("\nRECV from %s (%d bytes) [ Client ]\n", IP, recv_num);
        for(int i = 0; i < recv_num; i++)
        {
            printf("%02x ", (unsigned char)(rev_buffer[i]));
        }
        printf("\n");
        int flag_array[8];
        getFlags(header->flags, flag_array);
        char *prompt[] = {"QR", "Opcode", "AA", "TC", "RD", "RA", "Z", "Rcode"};
        for(int i = 0; i < 8; i++)
        {
            printf("%s: %d\t", prompt[i], flag_array[i]);
        }
        printf("\n");

        short qdCnt = header->qdCnt;
        convertEndian((char *)(&qdCnt), 2);
        printf("Question count: %d\t", qdCnt);

        short anCnt = header->anCnt;
        convertEndian((char *)(&anCnt), 2);
        printf("Answer count: %d\t", anCnt);

        short nsCnt = header->nsCnt;
        convertEndian((char *)(&nsCnt), 2);
        printf("Authority count: %d\t", nsCnt);

        short arCnt = header->arCnt;
        convertEndian((char *)(&arCnt), 2);
        printf("Additional count: %d\t", arCnt);

        
        printf("\n");
        fflush(stdout);
    }
    free(query_name);
}
void debug_show_send_mes(char IP[], int length)
{
    if(debug_level == 2)
    {
        printf("SEND to %s (%d bytes)\n", IP, length);

    }

}

