#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "data.h"


class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent=0);
    void stop();
protected:
    //线程执行函数
    void run();
private:
    volatile bool stopped;
signals:
    void TestSignal(int,int);
};

#endif // MYTHREAD_H
