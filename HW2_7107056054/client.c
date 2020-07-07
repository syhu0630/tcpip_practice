#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include<pthread.h>

void *threadFunc(void *arg){
	char receiveMessage[100] = {};
    int fd = (int)arg;
	while(1){
		recv(fd,receiveMessage,sizeof(receiveMessage),0);
        printf("%s",receiveMessage);
	}
}

int main(int argc , char *argv[]){
    //socket的建立
    int sockfd = 0, rc;
    pthread_t thread;

    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(8700);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }

    rc = pthread_create(&thread, NULL, threadFunc, (void *) sockfd);
	if(rc){
		printf("Error; return code form pthread_create() is %d\n", rc);
		exit(-1);
	}

    //Send a message to server
    while(1){
        char message[161];
        printf("Type message:\n");
        fgets(message, 161, stdin);
        send(sockfd,message,sizeof(message),0);
    }

    printf("close Socket\n");
    close(sockfd);
    return 0;
}