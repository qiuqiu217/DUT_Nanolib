#include "mainwindow.h"

#include <QApplication>
#include "bluetooth/dataprocess.h"

DataProcess *dataL;
Algorithm *algorithm;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dataL = new DataProcess();
    algorithm = new Algorithm();
    MainWindow w;
    w.show();
    return a.exec();
}
