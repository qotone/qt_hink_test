#include "HinkInputAlsa.h"
#include <QDebug>
#include <QDateTime>

HinkInputAlsa::HinkInputAlsa(QObject *parent):HinkObject(parent)
{

    m_audioFmtList.push_back(SND_PCM_FORMAT_S16_LE);
    ALSACaptureParameters param("hw:0,0",m_audioFmtList,16000,1,1);
    m_capture = ALSACapture::createNew(param);
    m_pkt = 0;
    m_time = 0;
}

HinkInputAlsa::~HinkInputAlsa()
{
    disconnect(notifier,SIGNAL(activated(int)),this,SLOT(onReadCamData()));
    delete m_capture;

    delete notifier;

}

void HinkInputAlsa::onStart()
{

    int fd = -1;
    fd = m_capture->getFd();
    this->notifier = new QSocketNotifier(fd,QSocketNotifier::Read,this);
    connect(this->notifier,SIGNAL(activated(int)),this,SLOT(onReadCamData()));
}

void HinkInputAlsa::onReadCamData()
{
    HinkObject::Packet pkt;
    int size = 0;
    char buf[m_capture->getBufferSize()] ;
    size = m_capture->read(buf,m_capture->getBufferSize());
    qint64 timeT = QDateTime::currentMSecsSinceEpoch();   //获取当前时间

    //if(m_time == 0){
    //    m_time = timeT;
    //}

    if(size < 0){
        qDebug()<<"HinkInputAlsa onReadCamData failed!";
        return;
    }

    m_pkt += 16*(timeT - m_time);//320;//size;
    pkt.data = buf;
    pkt.len = size;
    pkt.dts = 0;
    pkt.pts = m_pkt;
    pkt.sender = this;
    //qDebug()<<tr("usbcam read %1 bytes.").arg(size);
    qDebug()<<tr("timestamp interval %1 ").arg(timeT - m_time);
    emit(newPacketA(pkt));
    m_time = timeT;

}

