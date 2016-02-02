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
    QString connectMysql(QString Name, QString Host, qint16 Port, QString User,
                         QString Password, QString Dbname, QString strCharset = "");
    bool closeMysql(QString Name);
    //数据库操作接口
    QVariantMap queryMysql(QString Name, QString Sql);

    //文件读写接口
    QStringList listDir(QString dirName = "./");
    QString getCurPath();
    QString setCurPath(QString dirName);
    qint16 getFileType(QString fileName);
    QString getFileContent(QString fileName);
    bool putFileContent(QString fileName, QString content, QString mode);

    //!string tool
    QString strFilter(QString strData, int begin = 0, int lenth = 0);
    QString strEncode(QString strData);

    QVariantMap pbcToJs(QString protoData, QString strVal, QString strData);
    QString charestDecode(QString strData);

    QVariantMap jsToPbc(QString protoFile, QString strMessName, QVariantMap srcMap);
private:
    map<string, MYSQL*> m_DBConnetor;
};
#endif // QT_UI

