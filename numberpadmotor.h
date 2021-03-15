#ifndef NUMBERPADMOTOR_H
#define NUMBERPADMOTOR_H

#include <QWidget>

namespace Ui {
class numberpadMotor;
}

class numberpadMotor : public QWidget
{
    Q_OBJECT

public:
    explicit numberpadMotor(QWidget *parent = nullptr);
    ~numberpadMotor();

private:
    Ui::numberpadMotor *ui;

public slots:
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

public:
    void toledmethod();

signals:
    void tobesent(QString);

private slots:
    void on_pushButtonBS_clicked();
};

#endif // NUMBERPADMOTOR_H
