#include "mythread.h"
#include <QDebug>


MyThread::MyThread(QObject *parent):
    QThread(parent)
{
    stopped=false;
}

void MyThread::run()
{
    //调用SQLite驱动
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Value.db");
    db.open();
    QSqlQuery query;
    query.exec("select *from T_2018_12_09_12");
    while(query.next())
    {
//        qDebug()<<query.value(0).toInt()<<query.value(1).toInt();
//        触发信号
        emit TestSignal(query.value(0).toInt(),query.value(1).toInt());
        msleep(50);
    }
    stopped = false;
}

void MyThread::stop()
{
    stopped = true;
}
