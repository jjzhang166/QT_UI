#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtGui>
class QtWebkit;
class QWebView;
class QWebSettings;
class QWebFrame;
class CDBData;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    virtual ~Widget();

private slots:
    void addObject();

private:
    QWebView *wv;
    QWebSettings *pWebSettings;
    QWebFrame *wf;
    //QtWebkit *qw;
    CDBData *qw;
private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
