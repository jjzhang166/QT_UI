#include "mainfunc.h"

CDBData::CDBData(QObject *parent):QObject(parent)
{}

CDBData::~CDBData()
{
    Clear();
}

void CDBData::Init()
{
    Clear();
    time.start();
    GetDBConfig();
    qDebug() << time.elapsed()/1000.0<<"s";
    time.restart();
    GetTableConfig();
    qDebug() << time.elapsed()/1000.0<<"s";
    time.restart();
    //ConnectDB();
    qDebug() << time.elapsed()/1000.0<<"s";
   // m_SQuFu = {"w653", "w654", "w817", "w818", "w820", "w821", "w824", "w828", "w829", "w898", "w899"};
    m_SQuFu.insert("w653");
    m_SQuFu.insert("w654");
    m_SQuFu.insert("w817");
    m_SQuFu.insert("w818");
    m_SQuFu.insert("w820");
    m_SQuFu.insert("w821");
    m_SQuFu.insert("w824");
    m_SQuFu.insert("w828");
    m_SQuFu.insert("w829");
    m_SQuFu.insert("w898");
    m_SQuFu.insert("w899");

}

void CDBData::Clear()
{
    m_DBConfigList.clear();
    map<string, MYSQL*>::iterator iter = m_DBConnector.begin();
    for (; iter != m_DBConnector.end(); iter++)
    {
        MYSQL *conn = iter->second;
        mysql_close(conn);
    }
    m_DBConnector.clear();
    m_ErrorLogList.clear();
    m_ExceptionLogList.clear();
    m_ScriptLogList.clear();
    m_TableList.clear();
}

void CDBData::GetDBConfig()
{
    ifstream in;
    in.open("./config/config.txt");
    char buf[255];

    if (!in)
    {
        qDebug() << "config file open error";
        in.close();
        return;
    }
    //qDebug() << "config file open success";

    while (!in.eof())
    {
        memset(buf, 0, sizeof(buf));
        in.getline(buf, 255);

        vector<string> vecTmp = Split(buf, ",");
        if (vecTmp.size() != 6)
            break;

        DBConfig tmp;
        tmp.QuFuID   = vecTmp[0];
        tmp.QuFuName = vecTmp[1];
        tmp.IP       = vecTmp[2];
        tmp.UserName = vecTmp[3];
        tmp.PassWord = vecTmp[4];
        tmp.Port     = vecTmp[5];
        m_DBConfigList.push_back(tmp);
    }
    in.close();
    return;
}

void CDBData::GetTableConfig()
{
    ifstream in;
    in.open("./config/dbconfig.txt");
    char buf[255];

    if (!in)
    {
        qDebug() << "dbconfig file open error";
        in.close();
        return;
    }
    //qDebug() << "config file open success";

    while (!in.eof())
    {
        memset(buf, 0, sizeof(buf));
        in.getline(buf, 255);

        vector<string> vecTmp = Split(buf, ",");
        if (vecTmp.size() != 2)
            break;

        tableconfig tmp;
        tmp.strTableName   = vecTmp[0];
        tmp.strTable       = vecTmp[1];
        m_TableList.push_back(tmp);
    }
    in.close();
    return;
}

void CDBData::ConnectDB()
{
    if(m_DBConfigList.empty())
        return;

    list<DBConfig>::iterator iter = m_DBConfigList.begin();
    for (; iter != m_DBConfigList.end(); iter++)
    {
        DBConfig conf = *iter;
        MYSQL *conn;
        conn = mysql_init((MYSQL*)0);

        if (mysql_real_connect(conn, conf.IP.c_str(), conf.UserName.c_str(), conf.PassWord.c_str(),
                               "dblog", atoi(conf.Port.c_str()), NULL, 0))
        {
            char value = 0;
            mysql_options(conn, MYSQL_OPT_RECONNECT, &value);
            m_DBConnector[conf.QuFuID] = conn;
            //qDebug() << "success";
        }
        else
        {
            mysql_close(conn);
        }
    }
}

vector<string> CDBData::Split(string str, string p)
{
    vector<string> tmp;

    if (str.empty())
        return tmp;

    int pos = 0;
    int size = str.size();
    const char *d = p.c_str();

    for (int i = 0; i < size;i++)
    {
        pos = str.find(d, i);
        if (pos < size)
        {
            string s = str.substr(i, pos - i);
            tmp.push_back(s);
            i = pos;
        }
    }
    return tmp;
}

