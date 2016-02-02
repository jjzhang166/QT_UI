#include "qt_ui.h"
#include "ByteBuffer.h"
#include "pbc.h"
#include "pbcutil.h"

CQTui::CQTui(QObject *parent):QObject(parent)
{

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
                            QString Password, QString Dbname, QString strCharset )
{
    string name     = Name.toStdString();
    string host     = Host.toStdString();
    string user     = User.toStdString();
    string password = Password.toStdString();
    string dbname   = Dbname.toStdString();

    MYSQL* conn;
    conn = mysql_init((MYSQL*)0);
    if (strCharset.length() != 0){
        ::mysql_options(conn, MYSQL_SET_CHARSET_NAME, strCharset.toStdString().c_str());
    }
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

static bool doPutFileContent(QString fileName, QString content, QString mode)
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
bool CQTui::putFileContent(QString fileName, QString content, QString mode)
{
    return doPutFileContent(fileName, content, mode);
}
static string strFilterReal(const string& str){
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
    return ret;
}

static string toLatin1(const QString& foo){
    QByteArray foo2 = foo.toLatin1();
    string foo3(foo2.constData(), foo2.length());
    return foo3;
}
QString CQTui::strFilter(QString strData, int begin, int lenth)
{
    string str = toLatin1(strData);//strData.toStdString();
    if (lenth != 0){
        string ret(str.begin()+begin, str.begin()+begin+lenth);
        str = ret;
    }

    string ret = strFilterReal(str);
    return QString::fromStdString(ret);;
}

 QString CQTui::strEncode(QString strData1)
 {
     string strData = toLatin1(strData1);//.toStdString();
     const char* pInValue = strData.c_str();
     int nInLen = strData.size();

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
     string ret;
     ret.append((const char*)&_storage[0], _storage.size());
     return QString::fromStdString(ret);
}

 static QVariantMap decodePbc2Js(const string& strProp, const string& message, const string& strData)
 {
     QVariantMap mapValue;
     struct pbc_slice pb;
     struct pbc_env * env = pbc_new();

     read_file(strProp.c_str(), &pb);

     int r = pbc_register(env, &pb);
     if (r!=0) {
         fprintf(stderr, "Can't register %s\n", strProp.c_str());
         return mapValue;
     }

     /*
     struct pbc_wmessage *pbuff = pbc_wmessage_new(env, message.c_str());
     pbc_slice slice;
     pbc_wmessage_buffer(pbuff, &slice);
     strData.assign((const char*)slice.buffer, slice.len);
     pbc_wmessage_delete(pbuff);
     */

     struct pbc_slice data;
     data.len    = strData.size();
     data.buffer = (void*)(strData.c_str());

     struct pbc_rmessage * m = pbc_rmessage_new(env, message.c_str(), &data);
     if (m == NULL) {
         fprintf(stderr, "Decoding message '%s' failed\n", message.c_str());
         return mapValue;
     }
     dump_message(m,0, mapValue);

     return mapValue;
 }

QVariantMap CQTui::pbcToJs(QString protoData, QString strmessage, QString strData1)
{
    QVariantMap mapValue;
    string strProp = protoData.toStdString();
    string message = strmessage.toStdString();
    string strData = toLatin1(strData1);

    QString foo = QString::fromStdString(strData);

    mapValue = decodePbc2Js(strProp, message, foo.toStdString());
    return mapValue;
}


QVariantMap CQTui::queryMysql(QString Name, QString Sql)
{
    QByteArray bt = Sql.toUtf8();
    string name         = Name.toStdString();
    string sql(bt.constData(), bt.length());//Sql.toStdString();
    QVariant errmsg     = QString::fromStdString("");
    QVariant affectRows = 0;
    QVariantMap mapValue;
    QList<QVariant> resultList;
    QStringList colNameList;

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

            if (result)
            {
                MYSQL_FIELD* column_infos = ::mysql_fetch_fields(result);
                int fieldNum = mysql_num_fields(result);
                for (int c = 0; c < fieldNum; ++ c)
                {
                    colNameList << column_infos[c].name;
                }
                while (!!(sql_row = mysql_fetch_row(result)))
                {
                    unsigned long *lengths = mysql_fetch_lengths(result);
                    QStringList tmp;
                    for (int i = 0; i < fieldNum; ++i)
                    {
                        string data;
                        data.append(sql_row[i], lengths[i]);;
                        QString fooQ4 = QString::fromStdString(data);
                        tmp << fooQ4;

                    }
                    resultList << QVariant(tmp);
                }
                mysql_free_result(result);
            }
            else
            {
                //errmsg = mysql_error(conn);
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
    mapValue["fieldNames"] = QVariant(colNameList);;

    return mapValue;
}


QString CQTui::charestDecode(QString strData)
{
    QByteArray bt = strData.toLatin1();
    string s(bt.constData(), bt.length());
    QString ret = QString::fromUtf8(bt.constData(), bt.length());
    return ret;
}

static void jsToPbcMemsage(struct pbc_wmessage *pbuff, QVariantMap& srcMap)
{
    for (QVariantMap::iterator it = srcMap.begin(); it != srcMap.end(); ++it){
        const string& strKey   = it.key().toStdString();
        QVariant& varVal       = it.value();
        if (varVal.type() != QVariant::List){
            continue;
        }
        QList<QVariant> tmpList = varVal.toList();
        if (tmpList.size() != 2)
            continue;
        string pbcVal;
        if (tmpList[0].type() == QVariant::String)
        {
            pbcVal = tmpList[0].toString().toStdString();
        }
        if (tmpList[1].type() != QVariant::String)
            continue;

        string pbcType= tmpList[1].toString().toStdString();
        char buff[512] = {0};
        sprintf(buff, "pbcKey=%s,pbcVal=%s,pbcType=%s\n", strKey.c_str(), pbcVal.c_str(), pbcType.c_str());
        doPutFileContent("test.txt", buff, "a");

        if (pbcType == "int"){
            pbc_wmessage_integer(pbuff, strKey.c_str(), ::atoi(pbcVal.c_str()), 0);
        }
        else if (pbcType == "string"){
            pbc_wmessage_string(pbuff, strKey.c_str(), pbcVal.c_str(), pbcVal.size());
        }
        else if (pbcType == "real"){
            pbc_wmessage_real(pbuff, strKey.c_str(), ::atof(pbcVal.c_str()));
        }
        else if (pbcType == "bool"){
            pbc_wmessage_string(pbuff, strKey.c_str(), pbcVal.c_str(), pbcVal.size());
        }
        else if (pbcType == "repeated"){
            QVariantMap repeatedMap = tmpList[0].toMap();

            for (QVariantMap::iterator it2 = repeatedMap.begin(); it2 != repeatedMap.end(); ++it2){
                QList<QVariant> childVal         = it2.value().toList();
                if (childVal.size() != 2)
                    continue;
                string childType = childVal[1].toString().toStdString();
                if (childType == "message"){
                    pbc_wmessage* pbs = pbc_wmessage_message(pbuff , strKey.c_str());
                    QVariantMap childMessage = childVal[0].toMap();
                    jsToPbcMemsage(pbs, childMessage);
                }
            }
        }
    }
}

QVariantMap CQTui::jsToPbc(QString protoFile, QString strMessName, QVariantMap srcMap)
{
    QVariantMap retMap;
    QString ret;
    string strData;
    struct pbc_slice pb;
    struct pbc_env * env = pbc_new();
    string strProp = protoFile.toStdString();
    string message = strMessName.toStdString();

    read_file(strProp.c_str(), &pb);

    int r = pbc_register(env, &pb);
    if (r!=0) {
        fprintf(stderr, "Can't register %s\n", strProp.c_str());
        return retMap;
    }

    struct pbc_wmessage *pbuff = pbc_wmessage_new(env, message.c_str());
    /*
    for (QVariantMap::iterator it = srcMap.begin(); it != srcMap.end(); ++it){
        const string& strKey   = it.key().toStdString();
        QVariant& varVal       = it.value();
        if (varVal.type() != QVariant::List){
            continue;
        }
        QList<QVariant> tmpList = varVal.toList();
        if (tmpList.size() != 2)
            continue;
        if (tmpList[0].type() != QVariant::String)
            continue;
        if (tmpList[1].type() != QVariant::String)
            continue;
        string pbcVal = tmpList[0].toString().toStdString();
        string pbcType= tmpList[1].toString().toStdString();
        printf("pbcKey=%s,pbcVal=%s,pbcType=%s\n", strKey.c_str(), pbcVal.c_str(), pbcType.c_str());

        if (pbcType == "int"){
            pbc_wmessage_integer(pbuff, strKey.c_str(), ::atoi(pbcVal.c_str()), 0);
        }
        else if (pbcType == "string"){
            pbc_wmessage_string(pbuff, strKey.c_str(), pbcVal.c_str(), pbcVal.size());
        }
    }
    */
    jsToPbcMemsage(pbuff, srcMap);
    pbc_slice slice;
    pbc_wmessage_buffer(pbuff, &slice);
    strData.assign((const char*)slice.buffer, slice.len);
    pbc_wmessage_delete(pbuff);

    ret = QString::fromStdString(strData);
    retMap["data"] = ret;
    retMap["js"]   = pbcToJs(protoFile, strMessName, ret);
    return retMap;
}

