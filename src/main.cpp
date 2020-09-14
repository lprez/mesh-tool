#include "mainwindow.h"
#include "pythonplugin.h"
#include "meshmodel.h"

#include <QApplication>
#include <QObject>


int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);

    MainWindow w;

    w.show();
    return a.exec();
}
