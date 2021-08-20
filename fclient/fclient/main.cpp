#include <iostream>
#include <cstdio>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

int readLine(int fd, char* str);

int main(int argc, char *argv[])
{
    int cfd, port, result;
    char *host, inmsg[MAXLINE], outmsg[MAXLINE];
    struct sockaddr_in serverAddr;
    struct hostent *hp;

    if (argc != 3) {
        fprintf(stderr, "How to use : %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);
    cfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    // 서버의 IP 주소와 포트 번호를 채운다.
    if ((hp = gethostbyname(host)) == NULL)
        perror("gethostbyname error"); // 호스트 찾기 오류
    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    bcopy((char*)hp->h_addr_list[0], (char*)&serverAddr.sin_addr.s_addr, hp->h_length);
    serverAddr.sin_port = htons(port);


    do { // 연결 요청
        result = connect(cfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
        printf("connecting...\n");
        if (result == -1)
            sleep(1);
    } while (result == -1);


    char start[MAXLINE], dest[MAXLINE];
    // get starting point and destination from socket
    readLine(cfd, start);
    printf("starting point : %s\n", start);
    readLine(cfd, dest);
    printf("destination : %s\n", dest);



    char s1[10];               // 변환한 문자열을 저장할 배열
    int num1 = 3;   // 38.972340은 실수

    sprintf(s1, "%d", num1);   // %f를 지정하여 실수를 문자열로 저장
    write(cfd, s1, strlen(s1) +1);


    num1 = 400;
    sprintf(s1, "%d", num1);   // %f를 지정하여 실수를 문자열로 저장
    write(cfd, s1, strlen(s1) +1);

    num1 = 500;
    sprintf(s1, "%d", num1);   // %f를 지정하여 실수를 문자열로 저장
    write(cfd, s1, strlen(s1) +1);

    printf("send %s!\n",s1);




    while (readLine(cfd, outmsg))
        printf("%s\n", outmsg);
    close(cfd);
    printf("en2d\n");
    exit(0);


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
