#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dns.h"
#include "AVL.h"
int same = 0;
int height(AVL *node) {
    return node == NULL ? -1 : node->height;
}
int max(int a, int b) {
    return a >= b ? a : b;
}
AVL *LLRotation(AVL *node) {
    AVL *tmp = node->lchild;
    node->lchild = tmp->rchild;
    tmp->rchild = node;
    node->height = max(height(node->lchild), height(node->rchild)) + 1;
    tmp->height = max(height(tmp->lchild), height(tmp->rchild)) + 1;
    return tmp;
}
AVL *RRRotation(AVL *node) {
    AVL *tmp = node->rchild;
    node->rchild = tmp->lchild;
    tmp->lchild = node;
    node->height = max(height(node->lchild), height(node->rchild)) + 1;
    tmp->height = max(height(tmp->lchild), height(tmp->rchild)) + 1;
    return tmp;
}
AVL *LRRotation(AVL *node) {
    node->lchild = RRRotation(node->lchild);
    return LLRotation(node);
}

AVL *RLRotation(AVL *node) {
    node->rchild = LLRotation(node->rchild);
    return RRRotation(node);
}
AVL *insert(AVL *root, char *domain, char *type, char *ip) {
//    printf("%s\n", ip);
    if(root == NULL) {
        root = (AVL *)malloc(sizeof(AVL));
        if(root) {
            root->domain = malloc(sizeof(char) * sizeof(domain));
            root->type = malloc(sizeof(char) * sizeof(type));
            root->ip = malloc(sizeof(char) * sizeof(ip));
            strcpy(root->domain, domain);
            strcpy(root->type, type);
            strcpy(root->ip, ip);
            root->lchild = root->next = root->rchild = NULL;
            root->height = max(height(root->lchild),height(root->rchild)) + 1;
            return root;
        }
        else
            return NULL;
    }
    else if(strcmp(domain, root->domain) < 0) {
        root->lchild = insert(root->lchild, domain, type, ip);
        if(height(root->lchild) - height(root->rchild) == 2) {
            if(strcmp(domain, root->lchild->domain) < 0)
                root = LLRotation(root);
            else
                root = LRRotation(root);
        }
    }
    else if(strcmp(domain, root->domain) > 0) {
        root->rchild = insert(root->rchild, domain, type, ip);
        if(height(root->rchild) - height(root->lchild) == 2) {
            if(strcmp(domain, root->rchild->domain) > 0)
                root = RRRotation(root);
            else
                root = RLRotation(root);
        }
    }
    else {
        same = 1;
        AVL *tmpNode = malloc(sizeof(AVL));
        tmpNode->domain = malloc(sizeof(char) * sizeof(domain));
        tmpNode->type = malloc(sizeof(char) * sizeof(type));
        tmpNode->ip = malloc(sizeof(char) * sizeof(ip));
        strcpy(tmpNode->domain, domain);
        strcpy(tmpNode->type, type);
        strcpy(tmpNode->ip, ip);
        tmpNode->lchild = tmpNode->rchild = NULL;
        tmpNode->height = 0;
        tmpNode->next = root->next;
        root->next = tmpNode;
    }
    if(same == 0) {
        root->height = max(height(root->lchild),height(root->rchild)) + 1;
    }
    return root;
}
AVL* free_avl(AVL *root) {
    if(root) {
        free_avl(root->lchild);
        free_avl(root->rchild);
        free(root);
    }
    return NULL;
}
void Preorder(AVL *root) {
    if(!root)
        return ;
    AVL *tmp = root;
    while(tmp) {
        printf("%s %s %s\n", tmp->domain, tmp->type, tmp->ip);
        tmp = tmp->next;
    }
    free(tmp);
    Preorder(root->lchild);
    Preorder(root->rchild);
}
