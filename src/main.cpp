#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setOrganizationName("Zhang");
    QApplication::setApplicationName("CheckIP");

    MainWindow w;
    w.show();
    return a.exec();
}
