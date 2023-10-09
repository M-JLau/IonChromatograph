#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "qcustomplot.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString tempInfoNa;
    QString tempInfoCl;
    //数据库
    int n;
//    int num=0;//计数器
    void Graph_Show(QCustomPlot *customPlot);
    void addDataToGraph(int data);

//    void socket_Read_Data();

    //建立SQLite连接
    void Write_SQL_data(int Na_mid_data,int Cl_mid_data);



public slots:
    //数据可视化
    void Graph_Show();
    //客户端Tcp通信
    void on_pushButton_Connect_clicked();



////    发送备注信息
//    void on_pushButton_Send_clicked();


    void socketReadData(QString *tempInfo1, QString *tempInfo2);

    void socketReadData();

    void socket_Disconnected();

    //发送LED挡位信息
    void on_gear1_clicked();

    void on_gear2_clicked();

    void on_gear3_clicked();

    void on_gear4_clicked();

    void on_gear5_clicked();

    void on_gear6_clicked();

    void on_gear7_clicked();

    void on_gear8_clicked();

    void on_gearAuto_clicked();

//    //数据保存
//    void Save_Data();


private slots:

    //保存事件
    void on_pushButton_Save_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;

    //判断是否保存
    bool if_save=false;
};

#endif // MAINWINDOW_H
