#ifndef QT_UI
#define QT_UI
#include <QtGui>
#include <QSqlDatabase>
#include <winsock2.h>
#include <windows.h>
#include <winsock.h>
#include <QDebug>
#include <string>
#include <cstring>
#include <map>
#include "mysql.h"
#include <QTime>
#include <QVariant>
#include <QDir>
#include <QFileInfoList>
#include <QFileInfo>
#include <QFile>
using namespace std;

class CQTui  : public QObject
{
Q_OBJECT
public:
    CQTui(QObject *parent=0);
    virtual ~CQTui();

public slots:
    //数据库连接 删除接口
    QString connectMysql(QString Name, QString Host, QString User,
                         QString Password, qint16 Port, QString Dbname);
    bool closeMysql(QString Name);
    //数据库操作接口
    QVariantMap queryMysql(QString Name, QString Sql);

    //文件读写接口
    QStringList listDir(QString dirName = "./");
    QString getCurPath();
    QString setCurPath(QString dirName);
    qint16 getFileType(QString fileName);
    QVariantList getFileContent(QString fileName);
    bool putFileContent(QString fileName, QString content, QString mode);

private:
    map<string, MYSQL*> m_DBConnetor;
};
#endif // QT_UI

