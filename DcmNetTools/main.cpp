#include "DcmNetTools.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DcmNetTools w;
    w.show();
    return a.exec();
}
