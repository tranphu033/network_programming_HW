#define BUFF_SIZE 1024
#define not_found "1"
#define acc_blocked "2"
#define pass_incor "3"
#define block "4"
#define login_sucess "5"

typedef struct {
    char uname[30];
    char pass[50];
    int status;
} account;
typedef account ElType;
typedef struct node {
    ElType inf;
    struct node *next;
} node;
node *makeNode(ElType x);
node *chenGiua(node *cur,ElType x);
node *chenDau(node *first,ElType x);
node *chenCuoi(node *first,ElType x);
void hienthi(node *first);
node *timkiem(node *first,ElType key);
void xoaNode(node *cur);
node *xoaDau(node *first);
//node *readData(FILE *f,node *n);
//void writeData(FILE *f,node *n);
void freeList(node *first);
