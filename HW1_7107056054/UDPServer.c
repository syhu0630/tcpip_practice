#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

int main(int argc , char *argv[])

{
    //socket的建立
    char inputBuffer[256] = {};
    char message[] = {"Welcome\n"};
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_DGRAM , 0);
    struct  timeval start;
    struct  timeval end;
    unsigned  long diff;

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo,sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(8700);
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));


    while(1){
        recvfrom(sockfd,inputBuffer,sizeof(inputBuffer),0,(struct sockaddr *)&clientInfo,&addrlen);
        gettimeofday(&start,NULL);
        diff = 1000000 * (start.tv_sec)+ start.tv_usec;
        printf("thedifference is %ld\n",diff);
        sendto(sockfd,message,sizeof(message),0,(struct sockaddr *)&clientInfo,sizeof(clientInfo));
        printf("Received packet:%s\n",inputBuffer);
    }

    return 0;
}