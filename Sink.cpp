#include "Sink.h"
#include "Json.h"

// ireader libs include
#include "rtp-payload.h"
#include "rtp-profile.h"
#include "rtp.h"
#include <QDebug>

static void *pEncoder;
static struct rtp_payload_t handler;

static void * rtp_alloc(void *param, int bytes)
{
    static char buf[2*1024*1024 + 4];
    buf[0] = buf[1] = buf[2] = 0;
    buf[3] = 1;
    return buf + 4;
}

static void rtp_free(void *param, void *packet)
{

}

static void rtp_encode_packet(void *param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
    Sink *sik = (Sink *)param;
    sik->socket.writeDatagram((char *)packet,bytes,sik->ip,sik->dataPort);
    //qDebug()<<"[RTP]\tbytes:"<<bytes;
}

Sink::Sink(QObject *parent) : HinkObject(parent),socket(this)
{
    data["memSize"]=4096000;//4M
    data["dataPort"]=5000;
    data["infoPort"]=5001;
    data["payload"] = 96;
    data["payloadType"] = "H264";// "H265"
    dataPort=5000;
    infoPort=5001;
}

void Sink::onStart()
{
    socket.bind();
    rtp_packet_setsize(1456);
    handler.alloc = rtp_alloc;
    handler.free = rtp_free;
    handler.packet = rtp_encode_packet;
    pEncoder = rtp_payload_encode_create(data["payload"].toInt(),data["payloadType"].toString().toStdString().c_str(),33445,11293,&handler,this);
    if(pEncoder == NULL){
        qDebug()<<"Rtp encoder created error.";
    }else{
        qDebug()<<__FILE__<<"Rtp encoder created ok.";
    }
}

void Sink::onStop()
{
    socket.close();
}

void Sink::onSetData(QVariantMap newData)
{
    dataPort=data["dataPort"].toInt();
    infoPort=data["infoPort"].toInt();
    ip.setAddress(data["ip"].toString());
    if(!newData.isEmpty() && infoSrcV.type!=StreamInfo::UnKnow)
        onNewInfoV(infoSelfV);
}

void Sink::onNewInfoV(StreamInfo info)
{
    infoSrcV=info;

  //  if(!ip.isNull())
  //      socket.writeDatagram(Json::encode(info.info).toLatin1(),ip,infoPort);
}

void Sink::onNewPacketV(HinkObject::Packet pkt)
{
    if(ip.isNull())
        return;

    //char *buf=bufferData(NULL,pkt.len+sizeof(PktHeader));
    //PktHeader header;
    //header.pts=pkt.pts;
    //header.dts=pkt.dts;
    //header.isIframe=(pkt.type==Packet::IFrame);
    //memcpy(buf,&header,sizeof(PktHeader));
    //memcpy(buf+sizeof(PktHeader),pkt.data,pkt.len);
    rtp_payload_encode_input(pEncoder,pkt.data,pkt.len,pkt.pts);
    //qDebug()<<tr("pkt data = %1").arg(pkt.data[5]);
    //socket.writeDatagram(buf,pkt.len+sizeof(PktHeader),ip,dataPort);
}

