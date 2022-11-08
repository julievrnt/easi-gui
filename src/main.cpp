#include "mainwindow.h"

#include <QApplication>
#define UNUSED(x) ((x)=(x))

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
