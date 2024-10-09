#include "QfaceObject.h"
#include <QDebug>

#define FD_PATH "C:/opencv-4.5.2/opencv-SeetaFace-qt5.14.0minGW/SeetaFace/bin/model/fd_2_00.dat"
#define PD_PATH "C:/opencv-4.5.2/opencv-SeetaFace-qt5.14.0minGW/SeetaFace/bin/model/pd_2_00_pts5.dat"
#define FR_PATH "C:/opencv-4.5.2/opencv-SeetaFace-qt5.14.0minGW/SeetaFace/bin/model/fr_2_10.dat"


QfaceObject::QfaceObject(QObject *parent) : QObject(parent)
{
    //初始化 引擎
    seeta::ModelSetting FDmode(FD_PATH,seeta::ModelSetting::CPU,0);
    seeta::ModelSetting PDmode(PD_PATH,seeta::ModelSetting::CPU,0);
    seeta::ModelSetting FRmode(FR_PATH,seeta::ModelSetting::CPU,0);

    //seeta::ModelSetting FDmode("C:/opencv-4.5.2/opencv-SeetaFace-qt5.14.0minGW/SeetaFace/bin/model/fd_2_00.dat",seeta::ModelSetting::CPU,0);
    //seeta::ModelSetting PDmode("C:/opencv-4.5.2/opencv-SeetaFace-qt5.14.0minGW/SeetaFace/bin/model/pd_2_00_pts5.dat",seeta::ModelSetting::CPU,0);
    //seeta::ModelSetting FRmode("C:/opencv-4.5.2/opencv-SeetaFace-qt5.14.0minGW/SeetaFace/bin/model/fr_2_10.dat",seeta::ModelSetting::CPU,0);

    this->fptr = new seeta::FaceEngine(FDmode,PDmode,FRmode);

    //导入已有的人脸数据库
    this->fptr->Load("./face.db");
}

QfaceObject::~QfaceObject()
{
    delete fptr;
}

int64_t QfaceObject::faceRegist(cv::Mat &faceimage)
{
    //把opencv的数据转为seetaface数据
    SeetaImageData image;
    image.data = faceimage.data;
    image.width = faceimage.cols;
    image.height = faceimage.rows;
    image.channels = faceimage.channels();

    int64_t faceid =  this->fptr->Register(image);//注册返回一个人脸id

    if (faceid >= 0)
    {
        fptr->Save("./face.db");
    }
    return faceid;
}

int QfaceObject::faceQuery(cv::Mat &faceimage)//查询
{
     //把opencv的数据转为seetaface数据
    SeetaImageData image;
    image.data = faceimage.data;
    image.width = faceimage.cols;
    image.height = faceimage.rows;
    image.channels = faceimage.channels();

    float similarity = 0;
    int64_t faceid =  fptr->Query(image,&similarity);//运算时间比较长
    qDebug()<<"查询="<<faceid<<"   "<<similarity;
    if (similarity > 0.6)//相似度大于0.7
    {
        emit send_faceid(faceid);
    }
    else
    {
        emit send_faceid(-1);
    }
//    return faceid;
}
