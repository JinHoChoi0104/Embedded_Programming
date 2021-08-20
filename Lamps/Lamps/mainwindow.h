#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPainter>

#include <iostream>
#include <cmath>

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

private slots:
    void lamp_ch();
    void move_car();

private:
    Ui::MainWindow *ui;
    QTimer *timer;

};
#endif // MAINWINDOW_H
