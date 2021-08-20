
#include <cmath>
#include <vector>
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

#include <thread>

#define DEFAULT_PROTOCOL 0
#define MAXLINE 100
#define SIZE 21
using namespace std;

typedef struct {
    int previous;
    int dist;
}place;

typedef struct {
    int x;
    int y;
}location;

int readLine(int fd, char* str);
void connecting(char*arg1, char*arg2,int* cfd);

void get_se(int*start, int *end, int* cfd);

void dijkstra_find(place* p, int d[][SIZE], int* dist, int start);
void set_location();
int distance(int x1, int y1, int x2, int y2);
void get_distinction();
int show_result(place* p, int flag, int start);

int min_find( int* visit, int* length);
void send_root(int size, int* cfd);

int d[SIZE][SIZE];
location l[SIZE];
vector<int> result;
int cfd1; //socket to Ambulance
int cfd2; //socket to Lamps

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "How to use : %s <host> <port> <host> <port>\n", argv[0]);
        exit(0);
    }
    connecting(argv[1], argv[2], &cfd1); //connect with ambulance
    connecting(argv[3], argv[4], &cfd2);

    int start, end;// 시작 node 번호
    get_se(&start, &end, &cfd1);



    int flag = end;
    place p[SIZE];
    int length[SIZE] = { 0 };

    set_location();
    // 거리 정보  location (x, y) 결정 되면 변경
    get_distinction();
    dijkstra_find(p, d, length, start);
    //printf("\n");
    //for (int i = 0; i < SIZE; i++) printf("  시작 %d -> %d번도시,  최종 거리 : %d\n", start,i, p[i].dist);
    int size = show_result(p, flag, start);

    send_root(size, &cfd1);
    send_root(size, &cfd2);

    char c[MAXLINE];




    char ax[10];
   // while(readLine(cfd1,ax)){    write(cfd2,ax,strlen(ax)+1); }

    return 0;
}

void send_root(int size, int* cfd){
    char s1[10];               // 변환한 문자열을 저장할 배열
    sprintf(s1, "%d", size);
    printf("\nsize = %d\n",size);// %f를 지정하여 실수를 문자열로 저장
    write(*cfd, s1, strlen(s1) +1);
    for(int i=0; i<size; i++){
        int num1 = l[result[i]].x;
        sprintf(s1, "%d", num1);   // %f를 지정하여 실수를 문자열로 저장
        write(*cfd, s1, strlen(s1) +1);

        num1 = l[result[i]].y;
        sprintf(s1, "%d", num1);   // %f를 지정하여 실수를 문자열로 저장
        write(*cfd, s1, strlen(s1) +1);
        printf("\nl[%d] = (%d, %d)\n",i,l[result[i]].x,l[result[i]].y);
    }
}

