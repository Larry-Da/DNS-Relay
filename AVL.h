#ifndef AVL_h
#define AVL_h
typedef struct AVL {
    char *domain;
    char *type;
    char *ip;
    int height;
    struct AVL* next;
    struct AVL *lchild, *rchild;
}AVL;
AVL *root;
int height(AVL *node);
int max(int a, int b);
AVL *LLRotation(AVL *node);
AVL *RRRotation(AVL *node);
AVL *LRRotation(AVL *node);
AVL *RLRotation(AVL *node);
AVL *insert(AVL *root, char *domain, char *type, char *ip);
extern AVL* free_avl(AVL *root);
void Preorder(AVL *root);
#endif /* AVL_h */
