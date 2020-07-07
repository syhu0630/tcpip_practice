#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include<pthread.h>

int forClientSockfd[5] = {0};

void *threadFunc(void *arg){
    char inputBuffer[256] = {};
    //char message[256] = {"client"};
    char str[10] = {};
    char point[10]={": "};
    int fd = (int)arg;
    for(int j=0;j<5;j++){
        if(forClientSockfd[j] == fd){
            sprintf(str, "%d", j+1);
        }
    }

    while(1){
        char message[256] = {"client"};
        recv(fd,inputBuffer,sizeof(inputBuffer),0);
        strcat (message,str); 
        strcat (message,point); 
        strcat (message,inputBuffer); 
        printf("%s\n",inputBuffer);
        for(int j=0;j<5;j++){
            //printf("%d  %d",forClientSockfd[j],fd);
		    if(forClientSockfd[j] != fd){
                send(forClientSockfd[j],message,sizeof(message),0);
		    }
        }
    }

}

int main(int argc , char *argv[]){
    //socket的建立
    pthread_t thread_a[5];
    int sockfd = 0, i=0, rc;

    int result;
    fd_set readfds, testfds;

    sockfd = socket(AF_INET , SOCK_STREAM , 0);

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
    listen(sockfd,5);

    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);
        
    while(1){
        int fd;
        testfds = readfds;
        result = select(FD_SETSIZE, &testfds, (fd_set *)0, (fd_set *)0, (struct timeval *) 0);
        if(result < 1) {
            perror("server error");
            return (-1);
        }
        for(fd = 0; fd < FD_SETSIZE; fd++) {        
            // some data
            if(FD_ISSET(fd, &testfds)) {
                //new connection
                if(fd == sockfd) {
                    forClientSockfd[i] = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
                    FD_SET(forClientSockfd[i], &readfds);
                    printf("client%d connect\n", i+1);
		            rc = pthread_create(&thread_a[i], NULL, threadFunc, (void *) forClientSockfd[i]);
		            if(rc){
			            printf("Error; return code from pthread_create() is %d\n", rc);
			            exit(-1);
		            }
                    i++;
                }
            }
        }

	}

    return 0;
}