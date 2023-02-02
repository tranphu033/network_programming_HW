#define BUFF_SIZE 1024
typedef struct {
    char fname[BUFF_SIZE];
    char path[BUFF_SIZE];
} image;
typedef image ElType;
typedef struct node {
    ElType inf;
    struct node *next;
} node;
node *makeNode(ElType x);
node *chenGiua(node *cur,ElType x);
node *chenDau(node *first,ElType x);
node *chenCuoi(node *first,ElType x);
//void hienthi(node *first);
node *timkiem(node *first,ElType key);
void xoaNode(node *cur);
node *xoaDau(node *first);
void freeList(node *first);
