#ifndef PROCESS_DATA_H
#define PROCESS_DATA_H

#include <QWidget>
#include "mythread.h"
#include "qcustomplot.h"
#include "data.h"

namespace Ui {
class Process_data;
}

class Process_data : public QWidget
{
    Q_OBJECT

public:
    explicit Process_data(QWidget *parent = 0);
    ~Process_data();
    void setupRealtimeDataDemo(QCustomPlot *customPlot);

private:
    Ui::Process_data *ui;
    MyThread *thread_1;

public slots:
    void DisplayMsg(int,int);
private slots:
//    void on_pushButton_clicked();
//    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};

#endif // PROCESS_DATA_H
