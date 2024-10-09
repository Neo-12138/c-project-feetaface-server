#ifndef RECOGNIZE_SERVER_H
#define RECOGNIZE_SERVER_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

#include "QfaceObject.h"

//数据库模型头文件
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QTimer>





QT_BEGIN_NAMESPACE
namespace Ui { class recognize_server; }
QT_END_NAMESPACE

class recognize_server : public QMainWindow
{
    Q_OBJECT

public:
    recognize_server(QWidget *parent = nullptr);
    ~recognize_server();

signals:
    void query(cv::Mat &image);//cv::Mat &faceimage


protected slots:
    void accept_client();
    void read_data();
    void recv_faceid(int64_t faceid);

    void updateTime();


private:
    Ui::recognize_server *ui;

    QTcpServer myserver;

    QTcpSocket *mysocket;

    quint64 bsize;

    QfaceObject faceobj;

    QSqlTableModel model;

    QTimer *timer;
};
#endif // RECOGNIZE_SERVER_H
