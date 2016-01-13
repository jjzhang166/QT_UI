#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowFlags(Qt::Window);
 //   w.showMaximized();

    w.show();

    return a.exec();
}