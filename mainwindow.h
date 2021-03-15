#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTimer>
#include"mythread.h"
#include"numberpad.h"
#include"numberpadmotor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnDispManual_clicked();

    void on_btnDispAuto_clicked();

    void on_btnDispMotor_clicked();

    void on_btnDispGraph_clicked();

    //Manual
    void on_btnTouchOnOff1_clicked();

    void on_btnTouchOnOff2_clicked();

    void on_btnTouchOnOff3_clicked();

    void on_btnTouchOnOff4_clicked();

    void serialReceived();

    void on_pushButton_clicked();

    //Auto
    void on_btnStartStop_clicked();

    void displaycounter();

    void on_speedset_clicked();

//    void on_btnIntervalUp_clicked();

//    void on_btnIntervalDown_clicked();

    //Motor
    void on_pushButton_2_clicked();

    void on_setSpeed_clicked();

    void writeSpeed();

    //graph
    void writeTemperature_sample1();

    void on_pushButton_3_clicked();

    void on_btnIntervalUp_pressed();

    void on_btnIntervalUp_released();

    void on_btnIntervalDown_pressed();

    void on_btnIntervalDown_released();

    //numberpad
    void closedmethod2(QString);

    void closedmethod3(QString);

    private:
    Ui::MainWindow *ui;
    QSerialPort *device ;
    static const quint16 vendor_id=10442;
    static const quint16 product_id=32257;
    QString PortName="";
    bool device_is_available=false;

    //Manual
    QTimer *timermanual;

    //Auto

    int counter=1;

    int startstop=0;
    QTimer *timer;

    void counterstart();

    //Motor
    QTimer *timerMotor;

    int text;

    //Graph
    QString serialBuffer = "";
    QByteArray serialData= "";

    QTimer *timertemp;
    QTimer *timer2;
    QByteArray data2;
    QString output;
    int iMode = 0;

    void plot();
    QVector<double> x, y;
    void sample1();
    void sample2();

    //NumberPad
    numberpad *num;
    numberpadMotor *nummotor;

    signals:
    void fromled();

};
#endif // MAINWINDOW_H
