#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 26 //tong so ky tu bang chu cai tieng Anh

void ceasar_encrypt(char *str, int key){
    int i,num;
    for(i=0;i<strlen(str);i++){
        if(str[i]>='a' && str[i]<='z'){
            num=str[i]-97;
            str[i]= (char)((num+key) % N) + 97;
        }
        else if(str[i]>='A' && str[i]<='Z'){
            num=str[i]-65;
            str[i]= (char)((num+key) % N) + 65;
        }
    }
}

void ceasar_decrypt(char *str, int key){
    int i,num, tmp;
    for(i=0;i<strlen(str);i++){
        if(str[i]>='a' && str[i]<='z'){
            num=str[i]-97;
            tmp=num-key;
            if(tmp<0) tmp=tmp+N;
            str[i]= (char)(tmp % N) + 97;
        }
        else if(str[i]>='A' && str[i]<='Z'){
            num=str[i]-65;
            tmp=num-key;
            if(tmp<0) tmp=tmp+N;
            str[i]= (char)(tmp % N) + 65;
        }
    }
    key=2;
}

void main(){
    char ch[100];
    int choice;
    int k=3;
    printf("Choice:"); scanf("%d",&choice);
    while(getchar()!='\n');
    printf("Nhap:");
    fgets(ch,100,stdin);
    ch[strlen(ch)-1]='\0';

    if(choice==0) ceasar_encrypt(ch,k);
    else if(choice==1) ceasar_decrypt(ch,k);
    printf("Result: %s\n",ch);
    //printf("%c\n",itoa(k));
}