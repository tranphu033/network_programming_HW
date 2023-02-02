#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dslk.h"

node *readData(FILE *f,node *n){
	ElType e;
	while(fscanf(f,"%s %s %d",e.uname,e.pass,&e.status) !=EOF){
	  if (n==NULL) n=chenDau(n,e);
	  else n=chenCuoi(n,e);
	}
	return n;
}
void writeData(FILE *f,node *n){
    node *tmp;

    for (tmp=n; tmp!=NULL; tmp=tmp->next) {
        fprintf(f,"%s %s %d\n",tmp->inf.uname,tmp->inf.pass,tmp->inf.status);
    } 
}

int main(){
  int ch;
  ElType el;
  FILE *f;
  node *head=NULL,*n;
	f=fopen("account.txt","r");
	head=readData(f,head);
	fclose(f);

	//hienthi(head);
	do{
	  printf("USER MANAGEMENT PROGRAM\n1. Register\n2. Sign in\n3. Search\n4. Sign out\nYour choice:");
	  scanf("%d",&ch);
	  while(getchar()!='\n');
	  switch(ch){
	  case 1:{
	    do{
	      printf("Nhap username:");gets(el.uname);
	      if((n=timkiem(head,el)) != NULL)
		printf("Ten tai khoan da co nguoi dung\n");
	    }while(n!=NULL);
	    printf("Nhap password:");gets(el.pass);
	    el.status=1;
	    if (head==NULL) head=chenDau(head,el);
	    else head=chenCuoi(head,el);
    }
	    break;
	  case 2:{
	    int k=0;
	    printf("Nhap username:");gets(el.uname);
	    if((n=timkiem(head,el)) != NULL){
	      if(n->inf.status ==0){
		printf("Tai khoan bi khoa!\n");
		break;
	      }
	      do{
		printf("Nhap password:");gets(el.pass);
		if(strcmp(n->inf.pass, el.pass)==0) {
		  printf("Dang nhap thanh cong!\n");
		  n->inf.signIn=1;
		  break;		  
    }
		else {
		  k++;
		  if(k<=3) printf("Ban nhap sai mat khau!\n");
		  else {
		    printf("Nhap sai mat khau qua 3 lan. Tai khoan bi khoa!\n ");
		    n->inf.status=0;
		  }
		}
	      }while(k<=3);
	    }
	    else printf("Ten tai khoan khong ton tai!\n");
    }
	    break;
	  case 3:{
	      printf("Nhap username:");gets(el.uname);
	      if((n=timkiem(head,el)) != NULL){
	          printf("Username: %s\n",n->inf.uname);
	          printf("Password: %s\n",n->inf.pass);
	          printf("Status: ");
	          if(n->inf.status==1) printf("active\n");
	          else printf("blocked\n");
	      }
	      else printf("Khong tim thay tai khoan!\n");
}
	    break;
	  case 4:{
	      printf("Nhap username:");gets(el.uname);
	      if((n=timkiem(head,el)) == NULL){
	      	printf("Khong tim thay tai khoan.\n");
	        }
	       else{
	       	if(n->inf.signIn==1){
	       		printf("Dang xuat thanh cong.\n");
	       		n->inf.signIn=0;
	       	}
	       	else{
	       		printf("Tai khoan chua dang nhap.\n");
	       	}
	       }
	  }
	    break;
	  }		
	}while(ch>=1 && ch<=4);
	f=fopen("account.txt","w");
	writeData(f,head);
	freeList(head);
	fclose(f);
	return 0;
}
    
