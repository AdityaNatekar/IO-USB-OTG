#ifndef NUMBERPAD_H
#define NUMBERPAD_H

#include <QWidget>
#include<QMainWindow>


namespace Ui {
class numberpad;
}

class numberpad : public QWidget
{
    Q_OBJECT

public:
    explicit numberpad(QWidget *parent = nullptr);
    ~numberpad();

private slots:
    void on_pushButton1_clicked();

    void on_pushButton2_clicked();

    void on_pushButton3_clicked();

    void on_pushButton4_clicked();

    void on_pushButton5_clicked();

    void on_pushButton6_clicked();

    void on_pushButton7_clicked();

    void on_pushButton8_clicked();

    void on_pushButton9_clicked();

    void on_pushButtondel_clicked();

    void on_pushButtonESC_clicked();

    void on_pushButtonOk_clicked();

    void on_pushButton0_clicked();

    void on_pushButtonX_clicked();

    void on_pushButtonBS_clicked();

private:
    Ui::numberpad *ui;

public:
    void toledmethod();

signals:
    void tobesent(QString);
};

#endif // NUMBERPAD_H
