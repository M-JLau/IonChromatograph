#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include "qcustomplot.h"
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class Controlpanel;
}

class Controlpanel : public QWidget
{
    Q_OBJECT

public:
    explicit Controlpanel(QWidget *parent = 0);
    ~Controlpanel();
    //设置折线属性
    void setupRealtimeDataDemo(QCustomPlot *customPlot);
    //解析协议
    void Parsing_protocol(QString protocol);

private:
    Ui::Controlpanel *ui;
    //定时器，周期调用realtimeDataSlot()槽，实现动态数据添加到曲线
    QTimer dataTimer;

    //Led
    int Led;
    //设置频率时间
    int Frequency_Time;

    //********
    QTimer *timer;

    //Tcp通信_服务器
    QTcpSocket *socket;
    QTcpServer *server;

    //客户端列表
    QList<QTcpSocket*> *socket_list;
    QList<QString> *client_list;

private:
    //离子数据
    int CuValue;
    int NaValue;
    int ClValue;
    int SO4Value;


    //选择离子种类
    bool CuFlag;
    bool NaFlag;
    bool ClFlag;
    bool SO4Flag;

private slots:
    //绘制曲线
    void realtimeDataSlot();

    //建立Tcp新连接
    void New_Connect();

    //实时通信读取
    void socket_Read_Data();

    //实时通信发送
    void socket_Send_Data(QString infomation);


    //断开连接
    void Disconnected();
    void on_Rate_spinBox_valueChanged(int arg1);
};

#endif // CONTROLPANEL_H
