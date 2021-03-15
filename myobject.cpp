#include "myobject.h"
#include<QtCore>
#include<QMutex>
#include<QObject>

MyObject::MyObject(QObject *parent) : QObject(parent)
{

}


void MyObject::DoSetup(QThread &cThread)
{

    connect(&cThread,SIGNAL(started()),this,SLOT(DoWork()));
}



void MyObject::DoWork()
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
        qDebug()<<"in thread myobject";
        emit receivedsend();
    }

}
