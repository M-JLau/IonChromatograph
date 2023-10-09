#include "controlpanel.h"
#include "ui_controlpanel.h"
#include "qcustomplot.h"
#include "controlpanel.h"

Controlpanel::Controlpanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Controlpanel)
{
    ui->setupUi(this);
    setWindowTitle("服务器端");
    //设置客户端窗口大小
    setFixedSize(1200,800);

    //默认选择Na和Cl
    ui->Na_CheckBox->setChecked(true);
    ui->Cl_CheckBox->setChecked(true);

    //Tcp通讯配置——————————————————————————————
    server = new QTcpServer(this);
    socket = new QTcpSocket(this);
    socket_list = new QList<QTcpSocket*>;
    client_list = new QList<QString>;
    //设置监听端口
    server->listen(QHostAddress::Any,6666);
    //连接新客户端连接的信号槽
    connect(server,&QTcpServer::newConnection,
            this,&Controlpanel::New_Connect);

    //——————————————————————————————————————————————————————————————————————————-
    //频率初始化
    ui->Rate_spinBox->setValue(50);
    ui->Rate_spinBox->setMaximum(100);
    Frequency_Time = ui->Rate_spinBox->value();
    //开始绘制
    setupRealtimeDataDemo(ui->customPlot);
    ui->customPlot->replot();



    /****************反馈定时器*****************/
//    timer = new QTimer();

//    timer->start(1000*10);
    /**************************************/
//    connect(timer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
    dataTimer.start(Frequency_Time);

}

Controlpanel::~Controlpanel()
{
    delete ui;
}

//获取新Tcp连接
void Controlpanel::New_Connect()
{
    //获取客户端连接
    socket = server->nextPendingConnection();
    qDebug() << "客户端:"
             << QHostAddress(socket->peerAddress().toIPv4Address()).toString();
    //连接QTcpSocket的信号槽，以读取新数据

    //连接读取信息的槽
    QObject::connect(socket, SIGNAL(readyRead()),
                     this, SLOT(socket_Read_Data()));
    //连接关闭Tcp槽
    QObject::connect(socket, SIGNAL(disconnected()),
                     this, SLOT(Disconnected()));


    //获取地址
    QString IP4 = QHostAddress(socket->peerAddress().toIPv4Address()).toString();

    //显示客户端ip
    client_list->push_back(IP4);
    ui->Client_list->clear();
    for(int i = 0; i < client_list->length(); i++){
        ui->Client_list->append(client_list->at(i));
    }

    //添加连接记录
    socket_list->push_back(socket);

}

//画图初始化
void Controlpanel::setupRealtimeDataDemo(QCustomPlot *customPlot)
{

   //设置画布颜色
     QLinearGradient plotGradient;
     plotGradient.setColorAt(0, QColor(191, 191, 191));//参数决定颜色
     ui->customPlot->setBackground(plotGradient);//把颜色加入CustomPlot

    //四种离子：Cu Na Cl SO4
    customPlot->addGraph(); // red line
    customPlot->graph(0)->setPen(QPen(Qt::red));
    customPlot->graph(0)->setName("Cu");

    customPlot->addGraph(); // blue line
    customPlot->graph(1)->setPen(QPen(Qt::blue));
    customPlot->graph(1)->setName("Na");

    customPlot->addGraph(); // red line
    customPlot->graph(2)->setPen(QPen(Qt::yellow));
    customPlot->graph(2)->setName("Cl");

    customPlot->addGraph(); // red line
    customPlot->graph(3)->setPen(QPen(Qt::green));
    customPlot->graph(3)->setName("SO4");

    //右上角标注信息
    customPlot->legend->setVisible(true);

    //横坐标
    customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    customPlot->xAxis->setAutoTickStep(false);
    customPlot->xAxis->setTickStep(2);
    customPlot->axisRect()->setupFullAxesBox();
    //曲线绘制定时器
//    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
//    dataTimer.start(Frequency_Time);
    qsrand(QTime::currentTime().msec() + QTime::currentTime().second() * 4000);

}



//绘制折线
void Controlpanel::realtimeDataSlot()
{
    //key的单位是秒
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    //使用随机数产生曲线
    CuValue = qrand() % 500;
    NaValue = qrand() % 1000;
    ClValue = qrand() % 2500;
    SO4Value = qrand() % 4000;
    QString message = QString("ad|%1|%2").arg(NaValue).arg(ClValue);
    socket_Send_Data(message);

    //判断选择的曲线
    CuFlag = ui->Cu_CheckBox->isChecked();
    NaFlag = ui->Na_CheckBox->isChecked();
    ClFlag = ui->Cl_CheckBox->isChecked();
    SO4Flag = ui->SO4_CheckBox->isChecked();

    if(CuFlag)
    {
        ui->customPlot->graph(0)->addData(key, CuValue);//添加Cu数据到曲线1
    }
    if(NaFlag)
    {
        ui->customPlot->graph(1)->addData(key, NaValue);//添加Na数据到曲线2
    }
    if(ClFlag)
    {
        ui->customPlot->graph(2)->addData(key,ClValue);//添加Cl数据到曲线2
    }
    if(SO4Flag)
    {
        ui->customPlot->graph(3)->addData(key,SO4Value);;//添加SO4数据到曲线2
    }

    //删除8秒之前的数据。这里的8要和下面设置横坐标宽度的8配合起来
    //才能起到想要的效果，可以调整这两个值，观察显示的效果。
    ui->customPlot->graph(0)->removeDataBefore(key-8);
    ui->customPlot->graph(1)->removeDataBefore(key-8);
    ui->customPlot->graph(2)->removeDataBefore(key-8);
    ui->customPlot->graph(3)->removeDataBefore(key-8);


    //自动设定graph(1)曲线y轴的范围，如果不设定，有可能看不到图像
    //也可以用ui->customPlot->yAxis->setRange(up,low)手动设定y轴范围
    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->graph(1)->rescaleValueAxis();
    ui->customPlot->graph(2)->rescaleValueAxis();
    ui->customPlot->graph(3)->rescaleValueAxis();


    //这里的8，是指横坐标时间宽度为8秒，如果想要横坐标显示更多的时间
    //就把8调整为比较大到值，比如要显示60秒，那就改成60。
    //这时removeDataBefore(key-8)中的8也要改成60，否则曲线显示不完整。
    ui->customPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);//设定x轴的范围
    ui->customPlot->replot();
}


