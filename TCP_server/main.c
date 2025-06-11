#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


#define PORT 8080
#define SA struct sockaddr

int main(){
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    int sockfd;
    struct sockaddr_in serveraddr, cli;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1){
        printf("Socket not created!!\n");
        exit(EXIT_FAILURE);
    }
    else{
        printf("Lol\n");
    }
    bzero(&serveraddr,sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = PORT;


    //binding to the created socket.
    if(bind(sockfd,(SA*)&serveraddr,sizeof(serveraddr))!=0){
        printf("CONNECTION FAILED!!!. SOS\n");
        exit(EXIT_FAILURE);
    }
    else{
        printf("YOU just binded! CONGRATULATIONS!\n");
    }

    if(listen(sockfd,5)!=0){
        printf("Verification failed\n");
    }
    else{
        printf("Verification successful!!\n");
    }

    int len = sizeof(cli);

    int connfd = accept(sockfd,(SA*)&cli, &len);

    if(connfd<0){
        printf("Connection failed\n");
        exit(0);
    }
    else{
        printf("Connection successss");
    }

    closesocket(sockfd);
    WSACleanup();
    return 0;


}