void CDBData::GetData(QString sql, QString qufu)
{
    m_ErrorLogList.clear();
    m_ExceptionLogList.clear();
    m_ScriptLogList.clear();

    map<string, MYSQL*> mapDBConnector;
    if (qufu.compare("all") == 0)
    {
        mapDBConnector = m_DBConnector;
    }
    else
    {
        if (m_DBConnector.find(qufu.toStdString()) != m_DBConnector.end())
            mapDBConnector[qufu.toStdString()] = m_DBConnector[qufu.toStdString()];
    }

    //qDebug() << m_DBConnector.size();
    //qDebug() << qufu.compare("all");
    //return;
    //qDebug() << sql;

    map<string, MYSQL*>::iterator iter = mapDBConnector.begin();
    unsigned int id = 1;
    for (; iter != mapDBConnector.end(); iter++)
    {
        string strQuFu = iter->first;
        MYSQL* conn    = iter->second;
        MYSQL_RES *result;
        MYSQL_ROW sql_row;
        int res;
        //qDebug() << flag << qufu;

        //if (flag == false && qufu.compare(QString::fromStdString(strQuFu)) != 0)
         //   continue;

        mysql_query(conn, "SET NAMES latin1");
        res = mysql_query(conn, sql.toStdString().c_str());

        if (!res)
        {
            result = mysql_store_result(conn);
            if (result)
            {
                while (!!(sql_row = mysql_fetch_row(result)))
                {
                    if (sql.contains("tblerrorlog", Qt::CaseInsensitive))
                    {
                        errorlog tmp;
                        tmp.nRowID      = id++;
                        tmp.strErrorMsg = sql_row[1];
                        tmp.strQuFu     = strQuFu;

                        if (m_SQuFu.find(strQuFu) != m_SQuFu.end())
                        {
                            tmp.strGS   = sql_row[3];
                            tmp.strTime = sql_row[2];
                        }
                        else
                        {
                            tmp.strGS       = sql_row[2];
                            tmp.strTime     = sql_row[3];
                        }

                        m_ErrorLogList.push_back(tmp);
                    }

                    if (sql.contains("tblexceptionlog", Qt::CaseInsensitive))
                    {
                        exceptionlog tmp;
                        tmp.nRowID      = id++;
                        tmp.strFile     = sql_row[1];
                        tmp.strLine     = sql_row[2];
                        tmp.strFuncName = sql_row[3];
                        tmp.strException= sql_row[4];
                        tmp.strTime     = sql_row[5];
                        tmp.strQuFu     = strQuFu;
                        m_ExceptionLogList.push_back(tmp);
                        //qDebug() << QString::fromStdString(tmp.strFile);
                    }

                    if (sql.contains("tblscripterrorlog", Qt::CaseInsensitive))
                    {
                        scriptlog tmp;
                        tmp.nRowID       = id++;
                        tmp.strFile      = sql_row[1];
                        tmp.strFunc      = sql_row[2];
                        tmp.strErrorInfo = sql_row[3];
                        tmp.strTime      = sql_row[4];
                        tmp.strQuFu      = strQuFu;
                        m_ScriptLogList.push_back(tmp);
                        //qDebug() << QString::fromStdString(tmp.strFile);
                        //qDebug() << QString::fromStdString(tmp.strErrorInfo);
                    }
                }//end while sql_row

            }//end if result
        }//end if res
       //break;
    }

}

QVariantList CDBData::GetTable()
{
    QVariantList vlist;

    list<tableconfig>::iterator iter = m_TableList.begin();
    for (; iter != m_TableList.end(); iter++)
    {
        tableconfig tmp = *iter;
        QVariantMap map;
        map["tablename"] = QString::fromStdString(tmp.strTableName);
        map["table"]     = QString::fromStdString(tmp.strTable);
        vlist << map;
    }

    return vlist;
}

QVariantList CDBData::ConvertToJson()
{
    QVariantList vlist;
    //qDebug() << "ConvertToJson";

    if ((!m_ErrorLogList.empty()) && m_ExceptionLogList.empty() && m_ScriptLogList.empty())
    {
        list<errorlog>::iterator iter = m_ErrorLogList.begin();
        for (; iter != m_ErrorLogList.end(); iter++)
        {
            errorlog tmp = *iter;
            QVariantMap map;
            map["rowid"]    = tmp.nRowID;
            map["errormsg"] = QString::fromStdString(tmp.strErrorMsg);
            map["gs"]       = QString::fromStdString(tmp.strGS);
            map["time"]     = QString::fromStdString(tmp.strTime);
            map["qufu"]     = QString::fromStdString(tmp.strQuFu);
            map["table"]    = "tblerrorlog";
            vlist << map;
        }
    }

    if ((!m_ExceptionLogList.empty()) && m_ErrorLogList.empty() && m_ScriptLogList.empty())
    {
        list<exceptionlog>::iterator iter = m_ExceptionLogList.begin();
        for (; iter != m_ExceptionLogList.end(); iter++)
        {
            exceptionlog tmp = *iter;
            QVariantMap map;
            map["rowid"]     = tmp.nRowID;
            map["file"]      = QString::fromStdString(tmp.strFile);
            map["line"]      = QString::fromStdString(tmp.strLine);
            map["funcname"]  = QString::fromStdString(tmp.strFuncName);
            map["exception"] = QString::fromStdString(tmp.strException);
            map["time"]      = QString::fromStdString(tmp.strTime);
            map["qufu"]      = QString::fromStdString(tmp.strQuFu);
            map["table"]     = "tblexceptionlog";
            vlist << map;
        }
    }

    if ((!m_ScriptLogList.empty()) && m_ErrorLogList.empty() && m_ExceptionLogList.empty())
    {
        list<scriptlog>::iterator iter = m_ScriptLogList.begin();
        for (; iter != m_ScriptLogList.end(); iter++)
        {
            scriptlog tmp = *iter;
            QVariantMap map;
            map["rowid"]     = tmp.nRowID;
            map["file"]      = QString::fromStdString(tmp.strFile);
            map["func"]      = QString::fromStdString(tmp.strFunc);
            map["errorinfo"] = QString::fromStdString(tmp.strErrorInfo);
            map["time"]      = QString::fromStdString(tmp.strTime);
            map["qufu"]      = QString::fromStdString(tmp.strQuFu);
            map["table"]     = "tblscripterrorlog";
            vlist << map;
        }
    }

    return vlist;
}

QVariantList CDBData::GetQuFu()
{
    QVariantList vlist;

    map<string, MYSQL*>::iterator iter = m_DBConnector.begin();
    for (; iter != m_DBConnector.end(); iter++)
    {
        string tmp = iter->first;
        vlist << QString::fromStdString(tmp);
    }

    return vlist;
}