int readLine(int fd, char* str)
{
    int n;
    do {
        n = read(fd, str, 1);
    } while(n > 0 && *str++ != '\0');
    return n;
}
void connecting(char*arg1, char*arg2, int* cfd){
    int port, result;
    char *host;
    struct sockaddr_in serverAddr;
    struct hostent *hp;

    host = arg1;
    port = atoi(arg2);


    *cfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    // 서버의 IP 주소와 포트 번호를 채운다.
    if ((hp = gethostbyname(host)) == NULL)
        perror("gethostbyname error"); // 호스트 찾기 오류
    bzero((char *) &serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    bcopy((char*)hp->h_addr_list[0], (char*)&serverAddr.sin_addr.s_addr, hp->h_length);
    serverAddr.sin_port = htons(port);


    do { // 연결 요청
        result = connect(*cfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
        printf("connecting...\n");
        if (result == -1)
            sleep(1);
    } while (result == -1);

}
void get_se(int*start, int *end, int *cfd){
    char st[MAXLINE], de[MAXLINE];
    // get starting point and destination from socket

    readLine(*cfd, st);
    *start = atoi(st); // ch->int
    printf("starting point : %d\n", *start);

    readLine(*cfd, de);
    *end = atoi(de);
    printf("destination : %d\n", *end);
}

/* parameter : 도시정보 p, 거리정보 d, 최단거리 저장 length */
void dijkstra_find(place* p, int d[][SIZE], int* length, int start)
{
    int i;
    int visit[SIZE] = { 0 };  // 방문완료한 도시

    for (int i = 0; i < SIZE; i++)
        visit[i] = 0;
    visit[start] = 1;

    for (int i = 0; i < SIZE; i++)  // 초기 경로 설정, 직행 경로 초기화
    {
        length[i] = d[start][i];
        p[i].previous = start;
        p[i].dist = length[i];
    }

    /* 도시를 다 방문할 때까지 반복문 수행 */
    while ((i = min_find(visit, length)) != -1)
    {
        visit[i] = 1;  // 방문처리
        for (int j = 0; j < SIZE; j++)
        {
            if (j == start)
                continue;
            /* 새로운 길이 현재 저장된 값보다 더 작거나 0인 경우 갱신 */
            if (d[i][j] != 0 && (length[i] + d[i][j] < length[j] || length[j] == 0))
            {
                length[j] = length[i] + d[i][j];
                p[j].previous = i;
                p[j].dist = length[j];
            }
        }
    }
}

/* 방문하지 않은 도시 중 가장 적은 값의 노드를 찾음 */
/* parameter : 방문도시 visit, 현재 거리정보 length */
int min_find(int* visit, int* length)
{
    int min = 999999;
    int j = -1;
    for (int i = 1; i < SIZE; i++)
        if (length[i] != 0 && length[i] < min && visit[i] == 0)
        {
            min = length[i];
            j = i;
        }
    if (j == -1)  // 모두 방문했을 경우
        return -1;
    else  // 방문할 도시가 남은 경우
        return j;
}

void set_location(){
    l[0].x = 210;l[0].y = 36;
    l[1].x = 363; l[1].y = 72;
    l[2].x = 515; l[2].y = 109;
    l[3].x = 699; l[3].y = 154;
    l[4].x = 185; l[4].y = 95;
    l[5].x = 341; l[5].y = 132;
    l[6].x = 167; l[6].y = 155;
    l[7].x = 320; l[7].y = 200;
    l[8].x = 400; l[8].y = 216;
    l[9].x = 466; l[9].y = 234;
    l[10].x = 688; l[10].y = 270;
    l[11].x = 95; l[11].y = 222;
    l[12].x = 130; l[12].y = 234;
    l[13].x = 286; l[13].y = 277;
    l[14].x = 368; l[14].y = 300;
    l[15].x = 428; l[15].y = 318;
    l[16].x = 29; l[16].y = 308;
    l[17].x = 55; l[17].y = 315;
    l[18].x = 88; l[18].y = 323;
    l[19].x = 251; l[19].y = 362;
    l[20].x = 391; l[20].y = 401;
}

int distance(int x1, int y1, int x2, int y2){
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}
void get_distinction(){
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            d[i][j] = 0;
        }
    }
    d[0][1] = d[1][0] = distance(l[0].x, l[0].y, l[1].x, l[1].y); d[0][4] = d[4][0] = distance(l[0].x, l[0].y, l[4].x, l[4].y);
    d[1][2] = d[2][1] = distance(l[1].x, l[1].y, l[2].x, l[2].y); d[1][5] = d[5][1] = distance(l[1].x, l[1].y, l[5].x, l[5].y);
    d[2][3] = d[3][2] = distance(l[2].x, l[2].y, l[3].x, l[3].y); d[2][9] = d[2][9] = distance(l[2].x, l[2].y, l[9].x, l[9].y);
    d[3][10] = d[10][3] =distance(l[3].x, l[3].y, l[10].x, l[10].y);

    d[4][5] = d[5][4] = distance(l[4].x, l[4].y, l[5].x, l[5].y); d[4][6] = d[6][4] = distance(l[4].x, l[4].y, l[6].x, l[6].y);
    d[5][7] = d[7][5] = distance(l[5].x, l[5].y, l[7].x, l[7].y); d[5][7] = d[7][5] = distance(l[5].x, l[5].y, l[7].x, l[7].y);

    d[6][7] = d[7][6] = distance(l[6].x, l[6].y, l[7].x, l[7].y); d[6][12] = d[12][6] = distance(l[6].x, l[6].y, l[12].x, l[12].y);
    d[7][8] = d[8][7] = distance(l[7].x, l[7].y, l[8].x, l[8].y); d[7][13] = d[8][13] = distance(l[7].x, l[7].y, l[13].x, l[13].y);
    d[8][9] = d[9][8] = distance(l[8].x, l[8].y, l[9].x, l[9].y); d[8][14] = d[14][8] = distance(l[8].x, l[8].y, l[14].x, l[14].y);
    d[9][10] = d[10][9] = distance(l[9].x, l[9].y, l[10].x, l[10].y); d[9][15] = d[15][9] = distance(l[9].x, l[9].y, l[15].x, l[15].y);

    d[11][12] = d[12][11] = distance(l[11].x, l[11].y, l[12].x, l[12].y);
    d[12][13] = d[13][12] = distance(l[12].x, l[12].y, l[13].x, l[13].y); d[12][18] = d[18][12] = distance(l[12].x, l[12].y, l[18].x, l[18].y);
    d[13][14] = d[14][13] = distance(l[13].x, l[13].y, l[14].x, l[14].y); d[13][19] = d[19][13] = distance(l[13].x, l[13].y, l[19].x, l[19].y);
    d[14][15] = d[14][15] = distance(l[14].x, l[14].y, l[15].x, l[15].y);
    d[15][20] = d[20][15] = distance(l[15].x, l[15].y, l[20].x, l[20].y);

    d[16][17] = d[17][16] = distance(l[16].x, l[16].y, l[17].x, l[17].y);
    d[17][18] = d[18][17] = distance(l[17].x, l[17].y, l[18].x, l[18].y);
    d[18][19] = d[19][18] = distance(l[18].x, l[18].y, l[19].x, l[19].y);
    d[19][20] = d[20][19] = distance(l[19].x, l[19].y, l[20].x, l[20].y);

}

int show_result(place* p, int flag, int start){
    while (flag != start)
    {
        result.insert(result.begin(), flag);
        flag = p[flag].previous;
    }
    result.insert(result.begin(), flag);

    for (int i = 0; i < result.size(); i++)
        cout << result[i] << "\n";
    return result.size();
}
