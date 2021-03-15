#include "mainwindow.h"
#include"formstart.h"

#include <QApplication>
#include<QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try {
        MainWindow w;
        FormStart s;
        s.show();
        QTimer::singleShot(2500,&s,SLOT(close()));
        QTimer::singleShot(2500,&w,SLOT(show()));
        //  w.show();
        return a.exec();
    } catch (const std::bad_alloc &) {
        return EXIT_FAILURE;
    }

}
