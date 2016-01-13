#include "widget.h"
#include "mainfunc.h"
#include "ui_widget.h"
#include "qtwebkit.h"
#include <QtGui>
#include <QWebFrame>
#include <QWebView>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GBK"));
    //qw = new QtWebkit;
    qw = new CDBData;
    wv = new QWebView;
    pWebSettings = wv->page()->settings();
    pWebSettings->setAttribute(QWebSettings::JavascriptEnabled, true);


    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(wv);
    this->setLayout(layout);

    wf = wv->page()->mainFrame();
   // addObject();
    connect(wf, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addObject()));
    wv->load(QUrl("html/index.html"));
    ui->setupUi(this);
    this->resize(1400,900); //设置窗体大小
    qw->Init();
}

Widget::~Widget()
{
    delete ui;
    delete qw;
    delete wv;
}

void Widget::addObject()
{
    wf->addToJavaScriptWindowObject(QString("QT"), this->qw);
}
