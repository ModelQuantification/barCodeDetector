#include "widget.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":logo/img/barcode.png"));
    Widget w;
    w.show();
    return a.exec();
}
