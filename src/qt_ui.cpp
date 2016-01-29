#include "qt_ui.h"
#include "ByteBuffer.h"
#include "pbc.h"

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
     string strData = strData1.toStdString();
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

 static void
 dump_bytes(const char *data, size_t len) {
     size_t i;
     for (i = 0; i < len; i++)
         if (i == 0)
             fprintf(stdout, "%02x", 0xff & data[i]);
         else
             fprintf(stdout, " %02x", 0xff & data[i]);
 }

 static void dump_message(struct pbc_rmessage *m, int level, QVariantMap& mapValue);

 static void
 dump_value(struct pbc_rmessage *m, const char *key, int type, int idx, int level,
            QVariantMap& mapValue, QList<QVariant>* tmpChild) {
     int i;
     for (i=0;i<level;i++) {
         printf("  ");
     }
     printf("%s",key);
     int srcType = type;
     if (type & PBC_REPEATED) {
         printf("[%d]",idx);
         type -= PBC_REPEATED;
     }
     printf(": ");

     uint32_t low;
     uint32_t hi;
     uint64_t ret64;
     double real;
     const char *str;
     int str_len;

     QVariant realVal;
     QVariantMap childMapV;

     switch(type) {
     case PBC_INT:
     case PBC_UINT:
         low = pbc_rmessage_integer(m, key, idx, NULL);
         printf("%d", (int) low);
         realVal = (int) low;
         break;
     case PBC_REAL:
         real = pbc_rmessage_real(m, key , idx);
         printf("%lf", real);
         realVal = real;
         break;
     case PBC_BOOL:
         low = pbc_rmessage_integer(m, key, idx, NULL);
         printf("%s", low ? "true" : "false");
         realVal = (bool)low;
         break;
     case PBC_ENUM:
         str = pbc_rmessage_string(m, key , idx , NULL);
         printf("[%s]", str);
         realVal = str;
         break;
     case PBC_STRING:
         str = pbc_rmessage_string(m, key , idx , NULL);
         printf("'%s'", str);
         realVal = str;
         break;
     case PBC_MESSAGE:
         printf("\n");
         dump_message(pbc_rmessage_message(m, key, idx),level+1, childMapV);
         realVal = childMapV;
         break;
     case PBC_FIXED64:
     case PBC_INT64:
         low = pbc_rmessage_integer(m, key, idx, &hi);
         printf("0x%8x%8x",hi,low);
         memcpy((char*)(&ret64), &low, sizeof(low));
         memcpy((char*)(&ret64) + 4, &hi, sizeof(hi));
         realVal = ret64;
         break;
     case PBC_FIXED32:
         low = pbc_rmessage_integer(m, key, idx, NULL);
         printf("0x%x",low);
         realVal = low;
         break;
     case PBC_BYTES:
         str = pbc_rmessage_string(m, key, idx, &str_len);
         dump_bytes(str, str_len);
         break;
     default:
         printf("unknown %d", type);
         break;
     }

     printf("\n");
     if (srcType & PBC_REPEATED) {
         if (tmpChild)
            *tmpChild << realVal;
     }
     else{
         mapValue[key] = realVal;
     }
 }

 static void
 dump_message(struct pbc_rmessage *m, int level, QVariantMap& mapValue) {
     int t = 0;
     const char *key = NULL;
     for (;;) {
         t = pbc_rmessage_next(m, &key);
         if (key == NULL)
             break;
         if (t & PBC_REPEATED) {
             int n = pbc_rmessage_size(m, key);
             int i;
             QList<QVariant> tmpChild;
             for (i=0;i<n;i++) {

                 dump_value(m, key , t , i , level, mapValue, &tmpChild);
             }
             mapValue[key]     = tmpChild;
         } else {
             dump_value(m, key , t , 0 , level, mapValue, NULL);
         }
     }
 }
 static void
 read_file(const char *filename , struct pbc_slice *slice) {
     FILE *f = fopen(filename, "rb");
     if (f == NULL) {
         fprintf(stderr, "Can't open file %s\n", filename);
         return;
     }
     fseek(f,0,SEEK_END);
     slice->len = ftell(f);
     fseek(f,0,SEEK_SET);
     slice->buffer = malloc(slice->len);
     fread(slice->buffer, 1 , slice->len , f);
     fclose(f);
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
    string strData = toLatin1(strData1);//strData1.toStdString();

    mapValue = decodePbc2Js(strProp, message, strData);
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
                        data.append(sql_row[i], lengths[i]);
                        QString foo = QString::fromLatin1(sql_row[i], lengths[i]);
                        string foo3 = toLatin1(foo);
                        if (foo3 == data){
                            printf("xxx!\n");
                        }
                        else{
                            printf("xxx2!\n");
                        }
                        tmp << foo;

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
