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
int from;
int to;
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

    ui->from_node0->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node1->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node2->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node4->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node6->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node7->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node8->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node9->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node10->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node11->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node13->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node14->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node15->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node17->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node18->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");
    ui->from_node19->setStyleSheet("border-image:url(/home/pi/Downloads/blueplace.png)");

    ui->to_node3->setStyleSheet("border:0px");
    ui->to_node5->setStyleSheet("border:0px");
    ui->to_node12->setStyleSheet("border:0px");
    ui->to_node16->setStyleSheet("border:0px");
    ui->to_node20->setStyleSheet("border:0px");

    // Connect button signal to appropriate slot

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
  //  a.exec();
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
    //write(cfd, c, strlen(c)+1); //send starting point

    str = ui->line2->text();
    strcpy(c, str.toStdString().c_str()); //convert QString to char
    //write(cfd, c, strlen(c)+1); //send destination
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->line1->setText(aa1);
    ui->line2->setText(aa2);
}
void MainWindow::on_to_node3_clicked()
{
    ui->to_node3->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "3";
    write(cfd, c, strlen(c)+1);
    ui->to_node5->setStyleSheet("border:0px");
    ui->to_node12->setStyleSheet("border:0px");
    ui->to_node16->setStyleSheet("border:0px");
    ui->to_node20->setStyleSheet("border:0px");
}
void MainWindow::on_to_node5_clicked()
{
    ui->to_node5->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "5";
    write(cfd, c, strlen(c)+1);
    ui->to_node3->setStyleSheet("border:0px");
    ui->to_node12->setStyleSheet("border:0px");
    ui->to_node16->setStyleSheet("border:0px");
    ui->to_node20->setStyleSheet("border:0px");
}
void MainWindow::on_to_node12_clicked()
{
    ui->to_node12->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "12";
    write(cfd, c, strlen(c)+1);
    ui->to_node3->setStyleSheet("border:0px");
    ui->to_node5->setStyleSheet("border:0px");
    ui->to_node16->setStyleSheet("border:0px");
    ui->to_node20->setStyleSheet("border:0px");
}
void MainWindow::on_to_node16_clicked()
{
    ui->to_node16->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "16";
    write(cfd, c, strlen(c)+1);
    ui->to_node3->setStyleSheet("border:0px");
    ui->to_node5->setStyleSheet("border:0px");
    ui->to_node12->setStyleSheet("border:0px");
    ui->to_node20->setStyleSheet("border:0px");
}
void MainWindow::on_to_node20_clicked()
{
    ui->to_node20->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "20";
    write(cfd, c, strlen(c)+1);
    ui->to_node3->setStyleSheet("border:0px");
    ui->to_node5->setStyleSheet("border:0px");
    ui->to_node12->setStyleSheet("border:0px");
    ui->to_node16->setStyleSheet("border:0px");
}
void MainWindow::fromSelected()
{
    ui->to_node3->setStyleSheet("border-image:url(/home/pi/Downloads/yellowplace.png)");
    ui->to_node5->setStyleSheet("border-image:url(/home/pi/Downloads/yellowplace.png)");
    ui->to_node12->setStyleSheet("border-image:url(/home/pi/Downloads/yellowplace.png)");
    ui->to_node16->setStyleSheet("border-image:url(/home/pi/Downloads/yellowplace.png)");
    ui->to_node20->setStyleSheet("border-image:url(/home/pi/Downloads/yellowplace.png)");
    ui->from_node0->setStyleSheet("border:0px");
    ui->from_node1->setStyleSheet("border:0px");
    ui->from_node2->setStyleSheet("border:0px");
    ui->from_node4->setStyleSheet("border:0px");
    ui->from_node6->setStyleSheet("border:0px");
    ui->from_node7->setStyleSheet("border:0px");
    ui->from_node8->setStyleSheet("border:0px");
    ui->from_node9->setStyleSheet("border:0px");
    ui->from_node10->setStyleSheet("border:0px");
    ui->from_node11->setStyleSheet("border:0px");
    ui->from_node13->setStyleSheet("border:0px");
    ui->from_node14->setStyleSheet("border:0px");
    ui->from_node15->setStyleSheet("border:0px");
    ui->from_node17->setStyleSheet("border:0px");
    ui->from_node18->setStyleSheet("border:0px");
    ui->from_node19->setStyleSheet("border:0px");
}
void MainWindow::on_from_node0_clicked()
{
    ui->from_node0->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "0";
    write(cfd, c, strlen(c)+1);
    fromSelected();
    m_button = ui->from_node0;
    connect(m_button, &QPushButton::released, this, &MainWindow::handleButton);
}
void MainWindow::on_from_node1_clicked()
{
    ui->from_node1->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "1";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node2_clicked()
{
    ui->from_node2->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "2";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node4_clicked()
{
    ui->from_node4->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "4";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node6_clicked()
{
    ui->from_node6->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "6";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node7_clicked()
{
    ui->from_node7->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "7";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node8_clicked()
{
    ui->from_node8->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "8";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node9_clicked()
{
    ui->from_node9->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "9";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node10_clicked()
{
    ui->from_node10->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "10";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node11_clicked()
{
    ui->from_node11->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "11";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node13_clicked()
{
    ui->from_node13->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "13";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node14_clicked()
{
    ui->from_node14->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "14";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node15_clicked()
{
    ui->from_node15->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "15";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node17_clicked()
{
    ui->from_node17->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "17";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node18_clicked()
{
    ui->from_node18->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "18";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}
void MainWindow::on_from_node19_clicked()
{
    ui->from_node1->setStyleSheet("border-image:url(/home/pi/Downloads/redplace.png)");
    char c[MAXLINE] = "19";
    write(cfd, c, strlen(c)+1);
    fromSelected();
}

void MainWindow::handleButton()
{
  // change the text
  m_button->setStyleSheet("");
  // resize button
  m_button->resize(100,100);
}
