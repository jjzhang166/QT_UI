#include "qt_ui.h"
#include "ByteBuffer.h"

CQTui::CQTui(QObject *parent):QObject(parent)
{
    QDir::setCurrent("./html");
}

CQTui::~CQTui()
{
    map<string, MYSQL*>::iterator iter = m_DBConnetor.begin();
    for (; iter != m_DBConnetor.end(); ++iter)
    {
        MYSQL *conn = iter->second;
        mysql_close(conn);
    }
    m_DBConnetor.clear();
}

QString CQTui::connectMysql(QString Name, QString Host, qint16 Port, QString User,
                            QString Password, QString Dbname)
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
        string errmsg = mysql_error(conn);
        mysql_close(conn);
        return QString::fromStdString(errmsg);
    }
}

bool CQTui::closeMysql(QString Name)
{
    string name = Name.toStdString();
    map<string, MYSQL*>::iterator iter = m_DBConnetor.find(name);
    if (iter != m_DBConnetor.end())
    {
        mysql_close(m_DBConnetor[name]);
        m_DBConnetor.erase(iter);
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
                    unsigned long *lengths = mysql_fetch_lengths(result);
                    QStringList tmp;
                    for (int i = 0; i < fieldNum; ++i)
                    {
                        string data;
                        data.append(sql_row[i], lengths[i]);
                        tmp << QString::fromStdString(data);
                    }
                    resultList << QVariant(tmp);
                }
            }
            else
            {
                errmsg = mysql_error(conn);
            }
        }
        else
        {
            errmsg = mysql_error(conn);
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

QString CQTui::getFileContent(QString fileName)
{
    QString str = "";
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        str = "can not open file:" + fileName;
        return str;
    }

    QTextStream out(&file);

    while (!out.atEnd())
    {
        QString data = out.readLine();
        str += data;
    }

    return str;
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
/*
 * static int luaA_QueryHexString(lua_State *L)
{
    const char* pInValue = ((const char*)  tolua_tostring(L,1,0));
    int nInLen = ((int)  tolua_tonumber(L,2,0));

    std::vector<uint8> _storage;
    _storage.reserve(nInLen);
    _storage.resize(nInLen);
    memcpy(&_storage[0], pInValue, nInLen);
    for (size_t i = 0; i < _storage.size(); ++i)
    {
        uint8 value = _storage[i];
        if (value == 0)
        {
            _storage.erase(_storage.begin() + i, _storage.end());
        }
        else
        {
            switch (value)
            {
            case 255:
                _storage[i] = 0;
                break;
            case 253:
            case 254:
                {
                    if (_storage[i] == 253 && _storage[i+1] == 254) _storage[i] = 255;
                    _storage.erase(_storage.begin() + i + 1);
                }
                break;
            }
        }
    }
    lua_pushlstring(L, (const char*)&_storage[0], _storage.size());
    return 1;
}

static int luaA_BuildHexString(lua_State *L)
{
    const char* pInValue = ((const char*)  tolua_tostring(L,1,0));
    int nInLen = ((int)  tolua_tonumber(L,2,0));

    std::vector<uint8> _storage;
    _storage.reserve(nInLen);
    _storage.resize(nInLen);
    memcpy(&_storage[0], pInValue, nInLen);
    for (size_t i = 0; i < _storage.size(); ++i)
    {
        uint8 value = _storage[i];
        switch (value)
        {
        case 0:
            _storage[i] = 255;
            break;
        case 253:
        case 254:
            _storage.insert(_storage.begin() + (++i), value);
            break;
        case 255:
            {
                _storage[i++] = 253;
                _storage.insert(_storage.begin() + i, 254);
            }
            break;
        }
    }
    lua_pushlstring(L, (const char*)&_storage[0], _storage.size());
    return 1;
}*/

QString CQTui::strFilter(QString strData, int begin, int lenth)
{
    string str = strData.toStdString();
    if (lenth != 0){
        string ret(str.begin()+begin, str.begin()+begin+lenth);
        str = ret;
    }

    std::vector<uint8> _storage;
    _storage.reserve(str.size());
    _storage.resize(str.size());
    memcpy(&_storage[0], str.c_str(), str.size());
    for (size_t i = 0; i < _storage.size();++i)
    {
        uint8 value = _storage[i];
        if (value == 0)
        {
            _storage.erase(_storage.begin() + i, _storage.end());
        }
        else
        {

            switch (value)
            {
            case 255:
                _storage[i] = 0;
                break;
            case 253:
            case 254:
                {
                    if (_storage[i] == 253 && _storage[i+1] == 254) _storage[i] = 255;
                    _storage.erase(_storage.begin() + i + 1);
                }
                break;
            }
        }
    }

    string ret;
    ret.append((const char*)&_storage[0], _storage.size());
    return QString::fromStdString(ret);;
}
