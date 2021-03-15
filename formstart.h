#ifndef FORMSTART_H
#define FORMSTART_H

#include <QMainWindow>

namespace Ui {
class FormStart;
}

class FormStart : public QMainWindow
{
    Q_OBJECT

public:
    explicit FormStart(QWidget *parent = nullptr);
    ~FormStart();

private:
    Ui::FormStart *ui;

private slots:
    void on_pushButton_clicked();
};

#endif // FORMSTART_H
