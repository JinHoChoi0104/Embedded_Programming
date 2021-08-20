#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QPixmap>

#include <iostream>
#include <string>
#include <cstdio>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <thread>
using namespace std;

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEFAULT_PROTOCOL 0
#define MAXLINE 100
#define SIZE 21

typedef struct {
    int previous;
    int dist;
}place;

typedef struct {
    int x;
    int y;
}location;

int readLine(int fd, char* str);
void get_root();

void get_se(int*start, int *end);

void set_location();
int distance(int x1, int y1, int x2, int y2);
int show_result(place* p, int flag, int start);

int min_find( int* visit, int* length);

int d[SIZE][SIZE];
location l[SIZE];
vector<int> result;

int cfd; //socket
static int num; //number of node

static QString aa1;
static QString aa2;



static double cx=710;
static double cy=180;

class Place {
public: //데이터
    double x; //Arrival Time 도착시간
    double y; //Burst Time 처리시간

public:
    Place (double _x, double _y) { //생성자
        x = _x;
        y = _y;
    }
    Place() {
        x = 0;
        y = 0;
    }

    void Set_Place(double _x, double _y) { //생성자
        x = _x;
        y = _y;
    }
};

static Place P_L[100];

QLabel *label_array[7];

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap bg("/home/pi/Downloads/map.png");
    bg =bg.scaled(800,450);
    QPalette p(palette());
    p.setBrush(QPalette::Background, bg);
    setAutoFillBackground(true);
    setPalette(p);

    label_array[0]=ui->label;
    label_array[1]=ui->label_2;
    label_array[2]=ui->label_3;
    label_array[3]=ui->label_4;
    label_array[4]=ui->label_5;
    label_array[5]=ui->label_6;
    label_array[6]=ui->label_7;


    QPixmap pix;
    QPixmap pix2("/home/pi/Downloads/car.png");
    pix = pix2.scaled(60, 60, Qt::KeepAspectRatio);
   // ui->label_pic->setPixmap(pix);

    timer = new QTimer(this);
    timer -> start(30);
    connect(timer,SIGNAL(timeout()),this,SLOT(move_car()));
    connect(timer,SIGNAL(timeout()),this,SLOT(lamp_ch()));
}

MainWindow::~MainWindow()
{
    delete ui;
}




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    set_location();

    int sfd, port, clientlen;
    struct sockaddr_in serveraddr, clientaddr;

    signal(SIGCHLD, SIG_IGN); //just use it

    port = 4005;
    sfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY: my IP address
    serveraddr.sin_port = htons((unsigned short)port);
    bind(sfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    listen(sfd, 5); //why!!!!

    while (1) {
        clientlen = sizeof(clientaddr);
        cfd = accept(sfd,(struct sockaddr*)&clientaddr,  (socklen_t*) &clientlen);
        //클라이언트의 도메인 이름과 IP 주소 결정

        gethostbyaddr((char*)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        inet_ntoa(clientaddr.sin_addr);
        thread t1(get_root);

        a.exec();
    }
    close(cfd);

    return a.exec();
}
void get_root()
{
    char tmp[10];
    readLine(cfd,tmp);
    string str(tmp); //ch -> str
    num = atoi(str.c_str()); //str->int

    for(int i=0; i<num ;i++){
        readLine(cfd,tmp);
        str = string(tmp);
        int _x = atoi(str.c_str()); //str->int
        P_L[i].x= _x;

        readLine(cfd,tmp);
        str = string(tmp);
        int _y = atoi(str.c_str()); //str->int
        P_L[i].y= _y;

    }
    cx = P_L[0].x;
    cy = P_L[0].y;


    while(readLine(cfd,tmp)){

    }
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter myline(this);
    QPen linePen(Qt::blue);
    linePen.setWidth(3);
    myline.setPen(linePen);

    for(int i=0; i< num -1;i++){
        myline.drawLine(P_L[i].x,P_L[i].y,P_L[i+1].x,P_L[i+1].y);
        label_array[i]->move(P_L[i+1].x,P_L[i+1].y-40);
    }
}
void MainWindow::lamp_ch()
{
    QPixmap pix("/home/pi/Downloads/red.png");
    QPixmap red = pix.scaled(60, 60, Qt::KeepAspectRatio);
    QPixmap pix2("/home/pi/Downloads/green.png");
    QPixmap gr = pix2.scaled(60, 60, Qt::KeepAspectRatio);
    QPixmap pix3("/home/pi/Downloads/yellow.png");
    QPixmap ye = pix3.scaled(60, 60, Qt::KeepAspectRatio);
    static int cnt[7] = {0};






    for(int i=0; i<num-1; i++){

        if( abs(cy-12.5 - P_L[i+1].y) < 20 || abs(cx-30 - P_L[i+1].x) < 20 )
            cnt[i]=0;

        cnt[i] += 1;
        if(cnt[i]<=70)
            label_array[i]->setPixmap(gr);
        else if(cnt[i]<=90)
            label_array[i]->setPixmap(ye);
        else if(cnt[i]<=180)
            label_array[i]->setPixmap(red);
        else
            cnt[i] = 0;

    }
    update();
}
void MainWindow::move_car()
{
    static int i = 0;
    int d[20];

    for(int j=0 ; j< num -1; j++)
        d[j] = sqrt(pow(P_L[j+1].x-P_L[j].x,2) + pow(P_L[j+1].y-P_L[j].y,2));

    if(i!=num-1){ //untill arive
        cx+=(P_L[i+1].x-P_L[i].x)/(d[i]*2);
        cy+=(P_L[i+1].y-P_L[i].y)/(d[i]*2);
    }

    if( abs(cy - P_L[i+1].y) < 0.1 && abs(cx - P_L[i+1].x) < 0.1 )
        i++;

    ui->label_pic->move(cx-30,cy-12.5);

    update();
}
int readLine(int fd, char *str)
{
    int n;
    do {
        n = read(fd, str, 1);
    } while(n > 0 && *str++ != '\0');
    return n;
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
