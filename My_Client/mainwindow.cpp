#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>
#include <QTime>
#include <QTimer>
#include <time.h>
#include "qcustomplot.h"
#include "process_data.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("客户端");

    socket = new QTcpSocket();

    //连接信号槽
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadData()));
    QObject::connect(socket, &QTcpSocket::disconnected,
                     this, &MainWindow::socket_Disconnected);
    //发送按键失能
//    ui->pushButton_Send->setEnabled(false);
    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("6666");

    //构建数据可视化
    Graph_Show(ui->CustomPlot);

    //设置警告字体
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::red);
    ui->labelState->setPalette(pe);

    //链接数据库


}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::Graph_Show()
{
    Graph_Show(ui->CustomPlot);
}

void MainWindow::Graph_Show(QCustomPlot *CustomPlot)
{


   //设置画布颜色
    QLinearGradient plotGradient;
    plotGradient.setColorAt(0, QColor(80, 80, 80));//（80，80，80）参数决定颜色
    ui->CustomPlot->setBackground(plotGradient);//把颜色加入CustomPlot

   //设置坐标颜色/坐标名称颜色
    ui->CustomPlot->xAxis->setLabelColor(Qt::white);//文字颜色
    ui->CustomPlot->yAxis->setLabelColor(Qt::white);
    ui->CustomPlot->xAxis->setTickLabelColor(Qt::white);//坐标轴数字颜色
    ui->CustomPlot->yAxis->setTickLabelColor(Qt::white);
    ui->CustomPlot->xAxis->setBasePen(QPen(Qt::white, 1));//坐标轴颜色及宽度
    ui->CustomPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    ui->CustomPlot->xAxis->setTickPen(QPen(Qt::white, 1));//主刻度
    ui->CustomPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    ui->CustomPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));//副刻度
    ui->CustomPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));

    //添加第一条曲线
    CustomPlot->addGraph();
    CustomPlot->graph(0)->setPen(QPen(Qt::green)); //0是曲线序号，添加的第一条是0，设置曲线颜色
    CustomPlot->graph(0)->setName("Na+");//设置曲线名字
    //添加第二条曲线
    CustomPlot->addGraph();
    CustomPlot->graph(1)->setPen(QPen(Qt::yellow));
    CustomPlot->graph(1)->setName("Cl-");

    //设置实时时间
    CustomPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    CustomPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    CustomPlot->xAxis->setAutoTickStep(false);
    CustomPlot->xAxis->setTickStep(2);

    //设置各变量注释
    CustomPlot->axisRect()->setupFullAxesBox();
    CustomPlot->legend->setVisible(true);

    CustomPlot->xAxis->setLabel("time");//x轴的含义
    CustomPlot->yAxis->setLabel("浓度");//y轴的含义

    CustomPlot->replot();//可视化重绘
}


//Tcp通讯读取数据
void MainWindow::socketReadData()
{
    socketReadData(&tempInfoNa, &tempInfoCl);
}


//根据协议解读信息
void MainWindow::socketReadData(QString *tempInfo1, QString *tempInfo2)
{
    QDateTime dateTime;
    QByteArray buffer;
    buffer = socket->readAll();
    if(!buffer.isEmpty())
    {
//        qDebug() << tr(buffer);
        QString info = buffer;
        QList<QString> infoList = info.split('|');
        if (infoList[0] == "ad")
        {
            if(if_save)
            {
                Write_SQL_data(infoList[1].toInt(),infoList[2].toInt());
            }
            //获取实时时间
            QString timestr = dateTime.currentDateTime().toString("yyyyMMddHHmmss");
            if (infoList[1].toInt() && ui->radioButton->isChecked())
            {
                *tempInfo1 += infoList[1] + "." + timestr + "|";
                //qDebug() << "tempNa:" << tempInfo1->toLatin1();
                //LED显示屏显示获取到的AD值

                ui->lcdNumber->display(infoList[1].toInt());

                addDataToGraph(infoList[1].toInt());

                //设置Na离子的警告浓度
                if (infoList[1].toInt() > 950)
                {
                    ui->labelState->setText("warning!Na overmaxvalue!");
                    qDebug() << "Send: Na warning overflow" ;
                }
            }
            if (infoList[2].toInt() && ui->radioButton_2->isChecked())
            {
                *tempInfo2 += infoList[2] + "." + timestr + "|";
                //qDebug() << "tempCl:" << tempInfo1->toLatin1();
                ui->lcdNumber->display(infoList[2].toInt());
                addDataToGraph(infoList[2].toInt());

                //设置Cl离子的警告浓度
                if (infoList[2].toInt() > 2400)
                {
                    ui->labelState->setText("warning!Na overflow!");
                    qDebug() << "Send: Na warning overflow" ;
                }
            }
        }
        else if (infoList[0] == "led")
        {
            ui->groupBox->setTitle("当前档位" + infoList[1]);
            QString str = ui->textEdit_Recv->toPlainText();
            str+=tr("当前挡位：")+infoList[1];
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
        }
//        else if (infoList[0] == "war")
//        {

//        }
//        else if(infoList[0] == "close")
//        {

//        }

    }
    else
    {
       qDebug() <<"未获取到数据！";
    }

    //判断是否保存数据
//    if()

}

void MainWindow::addDataToGraph(int data)
{
    // 从字符串转换为毫秒（需完整的年月日时分秒）
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
//    // 从字符串转换为秒（需完整的年月日时分秒）
//    datetime.fromString("2011-09-10 12:07:50:541", "yyyy-MM-dd hh:mm:ss:zzz").toTime_t();

    //判断是那种离子
    if (ui->radioButton->isChecked())
    {
        ui->CustomPlot->graph(0)->addData(key, data);
    }
    if (ui->radioButton_2->isChecked())
    {
        ui->CustomPlot->graph(1)->addData(key, data);
    }
    ui->CustomPlot->graph(0)->removeDataBefore(key-8);
    ui->CustomPlot->graph(1)->removeDataBefore(key-8);
    ui->CustomPlot->graph(0)->rescaleValueAxis();
    ui->CustomPlot->graph(1)->rescaleValueAxis(true);
    ui->CustomPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
    ui->CustomPlot->replot();
}


