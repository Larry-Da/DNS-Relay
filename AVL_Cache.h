#ifndef AVL_Cache_h
#define AVL_Cache_h

#include "dns.h"
#include "time.h"
typedef struct CacheResponse {
    Response * response;
    time_t timestamp;
    int height;
    struct CacheResponse *lchild, *rchild;
}CacheResponse;
CacheResponse *getRoot(Response *response);
CacheResponse* insertCache(CacheResponse *root, Response *data);
CacheResponse* searchCache(CacheResponse *root, char* qName);
CacheResponse* deleteCache(CacheResponse *root, CacheResponse *node);
CacheResponse *cacheRoot;

#endif /* AVL_Cache_h */