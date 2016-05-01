#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("DFTFringe");
    a.setApplicationName("DFTFringe");
    MainWindow w;

    w.show();

    return a.exec();
}
