#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QDataStream>
#include <QFontDatabase>
#include <QInputDialog>
#include "qcustomplot.h"
#include<QThread>
#include"numberpad.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("IOdemo");
    on_btnDispManual_clicked();     //to go to page 1 on startup
    ui->upperBar_2->setStyleSheet("border: 1px solid blue;");
    ui->lowerBar_2->setStyleSheet("border: 1px solid blue;");
    ui->btnTouchOnOff1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->btnTouchOnOff2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->btnTouchOnOff3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->btnTouchOnOff4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->btnTactSwOnOff1_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
    ui->btnTactSwOnOff2_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
    ui->btnTactSwOnOff3_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
    ui->btnTactSwOnOff4_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED7->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    ui->ceCellLED8->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px;");
    //ui->btnDispManual->setStyleSheet("background-color:gray;border-style: solid; border-radius: 10px;");
    ui->btnIntervalUp->setStyleSheet("border-image:url(:/new/prefix1/buttonup.jpg);");
    ui->btnIntervalDown->setStyleSheet("border-image:url(:/new/prefix1/buttondown.jpg);");
    ui->autoflag->setText("0");
    ui->autoflag->setVisible(false);

    //Graph
    ui->cellTempNow->setStyleSheet("border: 1px solid black;");
    //ui->btnDispGraph->setStyleSheet("background-color:gray;border-style: solid; border-radius: 10px;");
    ui->plot->xAxis->setTicks(false);
    ui->plot->yAxis->setTicks(false);
    ui->plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->tempUpperLimit->setText("温度スケール最大:100℃");
    ui->cellOrg->setText("原点：0℃");
    ui->label_4->setVisible(false);

    QFontDatabase::addApplicationFont(":/new/prefix1/ipaexg.ttf");
    //Serial Communication Configuration
    device= new QSerialPort;

    foreach(const QSerialPortInfo &Serialportinfo, QSerialPortInfo::availablePorts())
    {
        //qDebug()<<"has vendor id:"<<Serialportinfo.hasVendorIdentifier();
        if(Serialportinfo.hasVendorIdentifier()&&Serialportinfo.hasProductIdentifier())
        {
            if(Serialportinfo.vendorIdentifier()==vendor_id)
            {
                if(Serialportinfo.productIdentifier()==product_id)
                {
                    //qDebug()<<"Vendor ID:"<<Serialportinfo.vendorIdentifier();
                    //qDebug()<<"Product ID:"<<Serialportinfo.productIdentifier();
                    //qDebug()<<"PortName:"<<Serialportinfo.portName();
                    PortName=Serialportinfo.portName();
                    device_is_available=true;
                }
            }
        }
    }

    if(device_is_available==true)
    {
        device->setPortName(PortName);
        //device->open(QSerialPort::WriteOnly);
        device->setBaudRate(QSerialPort::Baud4800);
        device->setDataBits(QSerialPort::Data8);
        device->setParity(QSerialPort::NoParity);
        device->setStopBits(QSerialPort::OneStop);
        device->setFlowControl(QSerialPort::NoFlowControl);
        device->open(QIODevice::ReadWrite);
        //qDebug()<<"error code"<<device->error();
        //qDebug()<<"error string"<<device->errorString();

        QString Command="T:00000000,00000000'\r'";
        device->write(Command.toStdString().c_str());

        //Generate Timer event to read data from switch
        //timermanual = new QTimer(this);
        //QObject::connect(timermanual,SIGNAL(timeout()),this,SLOT(serialReceived()));
        //timermanual->start(5);
    }
    else
    {
        QMessageBox msgbox;
        QMessageBox::StandardButton reply;
        reply= QMessageBox::warning(this,"Port Error","通信ポートエラー\n\n通信ポートが開けません。",QMessageBox::Ok);
        if(reply==QMessageBox::Ok)
        {
            QApplication::quit();
        }
    }

    QFontDatabase::addApplicationFont(":/new/prefix1/ipaexg.ttf");

    //Auto
    QString val="100";
    ui->speedset->setText(val);



    //numberpad
    //led
    num= new numberpad(this);
    num->hide();
    connect(num,SIGNAL(tobesent(QString)),this,SLOT(closedmethod2(QString)));

    //motor
    nummotor= new numberpadMotor(this);
    nummotor->hide();
    connect(nummotor,SIGNAL(tobesent(QString)),this,SLOT(closedmethod3(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//manual
void MainWindow::on_btnDispManual_clicked()
{
    //Generate Timer event to read data from switch
    timermanual = new QTimer(this);
    QObject::connect(timermanual,SIGNAL(timeout()),this,SLOT(serialReceived()));
    timermanual->start(10);

    ui->stackedWidget->setCurrentIndex(0);
    ui->btnDispManual->setStyleSheet("background-color:gray;border-style: solid; border-radius: 10px;");
    ui->btnDispAuto->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispMotor->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispGraph->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
}

void MainWindow::on_btnDispAuto_clicked()
{
    timermanual->stop();

    ui->stackedWidget->setCurrentIndex(1);
    ui->btnDispManual->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispAuto->setStyleSheet("background-color:gray;border-style: solid; border-radius: 10px;");
    ui->btnDispMotor->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispGraph->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
}

void MainWindow::on_btnDispMotor_clicked()
{
    timermanual->stop();

    ui->stackedWidget->setCurrentIndex(2);
    ui->btnDispManual->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispAuto->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispMotor->setStyleSheet("background-color:gray;border-style: solid; border-radius: 10px;");
    ui->btnDispGraph->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
}

void MainWindow::on_btnDispGraph_clicked()
{
    timermanual->stop();

    ui->stackedWidget->setCurrentIndex(3);
    ui->btnDispManual->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispAuto->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispMotor->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.49, y1:0.722182, x2:0.500192, y2:0, stop:0.415459 rgba(0, 140, 126, 255), stop:1 rgba(147, 255, 213, 255));border-style: solid; border-radius: 10px;color: rgb(238, 238, 236);");
    ui->btnDispGraph->setStyleSheet("background-color:gray;border-style: solid; border-radius: 10px;");
}



void MainWindow::on_btnTouchOnOff1_clicked()
{
    static int i=1;
    if(i%2==1)
    {
        ui->btnTouchOnOff1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:1xxxxxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
            //qDebug()<<"can write at Led 1";
        }
        else
        {
            //qDebug()<<"cannot write at LED1";
        }
    }
    else
    {
        ui->btnTouchOnOff1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:0xxxxxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
            //qDebug()<<"can write at Led 1";
        }
        else
        {
            //qDebug()<<"cannot write at LED1";
        }
    }

    //    if(device->isWritable())
    //    {
    //        QString Command="T:10000000,00000000,'\r'";
    //        device->write(Command.toStdString().c_str());

    //        qDebug()<<"can write at Led 1";
    //    }
    //    else
    //    {
    //        qDebug()<<"cannot write at LED1";
    //    }
}


