#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AVL_Cache.h"
#include <time.h>
int getHeight(CacheResponse *node) {
    return node == NULL ? -1 : node->height;
}
int getMax(int a, int b) {
    return a >= b ? a : b;
}
CacheResponse *getRoot(Response *response) {
    CacheResponse *root = malloc(sizeof(CacheResponse));
    root->response = response;
    root->lchild = root->rchild = NULL;
    time_t now;
    root->timestamp = time(&now);
    root->height = getMax(getHeight(root->lchild),getHeight(root->rchild)) + 1;
    return root;
}
CacheResponse *LL_Rotation(CacheResponse *node) {
    CacheResponse *tmp = node->lchild;
    node->lchild = tmp->rchild;
    tmp->rchild = node;
    node->height = getMax(getHeight(node->lchild), getHeight(node->rchild)) + 1;
    tmp->height = getMax(getHeight(tmp->lchild), getHeight(tmp->rchild)) + 1;
    return tmp;
}
CacheResponse *RR_Rotation(CacheResponse *node) {
    CacheResponse *tmp = node->rchild;
    node->rchild = tmp->lchild;
    tmp->lchild = node;
    node->height = getMax(getHeight(node->lchild), getHeight(node->rchild)) + 1;
    tmp->height = getMax(getHeight(tmp->lchild), getHeight(tmp->rchild)) + 1;
    return tmp;
}
CacheResponse *LR_Rotation(CacheResponse *node) {
    node->lchild = RR_Rotation(node->lchild);
    return LL_Rotation(node);
}
CacheResponse *RL_Rotation(CacheResponse *node) {
    node->rchild = LL_Rotation(node->rchild);
    return RR_Rotation(node);
}
CacheResponse *maxMum(CacheResponse *node) {
    if(node == NULL) {
        return NULL;
    }
    while(node->rchild != NULL) {
        node = node->rchild;
    }
    return node;
}
CacheResponse *miniMum(CacheResponse *node) {
    if(node == NULL) {
        return NULL;
    }
    while(node->lchild != NULL) {
        node = node->lchild;
    }
    return node;
}
CacheResponse* insertCache(CacheResponse *root, Response *data) {
    if(root == NULL) {
        root = (CacheResponse *)malloc(sizeof(CacheResponse));
        if(root) {
            root->response = data;
            root->lchild = root->rchild = NULL;
            time_t now;
            root->timestamp = time(&now);
            root->height = getMax(getHeight(root->lchild),getHeight(root->rchild)) + 1;
            return root;
        }
        else
            return NULL;
    }
    else if(strcmp(data->question->qName, root->response->question->qName) < 0) {
        root->lchild = insertCache(root->lchild, data);
        if(getHeight(root->lchild) - getHeight(root->rchild) == 2) {
            if(strcmp(data->question->qName, root->lchild->response->question->qName) < 0)
                root = LL_Rotation(root);
            else
                root = LR_Rotation(root);
        }
    }
    else if(strcmp(data->question->qName, root->response->question->qName) > 0) {
        root->rchild = insertCache(root->rchild, data);
        if(getHeight(root->rchild) - getHeight(root->lchild) == 2) {
            if(strcmp(data->question->qName, root->rchild->response->question->qName) > 0)
                root = RR_Rotation(root);
            else
                root = RL_Rotation(root);
        }
    }
    root->height = getMax(getHeight(root->lchild),getHeight(root->rchild)) + 1;
    return root;
}
CacheResponse* searchCache(CacheResponse *root, char* qName) {
    CacheResponse *tmp = root;
    while(tmp) {
        if(strcmp(tmp->response->question->qName, qName) == 0) {
            return tmp;
        }
        else if(strcmp(tmp->response->question->qName, qName) < 0) {
            tmp = tmp->rchild;
        }
        else {
            tmp = tmp->lchild;
        }
    }
    return NULL;
}
CacheResponse* deleteCache(CacheResponse *root, CacheResponse *node) {
    if(node == NULL) {
        exit(0);
    }
    if(strcmp(node->response->question->qName, root->response->question->qName) > 0) {
        root->rchild = deleteCache(root->rchild, node);
        if(getHeight(root->lchild) - getHeight(root->rchild) == 2) {
            CacheResponse *tmp = root->lchild;
            if(getHeight(tmp->lchild) >= getHeight(tmp->rchild)) {
                root = LL_Rotation(root);
            }
            else {
                root = LR_Rotation(root);
            }
        }
    }
    else if(strcmp(node->response->question->qName, root->response->question->qName) < 0) {
        root->lchild = deleteCache(root->lchild, node);
        if(getHeight(root->rchild) - getHeight(root->lchild) == 2) {
            CacheResponse *tmp = root->rchild;
            if(getHeight(tmp->rchild) >= getHeight(tmp->lchild)) {
                root = RR_Rotation(root);
            }
            else {
                root = RL_Rotation(root);
            }
        }
    }
    else if(strcmp(node->response->question->qName, root->response->question->qName) == 0) {
        if(root->lchild != NULL && root->rchild != NULL) {
            if(getHeight(root->lchild) > getHeight(root->rchild)) {
                CacheResponse *max = maxMum(root->lchild);
                root->response = max->response;
                root->timestamp = max->timestamp;
                root->lchild = deleteCache(root->lchild, node);

            }
            else {
                CacheResponse *min = miniMum(root->rchild);
                root->response = min->response;
                root->timestamp = min->timestamp;
                root->rchild = deleteCache(root->rchild, node);
            }
        }
        else {
            CacheResponse *p = root;
            root = root->lchild ? root->lchild : root->rchild;
            if(p != NULL) {
                p->lchild = NULL;
                p->rchild = NULL;
                p = NULL;
            }
        }
    }
    return root;
}
