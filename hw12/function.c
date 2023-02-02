#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int str_split(char *input, char *alp_str, char *dig_str){
    int i, len, alp_len=0, dig_len=0;
    len=strlen(input);
    for(i=0;i<len;i++){
        if(isalpha(input[i])){
            alp_str[alp_len]=input[i];
            alp_len++;
        }
        else if(isdigit(input[i])){
            dig_str[dig_len]=input[i];
            dig_len++;
        }
        else return 0;
    }
    alp_str[alp_len]='\0';
    dig_str[dig_len]='\0';
    return 1;
}
void main(){
    char buf[]="abc1 23";
    char out1[20], out2[20];
    int ret;

    ret=str_split(buf, out1, out2);
    if(ret==1){
        printf("--> '%s' & '%s'\n", out1, out2);
    }
    else printf("loi\n");
}
