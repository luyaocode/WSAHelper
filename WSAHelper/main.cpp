#include "WSAHelper.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WSAHelper w;
    w.show();
    return a.exec();
}