//解析协议
void Controlpanel::Parsing_protocol(QString protocol)
{
    QStringList mid_message = protocol.split("|");
    QString str;

    //实时聊天协议(已写好，需要用时解除注释可以直接用)
//    if(mid_message[0]=="chat")
//    {
//        QString str_1=ui->textEdit_Recv->toPlainText();
//        str_1+=tr("chat:")+mid_message[1];
//        str_1+=tr("\n");
//        //刷新显示
//        ui->textEdit_Recv->setText(str_1);

//    }
    if(mid_message[0]=="led")
    {
        //获取Led信息
        QString mid_led = mid_message[1];
        Led = mid_led.toInt();

        switch(Led)
        {
        case 0:
            //点亮Led
         ui->LED1_label->setStyleSheet
                 ("QLabel {background-color: rgba(255, 170, 127, 0);}");
         ui->LED2_label->setStyleSheet
                 ("QLabel {background-color: rgba(255, 170, 127, 0);}");
         ui->LED3_label->setStyleSheet
                 ("QLabel {background-color: rgba(255, 170, 127, 0);}");
         ui->LED4_label->setStyleSheet
                 ("QLabel {background-color: rgba(255, 170, 127, 0);}");
         //显示操作信息
         str=ui->textEdit_Recv->toPlainText();
         str+=tr("Reset");
         str+=tr("\n");
         //刷新显示
         ui->textEdit_Recv->setText(str);

         break;

        case 1:

               //点亮Led
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED1");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);

            break;
        case 2:
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED2");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
            break;
        case 3:
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED1 & LED2");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
            break;
        case 4:
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED3");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
            break;
        case 5:
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED1 & LED3");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
            break;
        case 6:
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED2 & LED3");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
            break;
        case 7:
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED1 & LED2 & LED3");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
            break;
        case 8:
            ui->LED1_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED2_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED3_label->setStyleSheet
                    ("QLabel {background-color: rgba(255, 170, 127, 0);}");
            ui->LED4_label->setStyleSheet
                    ("QLabel {background-color: rgb(255, 170, 127);}");
            //显示操作信息
            str=ui->textEdit_Recv->toPlainText();
            str+=tr("点亮LED4");
            str+=tr("\n");
            //刷新显示
            ui->textEdit_Recv->setText(str);
            break;
        default:
            qDebug() << "数据错误";
            break;

        }
    }

}


//实时通信读取
void Controlpanel::socket_Read_Data()
{
    QByteArray buffer;
    //读取缓冲区数据
    //遍历客户端列表，所有客户端
    for(int i = 0; i < socket_list->length(); i++)
    {
        buffer = socket_list->at(i)->readAll();
        if(!(buffer.isEmpty()))
        {
            qDebug() << "读取消息";
            break;
        }
    }
    qDebug() << "Client:" <<  buffer;
    Parsing_protocol(buffer);
    socket_Send_Data(buffer);
    //反馈Led挡位信息
}


//void Controlpanel::Back_Send()
//{
//    QByteArray buffer;
//    buffer = socket->readAll();
//    if()
//    socket_Send_Data
//}

//实时通信发送
void Controlpanel::socket_Send_Data(QString infomation)
{
    QByteArray buffer_1 = infomation.toLatin1();
    for(int i = 0; i < socket_list->length(); i++){
        socket_list->at(i)->write(buffer_1);
    }
    socket->flush();
}

//客户端断开连接
void Controlpanel::Disconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());
    QString IP4 = QHostAddress(client->peerAddress().toIPv4Address()).toString();

    qDebug() << "客户端断开连接:"
             << IP4;
    client_list->removeAll(IP4);

    ui->Client_list->clear();
    for(int i = 0; i < client_list->length(); i++){
        ui->Client_list->append(client_list->at(i));
    }

    socket_list->removeOne(client);
}


//频率槽函数
void Controlpanel::on_Rate_spinBox_valueChanged(int arg1)
{

    Frequency_Time = arg1;

    //释放之前的计时器，创建新的计时器
    dataTimer.interval();
    dataTimer.start(Frequency_Time);

}
