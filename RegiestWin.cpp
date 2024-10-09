#include "RegiestWin.h"
#include "ui_RegiestWin.h"
#include <QFileDialog>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMessageBox>
#include <QDebug>


RegiestWin::RegiestWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegiestWin)
{
    ui->setupUi(this);
//    ui->birthdaydateEdit->setDate(QDate::currentDate());//更新时间至当前日期
}

RegiestWin::~RegiestWin()
{
    delete ui;
}

//定时器
void RegiestWin::timerEvent(QTimerEvent *)
{
    //获取摄像头数据，显示在界面上

    if (cap.isOpened())
    {
        cap>>image;
        if(image.data == nullptr)
        {
             return;
        }
    }


    cv::cvtColor(image,image, cv::COLOR_BGR2RGB);
    QImage qimage(image.data, image.cols, image.rows, image.step1(), QImage::Format_BGR888);
    QPixmap mmp = QPixmap::fromImage(qimage);
    mmp.scaledToHeight(ui->headpiclabel->width());
    ui->headpiclabel->setPixmap(mmp);
}

void RegiestWin::on_resetpushBt_clicked()
{
    ui->namelineEdit->clear();
    ui->birthdaydateEdit->setDate(QDate::currentDate());
    ui->addresslineEdit->clear();
    ui->phonelineEdit->clear();
    ui->picfileEdit->clear();
}

void RegiestWin::on_addipgBt_clicked()
{
    //弹一个文件对话框
    QString filepath = QFileDialog::getOpenFileName(this);
    ui->picfileEdit->setText(filepath);

    //显示图片
    QPixmap mmp(filepath);
    mmp = mmp.scaledToHeight(ui->headpiclabel->height());//按照宽度缩放
    ui->headpiclabel->setPixmap(mmp);
}

void RegiestWin::on_regiestpushBt_clicked()
{
    //1.通过照片，结合faceobject模块得到faceid
    QfaceObject faceobj;
    cv::Mat image = cv::imread(ui->picfileEdit->text().toUtf8().data());
    int faceid = faceobj.faceRegist(image);
    qDebug()<<"faceid:"<<faceid;

    //把头像保存在一个固定的路径下
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->namelineEdit->text().toUtf8().toBase64()));//数据 转换
    cv::imwrite(headfile.toUtf8().data(), image);//写入文件

    //2.把个人信息存入数据库
    QSqlTableModel model;
    model.setTable("userinfo");
    QSqlRecord record = model.record();
    record.setValue("name",ui->namelineEdit->text());
    record.setValue("sex",ui->manradioBt->isChecked()?"男":"女");
    record.setValue("birthday",ui->birthdaydateEdit->text());
    record.setValue("address",ui->addresslineEdit->text());
    record.setValue("phone",ui->phonelineEdit->text());
    record.setValue("faceid",faceid);
    //头像路径
    record.setValue("headfile",headfile);

    bool ret = model.insertRecord(0, record);
    if (ret)
    {
        QMessageBox::information(this,"注册提示","注册成功");
        //提交，写入数据库
        model.submitAll();
    }
    else
    {
        QMessageBox::critical(this,"注册提示","注册失败");
    }
}

void RegiestWin::on_videoSwitch_clicked()
{
    if (ui->videoSwitch->text() == "打开摄像头")
    {
        //打开摄像头
        if (cap.open(0))
        {
            ui->videoSwitch->setText("关闭摄像头");
            timerid = startTimer(100);
        }
    }
    else
    {
        killTimer(timerid);
        ui->videoSwitch->setText("打开摄像头");
        cap.release();//释放对象  关闭摄像头
    }
}

void RegiestWin::on_cameraBt_clicked()
{
    //保存数据
    QString headfile = QString("./data/%1.jpg").arg(QString(ui->namelineEdit->text().toUtf8().toBase64()));//数据 转换
    ui->picfileEdit->setText(headfile);
    cv::imwrite(headfile.toUtf8().data(), image);//写入文件

    killTimer(timerid);
    ui->videoSwitch->setText("打开摄像头");
    cap.release();//释放对象  关闭摄像头
}
