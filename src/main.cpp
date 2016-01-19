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

    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanCloseWindows,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);

    view.page()->mainFrame()->addToJavaScriptWindowObject(QString("QT"), &qt);
    view.setUrl(QUrl("index.html"));

    view.resize(1400,900);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));

    return a.exec();
}
