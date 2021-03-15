#include "mythread.h"
#include<QtCore>
#include<QMutex>
#include<QObject>

mythread::mythread(QObject* parent):
    QObject(parent)
{

}

mythread::~mythread()
{

}

void mythread::DoSetup(QThread &cThread)
{

    connect(&cThread,SIGNAL(started()),this,SLOT(Dowork()));
}



void mythread::DoWork()
{

    for(; ;)
    {
        QMutex mutex;
        mutex.lock();
        if(this->stop)
        {
            break;
        }
        mutex.unlock();
        QThread::msleep(100);
        qDebug()<<"in thread";
        emit receivedsend();
    }

}
