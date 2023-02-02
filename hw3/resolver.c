#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <sys/socket.h>

typedef struct hostent hent;
typedef struct in_addr inAd;
int main(int argc , char *argv[])
{ 
    inAd ip;
    if(argc <2)     
    {
    	printf("Hay nhap hostname hoac dia chi ip!");         
    	exit(1);
    }
    char *input=argv[1];
    
    if (!inet_pton(AF_INET,input,&ip)) {//check input la hostname hay dia chi IP
        return getIP(input);
    }else return getHostname(input);

    return 0;
}

int getIP(char *hostname){//lay dia chi IP tu ten mien
    hent *h;
    inAd **ad;
    int i;
    h= gethostbyname(hostname);
    ad=(inAd**)h->h_addr_list;
    if ( h!= NULL)
    {
        printf("Official IP: %s\n",inet_ntoa(*ad[0]));
        printf("Alias IP:\n");
    for(i = 1; ad[i] != NULL; i++) {
        printf("%s\n", inet_ntoa(*ad[i]));
    }
        return 0;
    }
    else{
        printf("No Information found!\n");
        return 1;
    }
}

int getHostname(char *ip){//lay ten mien tu dia chi IP
    inAd ad;
    hent *h;
    inet_aton(ip,&ad);
    if ((h = gethostbyaddr((const char*)&ad, sizeof(ad), AF_INET)) != NULL) {
        printf("Official name: %s\n",h->h_name);
        printf("Alias name:\n");
        for(int i =0; h->h_aliases[i]!=NULL;i++){
            printf("%s\n", h->h_aliases[i]);
        }
    }
    else {
       printf("No Information found!\n");
        return 1; 
    }
    return 0;
}
