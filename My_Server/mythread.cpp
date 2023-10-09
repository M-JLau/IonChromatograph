#include <QDebug>
#include "mythread.h"

MyThread::MyThread(QObject *parent):
    QThread(parent)
{
    stopped=false;
}

void MyThread::run()
{
    qreal i=0;
    while(!stopped)
    {
        qDebug()<<QString("in MyThread:%1").arg(i);
        msleep(1000);
        i++;

    }
    stopped=false;

    while(true)
    {
        //qDebug() << LED;
/*
        if(FLAG)
        switch (getLed())
        {
        case 1://001
            digitalWrite(1, HIGH);
            digitalWrite(3, LOW);
            digitalWrite(5, LOW);
            digitalWrite(25, LOW);
            break;
        case 2://010
            digitalWrite(1, LOW);
            digitalWrite(3, HIGH);
            digitalWrite(5, LOW);
            digitalWrite(25, LOW);
            break;
        case 3://011
            digitalWrite(1, HIGH);
            digitalWrite(3, HIGH);
            digitalWrite(5, LOW);
            digitalWrite(25, LOW);
            break;
        case 4://100
            digitalWrite(1, LOW);
            digitalWrite(3, LOW);
            digitalWrite(5, HIGH);
            digitalWrite(25, LOW);
            break;
        case 5://101
            digitalWrite(1, HIGH);
            digitalWrite(3, LOW);
            digitalWrite(5, HIGH);
            digitalWrite(25, LOW);
            break;
        case 6://110
            digitalWrite(1, LOW);
            digitalWrite(3, HIGH);
            digitalWrite(5, HIGH);
            digitalWrite(25, LOW);
            break;
        case 7://111
            digitalWrite(1, HIGH);
            digitalWrite(3, HIGH);
            digitalWrite(5, HIGH);
            digitalWrite(25, LOW);
            break;
        case 8://110
            digitalWrite(1, LOW);
            digitalWrite(3, LOW);
            digitalWrite(5, LOW);
            digitalWrite(25, HIGH);
            break;
        default:
            digitalWrite(1, LOW);
            digitalWrite(3, LOW);
            digitalWrite(5, LOW);
            digitalWrite(25, LOW);
            break;
        }*/
    }
}
