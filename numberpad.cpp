#include "numberpad.h"
#include "ui_numberpad.h"
#include<QDebug>


numberpad::numberpad(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::numberpad)
{
    ui->setupUi(this);
    ui->label->setText("");
    ui->titleNumberpad->setText("入力(50-10000)");
    ui->label->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);");
}

numberpad::~numberpad()
{
    delete ui;
}

void numberpad::on_pushButton1_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"1");
}

void numberpad::on_pushButton2_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"2");
}

void numberpad::on_pushButton3_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"3");
}

void numberpad::on_pushButton4_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"4");
}

void numberpad::on_pushButton5_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"5");
}

void numberpad::on_pushButton6_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"6");
}

void numberpad::on_pushButton7_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"7");
}

void numberpad::on_pushButton8_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"8");
}

void numberpad::on_pushButton9_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"9");
}

void numberpad::on_pushButtondel_clicked()
{
    //QString value= ui->label->text();
    //ui->label->setText(value);
    ui->label->setText("");
}

void numberpad::on_pushButtonESC_clicked()
{
    ui->label->setText("");
    this->hide();
}

void numberpad::on_pushButtonOk_clicked()
{
    QString value=ui->label->text();
    int valint=value.toInt();
    if(valint>10000)
    {
        value="10000";
        emit tobesent(value);
    }
    else
    {
        emit tobesent(value);
    }
    if(valint<50)
    {
        value="50";
        emit tobesent(value);
    }
    else
    {
        emit tobesent(value);
    }

    ui->label->setText("");
}

void numberpad::on_pushButton0_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"0");
}

void numberpad::toledmethod()
{
}

void numberpad::on_pushButtonX_clicked()
{
    ui->label->setText("");
    this->hide();
}

void numberpad::on_pushButtonBS_clicked()
{
    QString number=ui->label->text();
    number.chop(1);
    ui->label->setText(number);
}
