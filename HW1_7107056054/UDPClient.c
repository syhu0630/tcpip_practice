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
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_DGRAM , 0);
    struct  timeval start;
    struct  timeval end;
    unsigned  long diff;

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    int addrlen = sizeof(info);
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = INADDR_ANY;
    info.sin_port = htons(8700);


    //Send a message to server
    while(1){
        char message[161];
        printf("Packet content:");
        fgets(message, 161, stdin);
        char receiveMessage[100] = {};
        sendto(sockfd,message,sizeof(message),0,(struct sockaddr *)&info,sizeof(info));
        gettimeofday(&start,NULL);
        diff = 1000000 * (start.tv_sec)+ start.tv_usec;
        printf("thedifference is %ld\n",diff);
        recvfrom(sockfd,receiveMessage,sizeof(receiveMessage),0,(struct sockaddr *)&info,&addrlen);
        printf("%s",receiveMessage);
    }

    printf("close Socket\n");
    close(sockfd);
    return 0;
}