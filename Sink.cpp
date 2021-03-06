#include "Sink.h"
#include "Json.h"

// ireader libs include
#include "rtp-payload.h"
#include "rtp-profile.h"
#include "rtp.h"
#include <QDebug>

#if 0
// for ortp
#define Y_PLOAD_TYPE 96
#define MAX_RTP_PKT_LENGTH  1400
#define DefaultTimestampIncrement 3000  //(90000/30)
#endif

static void *pEncoder;
static struct rtp_payload_t handler;

static uint32_t g_userts = 0;

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
    //qDebug()<<"send data to "<<sik->ip<<" : "<<sik->dataPort;
#if 1
    sik->socket.writeDatagram((char *)packet,bytes,sik->ip,sik->dataPort);
#else
    int r = socket_sendto(sik->m_udp, packet, bytes, 0, (struct sockaddr*) &sik->m_ss/* (struct sockaddr*)&addr */, sik->m_addrLen /* addrlen */);
#endif
    //qDebug()<<"[RTP]\tbytes:"<<bytes<<"ip:"<<sik->ip<<"port:"<<sik->dataPort;
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
    // for socket from ireader sdk
    //m_udp = socket_udp_bind(NULL, 0);
    //m_addrLen = sizeof(m_ss);
    //memset(&m_ss, 0, sizeof(m_ss));

#if 0
    // for ortp init
    ortp_init();
    ortp_scheduler_init();
    ortp_set_log_level_mask(ORTP_MESSAGE|ORTP_WARNING|ORTP_ERROR);
    pRtpSession = rtp_session_new(RTP_SESSION_SENDONLY);
    rtp_session_set_scheduling_mode(pRtpSession,1);
    rtp_session_set_blocking_mode(pRtpSession,0);
    rtp_session_set_payload_type(pRtpSession,Y_PLOAD_TYPE);
    rtp_session_set_ssrc(pRtpSession,0x1212);
#endif
}

Sink::~Sink()
{
#if 0
    rtp_session_destroy(pRtpSession);
    ortp_exit();
    ortp_global_stats_display();
#endif
    qDebug()<<"Sink Destruct.";
}

void Sink::onStart()
{
    socket.bind();
    socket.setSocketOption(QAbstractSocket::SendBufferSizeSocketOption,256 * 1024);
    rtp_packet_setsize(1456);
    qDebug("send data to %s:%d\n",this->ip.toString().toStdString().c_str(),this->dataPort);
    //socket_addr_from(&m_ss, &m_addrLen, this->ip.toString().toStdString().c_str(), this->dataPort);
    handler.alloc = rtp_alloc;
    handler.free = rtp_free;
    handler.packet = rtp_encode_packet;
    pEncoder = rtp_payload_encode_create(data["payload"].toInt(),data["payloadType"].toString().toStdString().c_str(),1,11293,&handler,this);
    if(pEncoder == NULL){
        qDebug()<<"Rtp encoder created error.";
    }else{
        qDebug()<<__FILE__<<"Rtp encoder created ok. payload type ="<<data["payloadType"].toString()<<"dest ip:"<<data["ip"].toString()<<"port ="<<data["dataPort"].toInt();
    }

#if 0
    rtp_session_set_ssrc(pRtpSession,0x1212);
    rtp_session_set_remote_addr(pRtpSession,data["ip"].toString().toStdString().c_str(),data["dataPort"].toInt());
    //rtp_session_set_remote_addr(pRtpSession,data["ip"].toString(),data["dataPort"].toInt());
#endif
}

bool Sink::isNalu3Start(unsigned char *buffer)
{
    if (buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 1) {
        return false;
    } else {
        return true;
    }
}

bool Sink::isNalu4Start(unsigned char *buffer)
{
    if (buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 0 || buffer[3] != 1) {
        return false;
    } else {
        return true;
    }
}

