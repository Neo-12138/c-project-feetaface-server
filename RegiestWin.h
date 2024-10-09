#ifndef REGIESTWIN_H
#define REGIESTWIN_H

#include <QWidget>

#include "QfaceObject.h"
#include <opencv.hpp>


namespace Ui {
class RegiestWin;
}

class RegiestWin : public QWidget
{
    Q_OBJECT

public:
    explicit RegiestWin(QWidget *parent = nullptr);
    ~RegiestWin();

    void timerEvent(QTimerEvent *event);


private slots:
    void on_resetpushBt_clicked();

    void on_addipgBt_clicked();

    void on_regiestpushBt_clicked();

    void on_videoSwitch_clicked();

    void on_cameraBt_clicked();

private:
    Ui::RegiestWin *ui;

    int timerid;

    cv::VideoCapture cap;//抓取摄像头对象

    cv::Mat image;
};

#endif // REGIESTWIN_H
