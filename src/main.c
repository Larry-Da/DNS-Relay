#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "dns.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "AVL.h"
#include "debugging_info.h"
#include "AVL_Cache.h"

#define MAX_FILE_LINE_WIDTH 128
#define SERVICE_THREAD_NUM 5

pthread_mutex_t mutex_lock;
char *serverIP;

int main(int argc, const char * argv[]) {
    pthread_mutex_init(&mutex_lock, NULL);
    printf("Starting DNS relay......\n");
    printf("Mutex lock initialized.\n");
    //printf("%s %s\n", argv[1], argv[2]);
    cacheRoot = NULL;
    serverIP = malloc(sizeof(char) * 100);
    if(strcmp(argv[1], "-d") == 0)
    {
        set_up(1);
        strcpy(serverIP, argv[2]);
    }
    else if(strcmp(argv[1], "-dd") == 0)
    {
        set_up(2);
        strcpy(serverIP, argv[2]);
    }
    else
    {
        set_up(0);
        strcpy(serverIP, argv[1]);
    }
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    addr.sin_family =AF_INET;
    addr.sin_port = htons(53);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    char file_name[] = "./database.txt";
    FILE *fin;
    char *one_line;
    char *domain = NULL;
    one_line = (char*)malloc(MAX_FILE_LINE_WIDTH * sizeof(char));
    char *IP = NULL;
    int f = 0;
    root = (AVL*)malloc(sizeof(AVL));
    if((fin = fopen(file_name, "r"))==NULL)
    {
        printf("can not open file %s !\n", file_name);
    }
    char *type = NULL;

    while(fgets(one_line, MAX_FILE_LINE_WIDTH, fin) != NULL)
    {
        int first = 1;
        int len = strlen(one_line);
        if(one_line[len - 1] == '\n')
        {
            one_line[len - 1] = 0;
        }

        for(int i = 0; i < len; i++)
        {
            if(one_line[i] == ' ')
            {
                one_line[i] = 0;
                domain = one_line;

                if(first)
                {
                    type = &one_line[i + 1];
                    first = 0;
                }
                else
                {
                    IP = &one_line[i + 1];
                    one_line[len] = 0;
                }
            }
        }

        if(f == 0) {
            f = 1;
            root->domain = malloc(sizeof(char) * sizeof(domain));
            root->type = malloc(sizeof(char) * sizeof(type));
            root->ip = malloc(sizeof(char) * sizeof(IP));
            strcpy(root->domain, domain);
            strcpy(root->type, type);
            strcpy(root->ip, IP);
            root->next = NULL;
            root->lchild = NULL;
            root->rchild = NULL;
            root->height = 0;
        }
        else {
            root = insert(root, domain, type, IP);
        }

    }
//    printf("%s\n", root->ip);
//    Preorder(root);
//    free(root->domain);
//    free(root->type);
//    free(root->ip);
//    free(root);
    int ret =bind(sockfd,(struct sockaddr*)&addr, sizeof(addr));
    if(ret < 0) {
        printf("bind error\n");
        return -1;
    }
    printf("AVL tree build successfully.\n");
    int num_of_threads = SERVICE_THREAD_NUM;
    printf("Create %d service threads.\n", num_of_threads);

    fflush(stdout);
    free(one_line);
    pthread_t tid;

    for(int i = 0; i < SERVICE_THREAD_NUM; i++)
    {
        //query_thread(&sockfd);
        int arg = sockfd;
        pthread_create( &tid, NULL, service_thread, &arg);
    }
    pthread_join(tid, NULL);

}
