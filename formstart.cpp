#include "formstart.h"
#include "ui_formstart.h"

FormStart::FormStart(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FormStart)
{
    ui->setupUi(this);
    ui->cellMessage->setStyleSheet("border-top: 2px solid black;border-left: 2px solid black;border-right: 2px solid white;border-bottom: 2px solid white;background-color: rgb(255, 255, 255);");
}

FormStart::~FormStart()
{
    delete ui;
}

void FormStart::on_pushButton_clicked()
{
    QApplication::quit();
}
