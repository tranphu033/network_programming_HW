#define un_incorr "!!un_incorr"
#define acc_blocked "!!acc_blocked"
#define pass_incorr "!!pass_incorr"
#define login_succ "!!login_succ"
#define stop_tran "!!stop_tran"

typedef struct account{
    char uname[50];
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
void freeList(node *first);