//建立TCP连接
void MainWindow::on_pushButton_Connect_clicked()
{
    if(ui->pushButton_Connect->text() == tr("连接"))
    {
        QString IP;
        int port;

        //获取IP地址
        IP = ui->lineEdit_IP->text();
        //获取端口号
        port = ui->lineEdit_Port->text().toInt();

        //取消已有的连接
        socket->abort();
        //连接服务器
        socket->connectToHost(IP, port);

        //等待连接成功
        if(!socket->waitForConnected(30000))
        {
            qDebug() << "Connection failed!";
            return;
        }
        qDebug() << "Connect successfully!";

//        //发送按键使能
//        ui->pushButton_Send->setEnabled(true);
        //修改按键文字
        ui->pushButton_Connect->setText("断开连接");
    }
    else
    {
        //断开连接
        socket->disconnectFromHost();
        //修改按键文字
        ui->pushButton_Connect->setText("连接");
        //发送按键失能
//        ui->pushButton_Send->setEnabled(false);
    }
}


//已在上面协议解析里面完成重构，此处可注释掉
////Socket通讯_读取
//void MainWindow::socket_Read_Data()
//{
//    QByteArray buffer;
//    //读取缓冲区数据
//    buffer = socket->readAll();
//    QString mid=buffer;
//    if(!buffer.isEmpty())
//    {
//        QStringList back_message = mid.split("|");
//        if(back_message[0]=="led")
//        {
//            QString str = ui->textEdit_Recv->toPlainText();
//            str+=tr("当前挡位：")+back_message[1];
//            str+=tr("\n");
//            //刷新显示
//            ui->textEdit_Recv->setText(str);

//        }
//    }
//}

//发送LED挡位信息
void MainWindow::on_gear1_clicked()
{
    qDebug() << "Send: LED1" ;
    socket->write("led|1");
    socket->flush();
}

void MainWindow::on_gear2_clicked()
{
    qDebug() << "Send: LED1" ;
    socket->write("led|2");
    socket->flush();
}

void MainWindow::on_gear3_clicked()
{
    qDebug() << "Send: LED3" ;
    socket->write("led|3");
    socket->flush();
}

void MainWindow::on_gear4_clicked()
{
    qDebug() << "Send: LED4" ;
    socket->write("led|4");
    socket->flush();
}

void MainWindow::on_gear5_clicked()
{
    qDebug() << "Send: LED5" ;
    socket->write("led|5");
    socket->flush();
}

void MainWindow::on_gear6_clicked()
{
    qDebug() << "Send: LED6" ;
    socket->write("led|6");
    socket->flush();
}

void MainWindow::on_gear7_clicked()
{
    qDebug() << "Send: LED7" ;
    socket->write("led|7");
    socket->flush();
}

void MainWindow::on_gear8_clicked()
{
    qDebug() << "Send: LED8" ;
    socket->write("led|8");
    socket->flush();
}

void MainWindow::on_gearAuto_clicked()
{
    qDebug() << "Send: Reset" ;
    socket->write("led|0");
    socket->flush();
}


//(已写好，考虑到离子色谱仪服务器端无法搭载此功能，暂时注释掉)
////实时聊天
//void MainWindow::on_pushButton_Send_clicked()
//{
//    qDebug() << "Send: " << ui->textEdit_Send->toPlainText();
//     //获取文本框内容并以ASCII码形式发送
//    QString str_1="chat|";
//    str_1+=ui->textEdit_Send->toPlainText();
//    QByteArray buffer = str_1.toLatin1();
//    socket->write(buffer);
//    socket->flush();
//}

void MainWindow::socket_Disconnected()
{
//    //发送按键失能
//    ui->pushButton_Send->setEnabled(false);
    //修改按键文字
    ui->pushButton_Connect->setText("连接");
    qDebug() << "Disconnected!";
}


//数据库相关操作
void MainWindow::on_pushButton_Save_clicked()
{

    //调用SQLite驱动
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    if_save=!if_save;
    if(if_save==true)
    {
    //链接数据库
        db.setDatabaseName("Value.db");
        if (!db.open())
        {
                QMessageBox::critical(0, "Cannot open database",
                                  "Unable to establish a database connection.",
                                      QMessageBox::Cancel);
        }
        else
        {
            qDebug()<<"开始保存";
            ui->pushButton_Save->setText("停止数据存储");
        }
    }
    else
    {
        //关闭数据库
        db.close();
        qDebug()<<"停止保存";
        ui->pushButton_Save->setText("开始数据存储");
    }
}
void MainWindow::Write_SQL_data(int Na_mid_data,int Cl_mid_data)
{
    //或许当前存档建表的时间
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy_MM_dd_hh");
    QString table_name="create table T_"+current_date+" (Na int, Cl int)";
    QSqlQuery query;
    query.exec(table_name);
    QString mid_Str_1=QString::number(Na_mid_data);
    QString mid_Str_2=QString::number(Cl_mid_data);
    QString str_1="insert into T_"+current_date+" (Na,Cl) values("+mid_Str_1+","+mid_Str_2+")";
    query.exec(str_1);

}



//创建一个新的process_data类型的窗口
void MainWindow::on_pushButton_clicked()
{
    Process_data *data=new Process_data();
    data->show();
}