void MainWindow::on_btnTouchOnOff2_clicked()
{
    static int i=1;
    if(i%2==1)
    {
        ui->btnTouchOnOff2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:x1xxxxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
            //qDebug()<<"can write at LED2";
        }
        else
        {
            //qDebug()<<"cannot write at LED2";
        }
    }
    else
    {
        ui->btnTouchOnOff2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:x0xxxxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
            //qDebug()<<"can write at LED2";
        }
        else
        {
            //qDebug()<<"cannot write at LED2";
        }
    }
}

void MainWindow::on_btnTouchOnOff3_clicked()
{
    static int i=1;
    if(i%2==1)
    {
        ui->btnTouchOnOff3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:xx1xxxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
        }
        else
        {
            //qDebug()<<"cannot write at LED3";
        }
    }
    else
    {
        ui->btnTouchOnOff3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:xx0xxxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
        }
        else
        {
            //qDebug()<<"cannot write at LED3";
        }
    }
}

void MainWindow::on_btnTouchOnOff4_clicked()
{
    static int i=1;
    if(i%2==1)
    {
        ui->btnTouchOnOff4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:xxx1xxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
            //qDebug()<<"can write at LED4";
        }
        else
        {
            //qDebug()<<"cannot write at LED4";
        }
    }
    else
    {
        ui->btnTouchOnOff4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        i++;
        if(device->isWritable())
        {
            QString Command="T:xxx0xxxx,00000000,'\r'";
            device->write(Command.toStdString().c_str());
            //qDebug()<<"can write at LED4";
        }
        else
        {
            //qDebug()<<"cannot write at LED4";
        }
    }
}

