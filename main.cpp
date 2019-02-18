#include "fmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FMainWindow w;
    w.show();

    return a.exec();
}
