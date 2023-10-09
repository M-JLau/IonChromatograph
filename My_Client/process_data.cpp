#include "process_data.h"
#include "ui_process_data.h"

Process_data::Process_data(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Process_data)
{
    ui->setupUi(this);
    thread_1=new MyThread();
    connect(thread_1, SIGNAL(TestSignal(int,int)), this, SLOT(DisplayMsg(int,int)));
    //重构曲线图
    setupRealtimeDataDemo(ui->CustomPlot_1);
    ui->CustomPlot_1->replot();
}

Process_data::~Process_data()
{
    delete ui;
}

void Process_data::DisplayMsg(int mid,int mid_2)
{
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    ui->CustomPlot_1->graph(0)->addData(key, mid);//添加Cu数据到曲线1
    ui->CustomPlot_1->graph(0)->removeDataBefore(key-8);
    ui->CustomPlot_1->graph(0)->rescaleValueAxis();
    ui->CustomPlot_1->graph(1)->addData(key, mid_2);//添加Cu数据到曲线1
    ui->CustomPlot_1->graph(1)->removeDataBefore(key-8);
    ui->CustomPlot_1->graph(1)->rescaleValueAxis();

    //横坐标时间自动堆积
    ui->CustomPlot_1->xAxis->setRange(key+0.25, 8, Qt::AlignRight);//设定x轴的范围
    ui->CustomPlot_1->replot();

}

//画图初始化
void Process_data::setupRealtimeDataDemo(QCustomPlot *customPlot)
{

   //设置画布颜色
     QLinearGradient plotGradient;
     plotGradient.setColorAt(0, QColor(191, 191, 191));//参数决定颜色
     ui->CustomPlot_1->setBackground(plotGradient);//把颜色加入CustomPlot

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

void Process_data::on_pushButton_clicked()
{
    thread_1->start();
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
}


void Process_data::on_pushButton_2_clicked()
{
    if (thread_1->isRunning()) {
        thread_1->stop();
        ui->pushButton->setEnabled(true);
        ui->pushButton_2->setEnabled(false);
    }
}
