#include "qt_ui.h"

CQTui::CQTui(QObject *parent):QObject(parent)
{

}

CQTui::~CQTui()
{

}

QString CQTui::connectMysql(QString Name, QString Host, QString User,
                            QString Password, qint16 Port, QString Dbname)
{
    string name     = Name.toStdString();
    string host     = Host.toStdString();
    string user     = User.toStdString();
    string password = Password.toStdString();
    string dbname   = Dbname.toStdString();

    MYSQL* conn;
    conn = mysql_init((MYSQL*)0);

    if (mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(),
                           dbname.c_str(), Port, NULL, 0))
    {
        char value = 0;
        mysql_options(conn, MYSQL_OPT_RECONNECT, &value);
        m_DBConnetor[name] = conn;
        return "";
    }
    else
    {
        mysql_close(conn);
        return "connet db error";
    }
}

bool CQTui::closeMysql(QString Name)
{
    string name = Name.toStdString();
    map<string, MYSQL*>::iterator iter = m_DBConnetor.find(name);
    if (iter != m_DBConnetor.end())
    {
        mysql_close(m_DBConnetor[name]);
        return true;
    }
    else
        return false;
}

QVariantMap CQTui::queryMysql(QString Name, QString Sql)
{
    string name         = Name.toStdString();
    string sql          = Sql.toStdString();
    QVariant errmsg     = "";
    QVariant affectRows = 0;
    QVariantMap mapValue;
    QList<QVariant> resultList;

    map<string, MYSQL*>::iterator iter = m_DBConnetor.find(name);
    if (iter != m_DBConnetor.end())
    {
        MYSQL* conn = m_DBConnetor[name];
        MYSQL_RES *result;
        MYSQL_ROW sql_row;
        int res;

        mysql_query(conn, "SET NAMES latin1");
        res = mysql_query(conn, sql.c_str());
        affectRows = mysql_affected_rows(conn);

        if (!res)
        {
            result       = mysql_store_result(conn);
            int fieldNum = mysql_num_fields(result);
            if (result)
            {
                while (!!(sql_row = mysql_fetch_row(result)))
                {
                    QStringList tmp;
                    for (int i = 0; i < fieldNum; ++i)
                    {
                        tmp << sql_row[i];
                    }
                    resultList << QVariant(tmp);
                }
            }
            else
            {
                errmsg = "no result, query sql:" + Sql;
            }
        }
        else
        {
            errmsg = "mysql query error:" + Sql;
        }
    }
    else
    {
        errmsg = "DB:" + Name + "is undefined, Please confirm it is valid";
    }

    mapValue["errMsg"]     = errmsg;
    mapValue["affectRows"] = affectRows;
    mapValue["result"]     = QVariant(resultList);

    return mapValue;
}

QStringList CQTui::listDir(QString dirName)
{
    QStringList list;
    QDir dir(dirName);

    if (!dir.exists())
        return list;

    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);
    dir.setSorting(QDir::Name | QDir::IgnoreCase);
    QFileInfoList filelist = dir.entryInfoList();
    QFileInfoList::iterator iter = filelist.begin();

    for (; iter != filelist.end(); ++iter)
    {
        list << (*iter).fileName();
    }

    return list;
}

QString CQTui::getCurPath()
{
    return QDir::currentPath();
}

QString CQTui::setCurPath(QString dirName)
{
    QDir dir(dirName);
    if (!dir.exists())
        return "cannot find the dir";

    if (QDir::setCurrent(dirName))
        return "";
    else
        return "set error";
}

qint16 CQTui::getFileType(QString fileName)
{
    QFileInfo fileInfo(fileName);

    if (fileInfo.isFile())
        return 1;
    else if (fileInfo.isDir())
        return 2;
    else
        return 0;
}

QVariantList CQTui::getFileContent(QString fileName)
{
    QVariantList list;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        list << ("can not open file:" + fileName);
        return list;
    }

    QTextStream out(&file);

    while (!out.atEnd())
    {
        QString data = out.readLine();
        list << data;
    }

    return list;
}

bool CQTui::putFileContent(QString fileName, QString content, QString mode)
{
    QFile file(fileName);

    if (mode == "a")
    {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
            return false;
    }
    else if (mode == "w")
    {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
            return false;
    }
    else
        return false;

    QTextStream in(&file);
    in << content << "\r\n";
    return true;
}
