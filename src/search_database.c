
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dns.h"
#include "AVL.h"
#define MAX_FILE_LINE_WIDTH 128
enum TYPE search_database(char name[], char *resultIP[], enum TYPE requestType, int *resnum)
{
    
    AVL *tmp = malloc(sizeof(AVL));
    AVL *t = malloc(sizeof(AVL));
    tmp = root;
    while(tmp) {
        if(strcmp(name, tmp->domain) == 0) {
//            strcpy(resultIP, tmp->ip);
//            if(strcmp(tmp->type, "A") == 0 && requestType == A) {
//                if(strcmp(tmp->ip, "0.0.0.0") == 0)
//                    return BLOCK;
//                else
//                    return A;
//            }
//            else if(strcmp(tmp->type, "AAAA") == 0 && requestType == AAAA) {
//                if(strcmp(tmp->ip, "0.0.0.0") == 0)
//                    return BLOCK;
//                else
//                    return AAAA;
//            }
//            else {
//                break;
//            }
            t = tmp;
            *resnum = 0;
            while(t) {
                if(strcmp(t->ip, "0.0.0.0") == 0 || strcmp(t->ip, "0:0:0:0") == 0) {
                    printf("Database block\n");
                    return BLOCK;
                }
                else {
                    if(strcmp(t->type, "A") == 0 && requestType == A) {
                        strcpy(resultIP[(*resnum)++], t->ip);
                    }
                    else if(strcmp(t->type, "AAAA") == 0 && requestType == AAAA) {
                        strcpy(resultIP[(*resnum)++], t->ip);
                    }
                    t = t->next;
                    if(*resnum == 10)
                    {
                        break;
                    }
                }
            }
            return requestType;
        }
        else if(strcmp(name, tmp->domain) > 0) {
            tmp = tmp->rchild;
        }
        else {
            tmp = tmp->lchild;
        }
    }
    return NOT_EXIST;
    
}