void Sink::sendFrame(uint8_t *buffer, int len, uint32_t userts)
{
#if 0
    unsigned char NALU = buffer[4];
    uint32_t valid_len = len - 4;

    if (valid_len <= MAX_RTP_PKT_LENGTH) {
        int offset = 0;
        int lastNaluStartPos = -1;
        while (offset < len) {
            if (isNalu4Start(buffer + offset)) {
                if (lastNaluStartPos >= 0) {
                    rtp_session_send_with_ts(pRtpSession, buffer + lastNaluStartPos,
                            offset - lastNaluStartPos, userts);
                }
                lastNaluStartPos = offset + 4;
                offset += 3;
            } else if (isNalu3Start(buffer + offset)) {
                if (lastNaluStartPos >= 0) {
                    rtp_session_send_with_ts(pRtpSession, buffer + lastNaluStartPos,
                            offset - lastNaluStartPos, userts);
                }
                lastNaluStartPos = offset + 3;
                offset += 2;
            }
            ++offset;
        }
        rtp_session_send_with_ts(pRtpSession, buffer + lastNaluStartPos,
                len - lastNaluStartPos, userts);
    } else {
        valid_len -= 1;
        int packetnum = valid_len / MAX_RTP_PKT_LENGTH;
        if (valid_len % MAX_RTP_PKT_LENGTH != 0) {
            packetnum += 1;
        }
        int i = 0;
        int pos = 5;
        while (i < packetnum) {
            if (i < packetnum - 1) {
                buffer[pos - 2] = (NALU & 0x60) | 28;
                buffer[pos - 1] = (NALU & 0x1f);
                if (0 == i) {
                    buffer[pos - 1] |= 0x80;
                }
                rtp_session_send_with_ts(pRtpSession, &buffer[pos - 2],
                MAX_RTP_PKT_LENGTH + 2, userts);
            } else {
                int iSendLen = len - pos;
                buffer[pos - 2] = (NALU & 0x60) | 28;
                buffer[pos - 1] = (NALU & 0x1f);
                buffer[pos - 1] |= 0x40;
                rtp_session_send_with_ts(pRtpSession, &buffer[pos - 2],
                        iSendLen + 2, userts);
            }
            pos += MAX_RTP_PKT_LENGTH;
            ++i;
        }
    }
#endif
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
//static bool lastIFrame = false;
static int count = 0;
#include "hi_comm_venc.h"
#include "mpi_venc.h"
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
#if 0
    switch (pkt.data[4] & 0x1f ) {
    case 1 :
        //qDebug("NALU_TYPE_SLICE");
        //if(lastIFrame){
        //    lastIFrame = false;
        //    qDebug("NALU_TYPE_SLICE---------------> lost.");
        //    return;
        //}
        //if(++count % 50 == 0)
        //    HI_MPI_VENC_RequestIDR(0,HI_FALSE);
        qDebug("NALU_TYPE_SLICE");
        break;
    case 2:
        qDebug("NALU_TYPE_DPA");
        break;
    case 3:
        qDebug("NALU_TYPE_DPB");
        break;
    case 4:
        qDebug("NALU_TYPE_DPC");
        break;
    case 5:
        qDebug("NALU_TYPE_IDR");
        break;
    case 6:
        qDebug("NALU_TYPE_SEI");
        break;
    case 7:
        qDebug("NALU_TYPE_SPS");
        //lastIFrame = true;
        break;
    case 8:
        qDebug("NALU_TYPE_PPS");
        break;
    case 9:
        qDebug("NALU_TYPE_AUD");
        break;
    default:
        qDebug("NALUT_TYPE_FILL type = %d,pkt.len = %d",pkt.data[4] & 0x1f, pkt.len);
        break;

    }
#endif

    if(pkt.len > 10000)
    qDebug( "nal type = %d,len = %d.",(pkt.data[4] & 0x7E) >> 1,pkt.len);
    //if((pkt.data[4]& 0x7e >> 1) == 32)
    //    return;
    rtp_payload_encode_input(pEncoder,pkt.data,pkt.len,pkt.pts);
#if 0

    sendFrame((uint8_t *)pkt.data,pkt.len,g_userts);
    g_userts+= DefaultTimestampIncrement;
#endif
    //qDebug()<<tr("pkt data = %1").arg(pkt.data[5]);
    //socket.writeDatagram(buf,pkt.len+sizeof(PktHeader),ip,dataPort);
}

void Sink::onNewPacketA(HinkObject::Packet pkt)
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

