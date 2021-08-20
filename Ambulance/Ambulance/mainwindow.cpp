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

void get_root();
int readLine(int fd, char* str);

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

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap bg("/home/pi/Downloads/map.png");
    bg =bg.scaled(800,450);
    QPalette p(palette());
    p.setBrush(QPalette::Background, bg);
    setAutoFillBackground(true);
    setPalette(p);

    QPixmap pix;
    QPixmap pix2("/home/pi/Downloads/car.png");
    pix = pix2.scaled(60, 60, Qt::KeepAspectRatio);
    ui->label_pic->setPixmap(pix);

    timer = new QTimer(this);
    timer -> start(30);
    connect(timer,SIGNAL(timeout()),this,SLOT(move_car()));
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

        aa1 = QString::number(_x); //int -> qstr
        aa2 = QString::number(_y); //int -> qstr
    }
    cx = P_L[0].x;
    cy = P_L[0].y;
}
int readLine(int fd, char *str)
{
    int n;
    do {
        n = read(fd, str, 1);
    } while(n > 0 && *str++ != '\0');
    return n;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter myline(this);
    QPen linePen(Qt::red);
    linePen.setWidth(5);
    myline.setPen(linePen);

    for(int i=0; i< num -1;i++)
        myline.drawLine(P_L[i].x,P_L[i].y,P_L[i+1].x,P_L[i+1].y);
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

void MainWindow::on_pushButton_clicked()
{
    char c[MAXLINE];
    QString str = ui->line1->text(); //get text from line
    strcpy(c, str.toStdString().c_str()); //convert QString to char
    write(cfd, c, strlen(c)+1); //send starting point

    str = ui->line2->text();
    strcpy(c, str.toStdString().c_str()); //convert QString to char
    write(cfd, c, strlen(c)+1); //send destination

}

void MainWindow::on_pushButton_2_clicked()
{
    ui->line1->setText(aa1);
    ui->line2->setText(aa2);
}
