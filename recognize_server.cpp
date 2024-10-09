#include "recognize_server.h"
#include "ui_recognize_server.h"

#include <QJsonObject>
#include <QJsonDocument>

#include <QSqlQuery>

#include <QDateTime>
#include <QThread>


recognize_server::recognize_server(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::recognize_server)
{
    ui->setupUi(this);

    //当有新的客户端连接时会产生 newconnection信号
    connect(&myserver,&QTcpServer::newConnection,this,&recognize_server::accept_client);
    myserver.listen(QHostAddress::Any,50000);//监听，启动服务器

    bsize = 0;

    model.setTable("userinfo");//绑定表

    //创建线程
    QThread *thread = new QThread();
    //把qfaceobj对象移动到线程中执行
    faceobj.moveToThread(thread);
    //启动线程
    thread->start();
    connect(this,&recognize_server::query,&faceobj,&QfaceObject::faceQuery);
    connect(&faceobj,&QfaceObject::send_faceid,this,&recognize_server::recv_faceid);

    // 创建QTimer用于定时更新时间
    timer = new QTimer(this);

    // 连接QTimer的timeout()信号到槽函数updateTime()
    connect(timer, &QTimer::timeout, this, &recognize_server::updateTime);

    // 设置QTimer的间隔时间为1000毫秒（1秒）
    timer->start(1000);

}

recognize_server::~recognize_server()
{
    delete ui;
}


void recognize_server::accept_client()
{
    //获取通信套接字
    mysocket = myserver.nextPendingConnection();

    connect(mysocket,&QTcpSocket::readyRead,this,&recognize_server::read_data);
}

void recognize_server::read_data()
{
    QDataStream stream(mysocket);//把套接字绑定到数据流
    stream.setVersion(QDataStream::Qt_5_14);

    if (bsize == 0)
    {
        if (mysocket->bytesAvailable() < (quint64)sizeof(bsize))
        {
            return;
        }
        //采集数据的长度
        stream>>bsize;
    }
    if (mysocket->bytesAvailable() < bsize)
    {
        return;
    }

    QByteArray data;
    stream>>data;
    bsize = 0;
    if(data.size() == 0)
    {
        return;
    }

    //显示图片
    QPixmap mmp;
    mmp.loadFromData(data,".jpg");
    mmp = mmp.scaled(ui->label->size());
    ui->label->setPixmap(mmp);

    //人脸识别
    cv::Mat faceImage;
    std::vector<uchar> decode;
    decode.resize(data.size());
    memcpy(decode.data(),data.data(),data.size());

    faceImage = cv::imdecode(decode, cv::IMREAD_COLOR);

    //int faceid = faceobj.faceQuery(faceImage);//转到线程中执行
    emit query(faceImage);

}

//槽函数
void recognize_server::recv_faceid(int64_t faceid)
{
    qDebug()<<"=====faceid="<<faceid<<"============";
    //从数据库中查询faceid对应的个人信息
    //给数据库模型设置过滤器
    if (faceid < 0)
    {
        QJsonObject jsonObj;
        jsonObj["id"] = "-1";
        jsonObj["name"] = " ";
        jsonObj["work"] = " ";
        jsonObj["time"] = " ";

        QJsonDocument jsonDoc(jsonObj);
        // 转换为字符串
        QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

        mysocket->write(jsonString.toUtf8());
        return;
    }

    model.setFilter(QString( "faceid = %1").arg(faceid));

    //查询
    model.select();

    //判断查询结果
    if (model.rowCount() == 1)
    {
        //工号，姓名，部门，时间 给客户端发回去
        QSqlRecord record = model.record(0);//查到的一行数据

        /**********************调试***********************/
        // 创建一个 JSON 对象
        QJsonObject jsonObj;
        jsonObj["id"] = record.value("userid").toString();
        jsonObj["name"] = record.value("name").toString();
        jsonObj["work"] = "程序猿";
        jsonObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

        QJsonDocument jsonDoc(jsonObj);
        // 转换为字符串
        QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);
        // 输出 JSON 字符串
        qDebug() << jsonString;



        qDebug() <<"写入考勤数据库";
        //写入 考勤表数据库
        QString insertSql = QString("insert into attendance(userid) values('%1')").arg(record.value("userid").toString());
        QSqlQuery query;
        if (!query.exec(insertSql))
        {
            qDebug()<<"考勤失败";
            QJsonObject jsonObj;
            jsonObj["id"] = "-1";
            jsonObj["name"] = " ";
            jsonObj["work"] = " ";
            jsonObj["time"] = " ";

            QJsonDocument jsonDoc(jsonObj);
            // 转换为字符串
            QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

            mysocket->write(jsonString.toUtf8());
        }
        else
        {
            qDebug()<<"考勤成功";
            mysocket->write(jsonString.toUtf8());
        }
    }
}

//实时显示时间
void recognize_server::updateTime()
{
        // 获取当前时间并格式化为字符串
        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        // 设置QLabel的文本为当前时间
        ui->now_time->setText(currentTime);
}
