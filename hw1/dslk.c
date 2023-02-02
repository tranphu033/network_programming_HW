#include "dslk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

node *makeNode(ElType x) {
    node *new;
    new=(node*)malloc(sizeof(node));
    new->inf=x;
    new->next=NULL;
    return new;
}
node *chenGiua(node *cur,ElType x) {
    node *tmp;
    tmp=(node*)malloc(sizeof(node));
    tmp->inf=x;
    tmp->next=cur->next;
    cur->next=tmp;
    return tmp;
}
node *chenDau(node *first,ElType x) {
    node *tmp;
    tmp=(node*)malloc(sizeof(node));
    tmp->inf=x;
    tmp->next=first;
    first=tmp;
    return first;
}
node *chenCuoi(node *first,ElType x) {
    node *new,*tmp;
    new=(node*)malloc(sizeof(node));
    new->inf=x;
    new->next=NULL;
    tmp = first;
    while(tmp->next!=NULL)
        tmp = tmp->next;
    tmp->next = new;
    return first;
}
void hienthi(node *first) {
    node *iter;
    iter=first;
    int i=0;
    while(iter!=NULL) {
        printf("-So %d:\n",i+1);
        printf("Username: %s\n",iter->inf.uname);
        printf("Password: %s\n",iter->inf.pass);
        printf("Status: %d\n",iter->inf.status);
        iter=iter->next;
        i++;
    }
}
node *timkiem(node *first,ElType key) {
    node *tmp;
    for (tmp=first; tmp!=NULL; tmp=tmp->next) {
        if (strcmp(tmp->inf.uname,key.uname)==0) {
            return tmp;
        }
    }
    return NULL;
}
void xoaNode(node *cur) {
    node *tmp;
    tmp=cur->next;
    cur->next=cur->next->next;
    free(tmp);
}
node *xoaDau(node *first) {
    node *tmp;
    tmp=first->next;
    free(first);
    return tmp;
}

void freeList(node *first){
    node *n;
    while(first != NULL){
        n=first;
        first=first->next;
        free(n);
    }
}
