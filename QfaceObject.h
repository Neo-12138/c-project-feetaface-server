#ifndef QFACEOBJECT_H
#define QFACEOBJECT_H

#include <QObject>
#include <seeta/FaceEngine.h>
#include <opencv.hpp>


class QfaceObject : public QObject
{
    Q_OBJECT
public:
    explicit QfaceObject(QObject *parent = nullptr);
    ~QfaceObject();

public slots:
    int64_t faceRegist(cv::Mat &faceimage);//注册数据
    int faceQuery(cv::Mat &faceimage);//数据库中对应的id号


signals:
    void send_faceid(int64_t faceid);

private:
    seeta::FaceEngine *fptr;

};

#endif // QFACEOBJECT_H
