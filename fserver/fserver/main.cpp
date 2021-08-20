#include <iostream>
#include <string>
#include <cstdio>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

int readLine(int fd, char* str);
int cfd;

int main(int argc, char *argv[])
{
    int sfd, port, clientlen;
    struct sockaddr_in serveraddr, clientaddr;
    struct hostent *hp;
    char *haddrp;
    char *aaa;

    signal(SIGCHLD, SIG_IGN); //just use it

    port = 4001;
    sfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY: my IP address
    serveraddr.sin_port = htons((unsigned short)port);
    bind(sfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

//    aaa=inet_ntoa(serveraddr.sin_addr.s_addr);
  //  printf("%s\n",aaa);
    listen(sfd, 5); //why!!!!

    while (1) {
        clientlen = sizeof(clientaddr);
        cfd = accept(sfd,(struct sockaddr*)&clientaddr,  (socklen_t*) &clientlen);
        //클라이언트의 도메인 이름과 IP 주소 결정

        hp = gethostbyaddr((char*)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        haddrp = inet_ntoa(clientaddr.sin_addr);

        printf("Server: %s is connected to (%d)\n", haddrp, clientaddr.sin_port);

        char c[MAXLINE] = "connected!";
        write(cfd, c, strlen(c)+1);
    }
    close(cfd);
    return 0;
}

int readLine(int fd, char* str)
{
    int n;
    do {
        n = read(fd, str, 1);
    } while(n > 0 && *str++ != '\0');
    return n;
}
