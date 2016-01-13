#include "qtwebkit.h"
//#include "json.h"
#include <QtGui>
#include <QtCore>
#include <string>
#include <QDebug>
#include <stdio.h>
#include <string>
#include <iostream>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlDriver>
#include <QSqlRecord>
#include <QMap>
#include <QFile>

QtWebkit::QtWebkit(QObject *parent):QObject(parent)
{
}

QtWebkit::~QtWebkit()
{
}

bool QtWebkit::getData(QString qufu, QString sql, QString flag)
{
    CONFIG cf;
    QString isDura = flag.left(1);
    if (!config.contains(qufu))
    {
        QMessageBox::warning(0, "WARNING", "不存在此区服，请重新选择");
        return false;
    }

    cf = config[qufu];
    QString hostname = cf.localhost;
    QString username = cf.username;
    QString password = cf.password;
    QString database = "guild";
    QString port     = cf.port;
    char value       = 0;
    MYSQL conn;
    MYSQL_RES *result;
    MYSQL_ROW sql_row;
    int res;
    m_results.clear();
    std::vector<RESULT> vec_results;

    mysql_init(&conn);
    if (!mysql_real_connect(&conn, hostname.toStdString().c_str(), username.toStdString().c_str(), password.toStdString().c_str(),database.toStdString().c_str(), atoi(port.toStdString().c_str()), NULL, 0))
    {
        mysql_close(&conn);
        QMessageBox::warning(0, "WARNING", "connect error:mysql_real_connect");
        return false;
    }
    mysql_options(&conn, MYSQL_OPT_RECONNECT, &value);

    mysql_query(&conn, "SET NAMES latin1");
    res = mysql_query(&conn, sql.toStdString().c_str());

    if (!res)
    {
        result = mysql_store_result(&conn);
        if (result)
        {
            while (!!(sql_row = mysql_fetch_row(result)))
            {
                RESULT fd;
                std::string item  = sql_row[3];
                std::string count = sql_row[4];
                std::string time  = sql_row[8];

                std::vector<std::string> vec_item   = split(item, ",");
                std::vector<std::string> vec_count = split(count, ",");
                if (vec_item.size() == vec_count.size() && vec_item.size() > 0)
                {
                    if (vec_item.size() > 1)
                    {
                        bool flag = false;
                        for (std::vector<std::string>::size_type m = 1; m < vec_item.size(); ++m)
                        {
                            if (vec_item[0] != vec_item[m])
                            {
                                flag = true;
                                break;
                            }
                        }

                        if (flag)
                            continue;
                    }

                    unsigned int count = 0;
                    if (isDura == "1")
                    {
                        for (std::vector<std::string>::size_type n = 0; n < vec_count.size(); ++n)
                        {
                            count += atoi(vec_count[n].c_str());
                        }
                    } else if (isDura == "0") {
                        count = vec_item.size();
                    }


                    fd.itemlist    = vec_item[0];
                    fd.countlist   = count;
                    fd.yuanbaolist = atoi(sql_row[5] );
                    fd.time        = time.substr(0, 10);

                    vec_results.push_back(fd);
                }//end if
            }//end while

            bool flag = true;
            for (std::vector<RESULT>::size_type i = 0; i < vec_results.size(); ++i)
            {
                for (std::vector<RESULT>::size_type j = 0; j < m_results.size(); ++j)
                {
                    if (vec_results[i].itemlist == m_results[j].itemlist && vec_results[i].time == m_results[j].time)
                    {
                        m_results[j].countlist += vec_results[i].countlist;
                        m_results[j].yuanbaolist += vec_results[i].yuanbaolist;
                        flag = false;
                    }
                }

                if (flag)
                {
                    m_results.push_back(vec_results[i]);
                }
                else
                    flag = true;
            }
        }//end if
    }//end if
    else
    {
        QMessageBox::warning(0, "WARNING", "query sql failed!");
        mysql_close(&conn);
        return false;
    }

    if (result != NULL)
        mysql_free_result(result);

    mysql_close(&conn);
    return true;
}

QVariantList QtWebkit::convertToJson()
{
    QVariantList list;
    for (std::vector<RESULT>::size_type i = 0; i < m_results.size(); ++i)
    {
        QVariantMap map;
        RESULT res     = m_results[i];
        map["item"]    = QString::fromStdString(res.itemlist);
        map["count"]   = res.countlist;
        map["yuanbao"] = res.yuanbaolist;
        map["time"]    = QString::fromStdString(res.time);

        list << map;
    }
    return list;
}

std::vector<std::string> QtWebkit::split(std::string str, std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str += pattern;
    std::string::size_type size = str.size();

    for (std::string::size_type i = 0; i < size; ++i)
    {
        pos = str.find(pattern, i);
        if (pos < size)
        {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

bool QtWebkit::getConfig()
{
    QFile *file = new QFile("./config/config.txt");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(0, tr("Warning"), tr("can not load config file"));
        return false;
    }

    QTextStream in(file);
    config.clear();

    while (!file->atEnd())
    {
        QString data = file->readLine();
        QStringList list = data.split(",");
        if (list.size() >= 6)
        {
            CONFIG cf;
            cf.localhost = list[2];
            cf.username  = list[3];
            cf.password  = list[4];
            cf.port      = list[5];

            config[list[0]+list[1]] = cf;
        }
    }
    file->close();
    return true;
}

QVariantList QtWebkit::getItemList()
{
    QFile *file = new QFile("./config/itemconfig.txt");
    QVariantList vlist;
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(0, tr("Warning"), tr("can not load itemconfig file"));
        return vlist;
    }

    QTextStream in(file);

    while (!file->atEnd())
    {
        QString data = file->readLine();
        QStringList list = data.split(",");
        if (list.size() >= 4)
        {
            QString catagory1 = list[0];
            QString catagory2 = list[1];
            QString itemname  = list[2];
            QString isDura    = list[3];
           /* if (!m_itemlist.contains(catagory1))
            {
                QVariantMap m_item;
                QVariantMap m_subitem;

                m_item[itemname]      = isDura;
                m_subitem[catagory2]  = m_item;
                m_itemlist[catagory1] = m_subitem;
            } else {
                QVariantMap m_subitem;
                QVariantMap m_item;
                m_subitem = m_itemlist[catagory1].toMap();

                if (!m_subitem.contains(catagory2))
                {
                    m_item[itemname]     = isDura;
                    m_subitem[catagory2] = m_item;
                } else {
                    m_item = m_subitem[catagory2].toMap();
                    m_item[itemname] = isDura;
                }
            }*/
                QVariantMap map;
                map["catagory1"] = catagory1;
                map["catagory2"] = catagory2;
                map["itemname"]  = itemname;
                map["isDura"]    = isDura;
                vlist << map;
        }
    }

  //  list << m_itemlist;
    file->close();
    return vlist;
}

QVariantList QtWebkit::getQuFu()
{
    QMap<QString, CONFIG>::iterator iter;
    QVariantList list;

    for (iter = config.begin(); iter != config.end(); ++iter)
    {
        list << iter.key();
    }
    return list;
}

void QtWebkit::getItem(qint8 flag)
{
    qDebug() << flag;
}

