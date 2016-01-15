#ifndef MAINFUNC
#define MAINFUNC
#include <QtGui>
#include <QSqlDatabase>
#include <winsock2.h>
#include <windows.h>
#include <QMainWindow>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <winsock.h>
#include "mysql.h"
#include <QTime>
using namespace std;

struct DBConfig{
    string QuFuID;
    string QuFuName;
    string IP;
    string UserName;
    string PassWord;
    string Port;
};

struct errorlog{
    unsigned int nRowID;
    string strErrorMsg;
    string strGS;
    string strQuFu;
    string strTime;
};

struct scriptlog{
    unsigned int nRowID;
    string strFile;
    string strFunc;
    string strErrorInfo;
    string strQuFu;
    string strTime;
};

struct exceptionlog{
    unsigned int nRowID;
    string strFile;
    string strLine;
    string strFuncName;
    string strException;
    string strQuFu;
    string strTime;
};

struct tableconfig{
    string strTableName;
    string strTable;
};

class CDBData  : public QObject
{
Q_OBJECT
public:
    CDBData(QObject *parent=0);
    virtual ~CDBData();

    void Clear();

public slots:
    void Init();
    void GetDBConfig();
    void GetTableConfig();
    void ConnectDB();
    void GetData(QString sql, QString qufu);
    vector<string> Split(string str, string p = ",");
    QVariantList GetTable();
    QVariantList ConvertToJson();
    QVariantList GetQuFu();

private:
    list<DBConfig> m_DBConfigList;
    map<string, MYSQL*> m_DBConnector;
    list<errorlog> m_ErrorLogList;
    list<scriptlog> m_ScriptLogList;
    list<exceptionlog> m_ExceptionLogList;
    list<tableconfig> m_TableList;
    set<string> m_SQuFu;
    QTime time;

};

#endif // MAINFUNC

