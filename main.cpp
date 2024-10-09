#include "recognize_server.h"

#include <QApplication>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <opencv.hpp>

#include "RegiestWin.h"
#include "SelectWin.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //注册数据类型
    qRegisterMetaType<cv:: Mat>("cv::Mat&");
    qRegisterMetaType<cv:: Mat>("cv::Mat");
    qRegisterMetaType<int64_t>("int64_t");



    qDebug()<<"=========================";
    //连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据库名称
    db.setDatabaseName("server.db");
    //打开数据库
    if(!db.open())
    {
        qDebug()<<db.lastError().text();
        return -1;
    }
    //创建员工信息表格
    QString mysql = "create table if not exists userinfo(userid integer primary key autoincrement, "
                    "name text, sex text, birthday text, address text, "
                    "phone text, faceid integer unique, headfile text)";
    QSqlQuery query;

    if(!query.exec(mysql))
    {
        qDebug()<<"用户信息表"<<query.lastError().text();
        return -1;
    }

    //创建考勤表   datetime('now', localtime)获取本地时间
    mysql = "create table if not exists attendance(attendanceid integer primary key autoincrement, "
            "userid integer, time TimeStamp not null default CURRENT_TIMESTAMP)";
//    mysql = "create table if not exists attendance(attendanceid integer primary key autoincrement, "
//                        "userid integer, time TimeStamp not null default(datetime('now', localtime)))";
    if(!query.exec(mysql))
    {
        qDebug()<<"用户考勤表"<<query.lastError().text();
        return -1;
    }

    qDebug()<<"1111111111111111";


    recognize_server w;
    w.show();

//    RegiestWin ww;
//    ww.show();

//    SelectWin www;
//    www.show();

    return a.exec();
}
