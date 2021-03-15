#ifndef MYTHREAD_H
#define MYTHREAD_H

#include<QtCore>
#include<QThread>
#include<QObject>


class mythread:public QObject
{
    Q_OBJECT
public:
    mythread();

//    void run();

    void DoSetup(QThread &cThread);

    bool stop=false;

signals:

void receivedsend();

public slots:
void DoWork();

};

#endif // MYTHREAD_H
