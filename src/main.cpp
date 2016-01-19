#include "widget.h"
#include <QApplication>
#include <QWebView>
#include <QWebFrame>
#include "qt_ui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWebView view;
    CQTui qt;
    view.show();

    view.setUrl(QUrl("html/index.html"));
    view.page()->mainFrame()->addToJavaScriptWindowObject(QString("QT"), &qt);
    view.resize(1400,900);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

    return a.exec();
}
