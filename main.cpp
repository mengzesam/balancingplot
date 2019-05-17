#include "mainwindow.h"
#include "balancingplot.h"
#include <QApplication>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //BalancingPlot w;
    w.show();

    return a.exec();
}