void MainWindow::serialReceived()
{
    QStringList bufferSplit = serialBuffer.split(".");
    while(!device->atEnd()){
        QByteArray serialData = device->readAll();
        QString serialBuffer = QString::fromStdString(serialData.toStdString());
        QString Command="T:xxxxxxxx,xxxxxxxx,'\r'";
        device->write(Command.toStdString().c_str());

        QRegExp rx("(?:^|\n)R:([^\,]+)");
        QString ss = serialData;

        int poss = 0;
        while ((poss = rx.indexIn(ss, poss)) != -1)
        {
            QString output = rx.cap(0).split(":").last();
            //qDebug( )<< output;
            poss += rx.matchedLength();
            QString switchOFF = "00000000";
            QString switch1  = "10000000";
            QString switch2  = "01000000";
            QString switch3  = "00100000";
            QString switch4  = "00010000";
            QString switchall= "11110000";
            QString switch12 = "11000000";
            QString switch13 = "10100000";
            QString switch14 = "10010000";
            QString switch23 = "01100000";
            QString switch24 = "01010000";
            QString switch34 = "00110000";
            QString switch123 = "11100000";
            QString switch134 = "10110000";
            QString switch124 = "11010000";
            QString switch234 = "01110000";
            QString switch214 = "11010000";

            if(output==switchOFF)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px;");
            }

            if(output==switch1)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }


            if(output==switch2)
            {
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }

            if(output==switch3)
            {
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }


            if(output==switch4)
            {
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }

            if(output==switchall)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }

            if(output==switch12)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }

            if(output==switch13)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }

            if(output==switch14)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
            if(output==switch23)
            {
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
            if(output==switch24)
            {
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
            if(output==switch34)
            {
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
            if(output==switch123)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
            if(output==switch134)
            {
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
            if(output==switch234)
            {
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff3_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
            if(output==switch214)
            {
                ui->btnTactSwOnOff2_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff1_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
                ui->btnTactSwOnOff4_2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px;");
            }
        }
    }
}


//auto
void MainWindow::on_pushButton_clicked()
{
    if(device->isOpen())
    {
        device->close();
    }
    QApplication::quit();
}

void MainWindow::on_btnStartStop_clicked()
{
    ui->autoflag->setText("1");

    if(startstop==0)
    {
        ui->btnStartStop->setText("自動出力\nSTOP");
    }
    startstop ++;
    switch(startstop)
    {
    case 1:  timer = new QTimer(this);
        counterstart();

        break;
    case 2:
        ui->btnStartStop->setText("自動出力\nSTART");
        timer->stop(); //stop the timer to stop the count
        startstop=0;
    }
}

void MainWindow::counterstart()
{
    if(timer->isActive())
    {
        timer->stop();
        timer = new QTimer(this); //on stop timer gets killed ,hence make new object
    }

    QString speed= ui->speedset->text();
    if(device->isWritable())
    {
        //      if(speed.toInt()==100)
        //      {
        //           QString speed="0";
        //           timer->start(speed.toInt());

        //      }else{
        timer->start(speed.toInt());
    }
    connect(timer,SIGNAL(timeout()),this,SLOT(displaycounter()));
    //}
}

void MainWindow::displaycounter()
{
    QString s = QString::number(counter);
    ui->labelCount->setText(s);
    //    if(counter==0)
    //    {
    //        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //        ui->ceCellLED7->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //        ui->ceCellLED8->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
    //     QString Command="T:00000000,00000000,'\r'";
    //     device->write(Command.toStdString().c_str());
    //    }

    if(counter==1)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000000,00000000,'\r'";
        device->write(Command.toStdString().c_str());

    }
    if(counter==2)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==3)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==4)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==5)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==6)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==7)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==8)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==9)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==10)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==11)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==12)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==13)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==14)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==15)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }




    if(counter==16)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==17)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==18)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==19)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==20)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==21)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==22)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==23)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==24)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==25)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==26)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==27)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==28)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==29)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==30)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==31)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111000,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==32)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00000100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==33)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==34)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==35)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==36)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00100100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==37)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==38)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==39)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==40)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==41)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==42)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==43)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11010100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==44)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==45)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==46)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==47)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==48)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==49)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==50)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==51)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==52)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==53)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==54)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==55)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==56)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==57)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==58)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==59)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==60)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00111100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==61)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==62)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==63)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111100,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==64)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00000010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==65)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==66)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==67)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==68)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00100010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==69)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==70)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==71)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==72)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==73)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==74)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:0101001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==75)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:1101001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==76)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==77)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==78)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==79)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==80)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==81)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==82)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==83)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==84)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }

    if(counter==85)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==86)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==87)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==88)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==89)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==90)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==91)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==92)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00111010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==93)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==94)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==95)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111010,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==96)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00000110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==97)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==98)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==99)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==100)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00100110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==101)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==102)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==103)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==104)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==105)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==106)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==107)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11010110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==108)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==109)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==110)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==111)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==112)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==113)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==114)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==115)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==116)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==117)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==118)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==119)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==120)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==121)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==122)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==123)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==124)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00111110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==125)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==126)
    {
        ui->ceCellLED1->setStyleSheet("background-color: black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==127)
    {
        ui->ceCellLED1->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111110,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==128)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00000001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==129)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==130)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==131)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==132)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00100001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==133)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==134)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==135)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==136)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==137)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==138)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==139)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11010001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==140)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==141)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==142)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==143)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==144)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==145)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==146)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==147)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==148)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==149)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==150)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==151)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==152)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==153)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==154)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==155)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==156)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00111001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==157)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==158)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==159)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111001,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==160)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00000101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==161)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==162)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==163)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==164)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00100101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==165)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==166)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==167)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==168)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==169)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==170)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==171)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11010101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==172)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==173)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==174)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==175)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==176)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==177)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==178)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==179)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==180)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==181)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==182)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==183)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==184)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==185)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==186)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==187)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==188)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00111101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==189)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==190)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==191)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111101,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==192)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00000011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==193)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==194)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==195)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==196)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00100011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==197)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==198)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==199)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==200)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==201)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==202)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==203)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11010011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==204)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==205)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==206)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==207)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==208)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==209)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==210)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==211)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==212)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==213)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==214)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==215)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==216)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==217)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==218)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==219)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==220)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00111011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==221)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==222)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==223)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111011,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==224)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00000111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==225)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10000111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==226)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01000111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==227)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11000111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==228)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00100111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==229)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10100111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==230)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01100111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==231)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11100111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==232)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00010111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==233)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10010111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==234)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01010111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==235)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11010111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==236)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00110111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==237)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10110111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==238)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01110111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==239)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11110111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==240)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00001111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==241)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10001111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==242)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01001111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==243)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11001111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==244)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00101111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==245)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10101111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==246)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01101111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==247)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11101111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==248)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00011111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==249)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10011111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==250)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01011111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==251)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11011111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==252)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:00111111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==253)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:10111111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==254)
    {
        ui->ceCellLED1->setStyleSheet("background-color:black;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:01111111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    if(counter==255)
    {
        ui->ceCellLED1->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED2->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED3->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED4->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED5->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED6->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED7->setStyleSheet("background-color:red;border-style: solid; border-radius: 20px; ");
        ui->ceCellLED8->setStyleSheet("background-color: red;border-style: solid; border-radius: 20px; ");
        QString Command="T:11111111,00000000,'\r'";
        device->write(Command.toStdString().c_str());
    }
    counter++;
    if(counter>255)
    {
        counter =1;
    }
}

void MainWindow::on_speedset_clicked()
{
    num->show();
}


//motor
void MainWindow::on_pushButton_2_clicked()
{
    int text= ui->setSpeed->text().toInt();
    //qDebug()<<"running timer for motor";
    QString hexvalue = QString("%1").arg(text, 4, 16, QLatin1Char( '0' ));
    QString Command= QString("T:xxxxxxxx,xxxxxxxx,%1'\r'").arg(hexvalue);
    device->write(Command.toStdString().c_str());
}

void MainWindow::writeSpeed()
{
    //   qDebug()<<"running timer for motor";
    // QString Command="T:xxxxxxxx,xxxxxxxx,0100,'\r'";
    // device->write(Command.toStdString().c_str());
    // static int count=0;
    //    int text= ui->setSpeed->text().toInt();
    //    qDebug()<<"text"<<text;
    // count++;

    // QByteArray serialData = device->readAll();
    //     qDebug()<<serialData;
    //     qDebug()<<"count"<<count;
    // if(count>10000)
    // {
    //    timerMotor->stop();
    //     count=0;

    // }
    //   int text= ui->setSpeed->text().toInt();
    //    qDebug()<<"inside motor timer";
    //        if(device->isWritable())
    //        {

    //    for(int i=0;i<1000;i++)
    //    {
    //            QString Command="T:xxxxxxxx,xxxxxxxx,0100,'\r'";
    //            device->write(Command.toStdString().c_str());
    //            qDebug()<<"motor counter "<<i;
    //            if(i==1000)
    //            {
    //             timerMotor->stop();
    //            }

    //    }
    //    timerMotor->stop();
    //    qDebug()<<text;
}

void MainWindow::on_setSpeed_clicked()
{
    //    emit fromled();
    nummotor->show();
    //    int n=QInputDialog::getInt(this,"Speed","Input(10-65535)");
    //    ui->setSpeed->setText(QString::number(n));
}

//graph

void MainWindow::sample1()
{
    //if(timertemp->isActive())
    //{
    //    timertemp->stop();
    timertemp = new QTimer(this);
    //}
    timertemp->start(100);
    connect(timertemp, SIGNAL(timeout()),this, SLOT(writeTemperature_sample1()));
    ui->cellTempNow->setGeometry(41, 276, 168, 29);
    ui->plot->yAxis->setRange(0,100);
    //writeTemprature();
}

void MainWindow::sample2()
{
    //timertemp->stop();
    //timer2 = new QTimer(this);
    //timer2->start(100);
    //connect(timer2, SIGNAL(timeout()),this, SLOT(writeTemperature_sample1()));
    ui->plot->yAxis->setRange(10,40);
    ui->tempUpperLimit->setText("温度スケール最大:40℃");
    ui->cellOrg->setText("原点：10℃");
    //timer2->start(100);
    //writeTemprature();
}

void MainWindow::writeTemperature_sample1()
{
    //qDebug()<<"this is graphtimer";
    QString Command="T:xxxxxxxx,xxxxxxxx,'\r'";
    device->write(Command.toStdString().c_str());
    QByteArray serialData = device->readAll();
    //qDebug()<<serialData;
    //qDebug()<<"serial works";
    QRegExp rx("(?:|\n),([^\\r]+)");
    //chop serial data to avoid cluttering
    QString sserialdata = serialData;
    // qDebug()<<sserialdata;
    QString ss=serialData.mid(0,sserialdata.indexOf("\r"));
    //qDebug()<<ss;
    ui->plot->addGraph();
    //static int xaxisvalue=0;

    int poss = 0;
    while ((poss = rx.indexIn(ss, poss)) != -1)
    {
        QString output = rx.cap(0).split(",").last();
        //qDebug()<< "output"<<rx;
        poss += rx.matchedLength();
        bool bStatus = false;
        uint nHex = output.toUInt(&bStatus,16);
        //qDebug()<<nHex;
        nHex = nHex >> 3;
        //qDebug()<<nHex;
        float Tmp = (float)nHex * 0.0625;
        //qDebug()<<Tmp;
        //for inside sampling
        if(ui->label_4->text()=="0")
        {
            ui->cellTempNow->setGeometry(41, 276, 355, 29);
            QString tmpLower = QString("現在温度：%1℃(サンプリング停止中)").arg(QString::number(Tmp, 'f', 1));
            ui->cellTempNow->setText(tmpLower);
        }
        else{
            QString tmpLower = QString("現在温度：%1℃").arg(QString::number(Tmp, 'f', 1));
            ui->cellTempNow->setText(tmpLower);
        }

        //         QString tmpUpper = QString("温度スケール最大:100℃").arg(QString::number(Tmp, 'f', 1));
        //         ui->tempUpperLimit->setText(tmpUpper);

        //GraphPlot
        static int xaxisvalue=0;

        //adjustment
        //           QString adjust = ui->speedset->text();
        //           int adj=adjust.toInt();
        //           if(adj<500 && (startstop==1))
        //           {
        //               x.append(xaxisvalue-10);
        //           }
        //           else
        //           {
        x.append(xaxisvalue);
        //           }
        y.append(Tmp);
        // create graph and assign data to it:

        ui->plot->graph(0)->setData(x, y);
        // give the axes some labels:
        //ui->plot->xAxis->setLabel("x");
        //ui->plot->yAxis->setLabel("y");
        // set axes ranges, so we see all data:
        //adjust when led is glowing at speed less than 500


        //          if(adj<500 && (startstop==1))
        //          {
        //              ui->plot->xAxis->setRange(0,1250);
        //          }
        //          else
        //          {
        ui->plot->xAxis->setRange(0,500);
        //          }
        //          ui->plot->yAxis->setRange(0,100);
        ui->plot->xAxis->setTicks(false);
        ui->plot->yAxis->setTicks(false);

        ui->plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        ui->plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
        ui->plot->replot();

        xaxisvalue++;

        //         if(adj<500 && (startstop==1))
        //         {
        //             if(xaxisvalue>1250)
        //             {
        //             xaxisvalue=0;
        //             x.clear();
        //             y.clear();
        //             }
        //         }
        //         else
        //         {
        if(xaxisvalue>500)
        {
            xaxisvalue=0;
            x.clear();
            y.clear();
        }
        //          }
        //         //to start from zero on start again
        //         if(ui->label_4->text()=="1")
        //     {
        //          xaxisvalue=0;
        //          x.clear();
        //          y.clear();
        //          ui->label_4->setText("2");
        //          timer2->stop();
        //          ui->cellTempNow->setGeometry(31, 346, 168, 29);
        //         }
        if(ui->label_4->text()=="1")
        {
            xaxisvalue=0;
            x.clear();
            y.clear();
            ui->label_4->setText("2");
            //timer2->stop();
            ui->cellTempNow->setGeometry(41, 276, 168, 29);
        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(iMode==0)
    {
        ui->pushButton_3->setText("サンプリング\nスケール変更");
    }
    iMode ++;
    switch(iMode)
    {
    case 1:  /*qDebug()<<1;*/
        ui->tempUpperLimit->setText("温度スケール最大:100℃");
        ui->cellOrg->setText("原点：0℃");
        sample1();
        break;
    case 2:  /*qDebug()<<2;*/
        //timertemp->stop();
        ui->pushButton_3->setText("サンプリング\nストップ");
        ui->label_4->setText("0");
        sample2();
        break;
    case 3:
        //qDebug()<<"stop";
        timertemp->stop();
        //qDebug()<<"timer temp stopped";
        ui->pushButton_3->setText("サンプリング\nスタート");
        ui->label_4->setText("1");
        iMode=0;
    }
}


//auto
void MainWindow::on_btnIntervalUp_pressed()
{
    ui->btnIntervalUp->setStyleSheet("border-image:url(:/new/prefix1/buttondown.jpg);");
}

void MainWindow::on_btnIntervalUp_released()
{
    ui->btnIntervalUp->setStyleSheet("border-image:url(:/new/prefix1/buttonup.jpg);");
    QString autoflag= ui->autoflag->text();
    int flagauto= autoflag.toInt();
    QString speed= ui->speedset->text();
    int newspeed = speed.toInt()+10;
    if(newspeed>10000)
    {
        newspeed=10000;
    }
    ui->speedset->setText(QString::number(newspeed));
    if(!(flagauto==0))
    {
        if(startstop==1)
        {
            counterstart();
        }
        else{
            timer->stop();
            startstop=1;
            timer = new QTimer(this);
            on_btnStartStop_clicked();
        }
    }

}

void MainWindow::on_btnIntervalDown_pressed()
{
    ui->btnIntervalDown->setStyleSheet("border-image:url(:/new/prefix1/buttonup.jpg);");
}

void MainWindow::on_btnIntervalDown_released()
{
    ui->btnIntervalDown->setStyleSheet("border-image:url(:/new/prefix1/buttondown.jpg);");
    QString autoflag= ui->autoflag->text();
    int flagauto= autoflag.toInt();
    QString speed= ui->speedset->text();
    int newspeed = speed.toInt()-10;
    if(newspeed<50)
    {
        newspeed=50;

    }
    ui->speedset->setText(QString::number(newspeed));
    if(!(flagauto==0))
    {
        if(startstop==1)
        {
            counterstart();
        }
        else{
            timer->stop();
            startstop=1;
            timer = new QTimer(this);
            on_btnStartStop_clicked();
        }
    }
}


//numberpad
void MainWindow::closedmethod2(QString i)
{
    QString valtobutton=i;
    ui->speedset->setText(valtobutton);
    if(startstop==1)
    {
        //on_btnStartStop_clicked();
        counterstart();
    }
    else
    {
        startstop=1;
        timer = new QTimer(this);
        on_btnStartStop_clicked();
    }
    num->close();
}

void MainWindow::closedmethod3(QString i)
{
    QString valtobutton=i;
    ui->setSpeed->setText(valtobutton);
    nummotor->close();
}

