#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>

#include <iostream>
#include <cmath>
#include <QPushButton>
#include "QLineEdit"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void paintEvent(QPaintEvent *event);
   // int readLine(int fd, char* str);

  //  int size; //how many node


private slots:
    void on_pushButton_clicked();
    void move_car();
    void on_pushButton_2_clicked(); 
    void fromSelected();
    void on_to_node3_clicked();
    void on_to_node5_clicked();
    void on_to_node12_clicked();
    void on_to_node16_clicked();

    void on_from_node0_clicked();
    void on_from_node1_clicked();
    void on_from_node2_clicked();
    void on_from_node4_clicked();
    void on_from_node6_clicked();
    void on_from_node7_clicked();
    void on_from_node8_clicked();
    void on_from_node9_clicked();
    void on_from_node10_clicked();
    void on_from_node11_clicked();
    void on_from_node13_clicked();
    void on_from_node14_clicked();
    void on_from_node15_clicked();
    void on_from_node17_clicked();
    void on_from_node18_clicked();
    void on_from_node19_clicked();
    void on_to_node20_clicked();
    void handleButton();

private:
  QPushButton *m_button;

private:
    Ui::MainWindow *ui;
    QTimer *timer;
};
#endif // MAINWINDOW_H
