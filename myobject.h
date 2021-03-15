#ifndef MYOBJECT_H
#define MYOBJECT_H

#include<QtCore>
#include<QThread>
#include<QObject>

class MyObject : public QObject
{
    Q_OBJECT
public:
    explicit MyObject(QObject *parent = nullptr);

    void DoSetup(QThread &cThread);

    bool stop=false;


signals:

    void receivedsend();

    public slots:
    void DoWork();

};

#endif // MYOBJECT_H
