#include "numberpadmotor.h"
#include "ui_numberpadmotor.h"
#include<QDebug>


numberpadMotor::numberpadMotor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::numberpadMotor)
{
    ui->setupUi(this);
    ui->label->setText("");
    ui->titleNumberpad->setText("入力(10-65535)");
    ui->label->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);");
}

numberpadMotor::~numberpadMotor()
{
    delete ui;
}

void numberpadMotor::on_pushButton1_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"1");
}

void numberpadMotor::on_pushButton2_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"2");
}

void numberpadMotor::on_pushButton3_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"3");
}

void numberpadMotor::on_pushButton4_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"4");
}

void numberpadMotor::on_pushButton5_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"5");
}

void numberpadMotor::on_pushButton6_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"6");
}

void numberpadMotor::on_pushButton7_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"7");
}

void numberpadMotor::on_pushButton8_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"8");
}

void numberpadMotor::on_pushButton9_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"9");
}

void numberpadMotor::on_pushButtondel_clicked()
{
    //  QString value= ui->label->text();
    //  ui->label->setText(value);
    ui->label->setText("");
}

void numberpadMotor::on_pushButtonESC_clicked()
{
    ui->label->setText("");
    this->hide();
}

void numberpadMotor::on_pushButtonOk_clicked()
{
    QString value=ui->label->text();
    int valint=value.toInt();
    if(valint>65535)
    {
        value="65535";
        emit tobesent(value);
    }
    else
    {
        emit tobesent(value);
    }
    if(valint<10)
    {
        value="10";
        emit tobesent(value);
    }
    else
    {
        emit tobesent(value);
    }


    ui->label->setText("");

}

void numberpadMotor::on_pushButton0_clicked()
{
    QString value= ui->label->text();
    ui->label->setText(value+"0");
}

void numberpadMotor::toledmethod()
{
}

void numberpadMotor::on_pushButtonX_clicked()
{
    ui->label->setText("");
    this->hide();
}


void numberpadMotor::on_pushButtonBS_clicked()
{
    QString number=ui->label->text();
    number.chop(1);
    ui->label->setText(number);
}
