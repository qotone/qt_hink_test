#include "Source.h"

// ireader libs include
#include "rtp-payload.h"
#include "rtp-profile.h"
#include "rtp.h"
#include <QDebug>

static void * pDecoder;
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

static void rtp_decode_packet(void *param, const void *packet, int bytes, uint32_t timestamp, int flags)
{
    Source *src = (Source *)param;
    HinkObject::Packet pkt;
    char *pbuf = new char[bytes + 4];

    pbuf[0] = pbuf[1] = pbuf[2] = 0;
    pbuf[3] = 1;
    memcpy(pbuf + 4, packet,bytes);
    pkt.data=pbuf;
    pkt.len=bytes + 4;
    pkt.pts=0;
    pkt.dts=0;
    //pkt.type=head.isIframe?Packet::IFrame:Packet::PFrame;

    //qDebug()<<"flag = ........"<<flags;
    src->sendPacket(pkt);
    //src->dataSocket.writeDatagram((char *)packet,bytes,sik->ip,sik->dataPort);
    //qDebug()<<"[RTP]\tbytes:"<<bytes<<"ip:"<<sik->ip<<"port:"<<sik->dataPort;
    delete [] pbuf;
}

void Source::sendPacket(Packet pkt)
{
    sendPacketV(pkt);
}
#if 0
void* Source::decode_thread()
{

    return NULL;
}
#endif
Source::Source(QObject *parent) : HinkObject(parent),dataSocket(this),infoSocket(this)
{
    size_t n;
    data["memSize"]=4096000;//4M
    /*ctx.socket = socket_udp_bind(NULL,9002);
    socket_getrecvbuf(ctx.socket,&n);
    socket_setrecvbuf(ctx.socket,512*1024);
    socket_getrecvbuf(ctx.socket,&n);
    ctx.profile = rtp_profile_find(RTP_PAYLOAD_H264);
    ctx.handler.alloc = NULL;
    ctx.handler.free =NULL;
    ctx.handler.packet = rtp_packet_thub;
    ctx.payload = rtp_payload_decode_create(RTP_PAYLOAD_H264,"H264",&ctx.handler,&ctx);

    ctx.rtp = rtp_create()*/

}

void Source::onStart()
{
    dataSocket.bind(8008);
    infoSocket.bind(5001);
    connect(&dataSocket,SIGNAL(readyRead()),this,SLOT(onReadData()));
    connect(&infoSocket,SIGNAL(readyRead()),this,SLOT(onReadInfo()));
    handler.alloc = NULL;//rtp_alloc;
    handler.free = NULL;//rtp_free;
    handler.packet = rtp_decode_packet;
    //pDecoder = rtp_payload_decode_create(96, "H264", &handler, this);
    pDecoder = rtp_payload_decode_create(110, "H265", &handler, this);
    if(pDecoder == NULL){
        qDebug()<<"Rtp encoder created error.";
    }else{
        qDebug()<<__FILE__<<"Rtp encoder created ok. payload type ="<<data["payloadType"].toString()<<"dest ip:"<<data["ip"].toString()<<"port ="<<data["dataPort"].toInt();
    }

}

void Source::onReadData()
{
    while(dataSocket.hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(dataSocket.pendingDatagramSize());
        //char *buf=bufferData(NULL,dataSocket.pendingDatagramSize());
        int len=dataSocket.readDatagram(datagram.data(),datagram.size());
        //qDebug()<<"read "<<datagram.size()<<" bytes.";

        char *buf=datagram.data();
        //int len=datagram.size();
        rtp_payload_decode_input(pDecoder, buf, len);
#if 0
        PktHeader head;
        memcpy(&head,buf,sizeof(PktHeader));
        buf+=sizeof(PktHeader);
        len-=sizeof(PktHeader);

        Packet pkt;
        pkt.data=buf;
        pkt.len=len;
        pkt.pts=head.pts;
        pkt.dts=head.pts;
        pkt.type=head.isIframe?Packet::IFrame:Packet::PFrame;

        sendPacketV(pkt);
#endif
    }
}

void Source::onReadInfo()
{
#if 0
    infoSelfV.type=StreamInfo::H264;
    QByteArray ba;
    ba.resize(infoSocket.pendingDatagramSize());
    infoSocket.readDatagram(ba.data(),ba.size());
    infoSelfV.info=Json::decode(ba).toMap();

    sendInfoV(infoSelfV);
#endif
}
