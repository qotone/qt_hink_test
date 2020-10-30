#include "HinkInputV4l2.h"
#include "Hink.h"
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



// for 4vl
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <malloc.h>



HinkInputV4l2::HinkInputV4l2(QObject *parent) : HinkObject(parent)
{

    data["path"] = "/dev/video0";
    data["format"] = "mjpeg";
    data["framerate"] = 30;
    data["width"] = 1920;
    data["height"] = 1080;
    data["v4l2BufLen"] = 4;
    data["memSize"] = data["width"].toUInt() * data["height"].toUInt() * 2;
    uvc = NULL;
    //uvcParam.width = data["width"].toInt();
    //uvcParam.height = data["height"].toInt();
    //uvcParam.interval = data["framerate"].toInt();



}

void HinkInputV4l2::onReadCamData()
{
    HinkObject::Packet pkt;
    int size = 0;
    char *buf = bufferData(NULL,uvcParam.width*uvcParam.height*2);
    size = uvc_read(uvc,(void *)buf,uvcParam.width*uvcParam.height*2);

    if(size == -1)
        return;

    pkt.data = buf;
    pkt.len = size;
    pkt.dts = 0;
    pkt.pts = 0;
    pkt.sender = this;
    //qDebug()<<tr("usbcam read %1 bytes.").arg(size);
    emit(newPacketV(pkt));

}

void HinkInputV4l2::onStart()
{

//    QString str = "test";

//    QThread *thread = Hink::newThread(str);
//    this->moveToThread(thread);
//    //connect(uvc->fd,SIGNAL(ready),this,SLOT(onReadCamData()));

//    thread->start();

    uvc = uvc_open(data["path"].toString().toStdString().c_str());
    if(uvc)
        qDebug()<<tr("open %1 successed.").arg(data["path"].toString());
    else
        return;


    uvcParam.width = data["width"].toInt();
    uvcParam.height = data["height"].toInt();
    uvcParam.interval = data["framerate"].toInt();

    uvc_ioctl(uvc,UVC_SET_PARAM,(void *)&uvcParam,0);

    memInit(data["memSize"].toUInt());
    qDebug()<<"memSize ="<<data["memSize"].toInt();

    this->notifier = new QSocketNotifier(uvc->fd,QSocketNotifier::Read,this);
    connect(this->notifier,SIGNAL(activated(int)),this,SLOT(onReadCamData()));

}

void HinkInputV4l2::onNewPacketV(HinkObject::Packet pkt)
{

}

void HinkInputV4l2::onSetData(QVariantMap data)
{



}
