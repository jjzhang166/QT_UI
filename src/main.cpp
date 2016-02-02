#include "widget.h"
#include <QApplication>
#include <QWebView>
#include <QWebFrame>
#include "qt_ui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir::setCurrent("./html");

    QWebView view;
    CQTui qt;
    view.show();

    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanCloseWindows,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard,true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);

    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::AutoLoadImages,true);

    QWebSettings::globalSettings()->setDefaultTextEncoding("utf-8");
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));

    //QTextCodec *codec=QTextCodec::codecForName("utf8");
    //QTextCodec::setCodecForLocale(codec);
    //QTextCodec::setCodecForCStrings(codec);
    //QTextCodec::setCodecForTr(codec);

    view.page()->mainFrame()->addToJavaScriptWindowObject(QString("QT"), &qt);
    view.setUrl(QUrl("index.html"));

    view.resize(1400,900);


    return a.exec();
}
