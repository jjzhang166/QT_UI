#ifndef QTWEBKIT
#define QTWEBKIT
#include <QtGui>
#include <QSqlDatabase>
#include <winsock2.h>
#include <windows.h>
#include <QMap>
#include <QList>
#include "mysql.h"

struct RESULT{
    std::string itemlist;
    unsigned int countlist;
    unsigned int yuanbaolist;
    std::string time;
};

struct CONFIG{
    QString localhost;
    QString username;
    QString password;
    QString port;
};

struct ITEM{
    QString catagory1;
    QString catagory2;
    QString itemname;
    QString   isDura;
};

class QtWebkit : public QObject
{
    Q_OBJECT
public:
    QtWebkit(QObject *parent=0);
    ~QtWebkit();

public slots:
    bool getData(QString qufu, QString sql, QString flag);
    QVariantList convertToJson();
    void getItem(qint8 flag);
    bool getConfig();
    QVariantList getItemList();
    QVariantList getQuFu();

public:
    std::vector<std::string> split(std::string str, std::string pattern);

private:
    std::vector<RESULT> m_results;
    QMap<QString, CONFIG> config;

};

#endif // QTWEBKIT

