#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLocale::setDefault(QLocale(QLocale::C));
    MainWindow w;
    w.show();

    return a.exec();
}
