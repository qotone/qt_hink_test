#include "TokenBucketSink.h"
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

#include "token_bucket.h"
#include "TokenBucket.h"
#include <unistd.h>
#define RATE_BPS      (2000000UL)
#define INTERVAL      (50)
#define SCALE         (0.5)
#define BUF_MAX       (100)

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
    TokenBucketSink *sik = (TokenBucketSink *)param;
    //qDebug()<<"send data to "<<sik->ip<<" : "<<sik->dataPort;

    TokenBucketSink::RtpPacket rtp_pack;
    rtp_pack.pdata = (unsigned char *)malloc(bytes);
    if(rtp_pack.pdata){
        memcpy(rtp_pack.pdata,packet,bytes);

        rtp_pack.len = bytes;
        sik->rtpbuffer.lock.lockForWrite();
        if(sik->rtpbuffer.buffer.count() > BUF_MAX){
            TokenBucketSink::RtpPacket rtp_pack1;
            rtp_pack1 = sik->rtpbuffer.buffer.first();
            if(rtp_pack1.pdata){
                free(rtp_pack1.pdata);
            }
            sik->rtpbuffer.buffer.removeFirst();
            qDebug()<<"RtpBuffer is over than "<<BUF_MAX;
        }
        sik->rtpbuffer.buffer.push_back(rtp_pack);
        sik->rtpbuffer.lock.unlock();

    }else{
        qDebug()<<"memory malloc failed!";
    }
#if 0
    sik->socket.writeDatagram((char *)packet,bytes,sik->ip,sik->dataPort);
#endif
    //qDebug()<<"[RTP]\tbytes:"<<bytes<<"ip:"<<sik->ip<<"port:"<<sik->dataPort;
}

TokenBucketSink::TokenBucketSink(QObject *parent): HinkObject(parent),socket(this)
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
    m_tb = token_bucket_init(RATE_BPS,INTERVAL,SCALE);
    m_bRuning = true;
    pthread_create(&m_thid,NULL,threadStub,this);

    pthread_create(&m_thid_snd,NULL,send_threadStub,this);

}

TokenBucketSink::~TokenBucketSink()
{
    RtpPacket data;
    qDebug()<<"TokenBucketSink destruct.....";
    m_bRuning = false;
    pthread_join(m_thid,NULL);
    pthread_join(m_thid_snd,NULL);
    token_bucket_destroy(m_tb);
    while(!rtpbuffer.buffer.empty()){
        data = rtpbuffer.buffer.first();
        if(data.pdata){
            free(data.pdata);
            rtpbuffer.buffer.removeFirst();
        }else{
            break;
        }
    }
    rtpbuffer.buffer.clear();
}

void * TokenBucketSink::token_thread()
{

    uint64_t add_tokens = m_tb->add_tokens;
    while(m_bRuning){
        //qDebug()<<"token_bucket added..."<<add_tokens<<" bytes.";
       token_bucket_add_tokens(m_tb,add_tokens);
       usleep(m_tb->interval * 1000);

    }

    qDebug()<<"token_thread exit";
    return NULL;

}
#if 1
void * TokenBucketSink::send_thread()
{
    bool bStop = 0;
    RtpPacket rtp_pack;
    enum token_bucket_action ret;
    TokenBucket t(250000, 2000000);
    while(m_bRuning){
        rtpbuffer.lock.lockForRead();
        if(!rtpbuffer.buffer.empty()){

#if 1

           #if 0
            rtp_pack = rtpbuffer.buffer.first();
            rtpbuffer.buffer.removeFirst();
            rtpbuffer.lock.unlock();

            if(rtp_pack.pdata == NULL || rtp_pack.len == 0){
                break;
            }

            token_bucket_get_tokens(m_tb,rtp_pack.len );
            socket.writeDatagram((char *)rtp_pack.pdata,rtp_pack.len,ip,dataPort);
            free(rtp_pack.pdata);

           #else

            rtp_pack = rtpbuffer.buffer.first();
            rtpbuffer.buffer.removeFirst();
            rtpbuffer.lock.unlock();

            if(rtp_pack.pdata == NULL || rtp_pack.len == 0){
                break;
            }

//            if (t.consume(rtp_pack.len)) {
              //send bs octets
            qDebug("send %d bytes.",rtp_pack.len);
              socket.writeDatagram((char *)rtp_pack.pdata,rtp_pack.len,ip,dataPort);
//            } else {
//                qDebug()<<"stucked.";
//                usleep(1000);
//            }
            free(rtp_pack.pdata);
            #endif

#else

            rtp_pack = rtpbuffer.buffer.first();
            rtpbuffer.buffer.removeFirst();
            rtpbuffer.lock.unlock();

            if(rtp_pack.pdata == NULL || rtp_pack.len == 0){
                break;
            }

            //token_bucket_get_tokens(m_tb,rtp_pack.len*8 );
            ret = token_bucket_try_get_tokens(m_tb, rtp_pack.len);
            if (ret == TB_SUCCESS) {
            socket.writeDatagram((char *)rtp_pack.pdata,rtp_pack.len,ip,dataPort);
            }else{
                qDebug()<<"token_sucked!";
                /*sleep half of interval to wait new tokens*/
                usleep(m_tb->interval * 1000 / 2);
            }
            free(rtp_pack.pdata);
#endif
        }else {
            rtpbuffer.lock.unlock();
            usleep(1000);
        }


    }

    qDebug()<<"send_thread exit";
    return NULL;

}
#else
void * TokenBucketSink::send_thread()
{
    RtpPacket rtp_pack;
    enum token_bucket_action ret;
    while(m_bRuning){
        token_bucket_get_tokens(m_tb,1500);
        rtpbuffer.lock.lockForRead();
        if(!rtpbuffer.buffer.empty()){
            rtp_pack = rtpbuffer.buffer.first();
            rtpbuffer.buffer.removeFirst();
            rtpbuffer.lock.unlock();
            if(rtp_pack.pdata == NULL || rtp_pack.len == 0){
                break;
            }
            socket.writeDatagram((char *)rtp_pack.pdata,rtp_pack.len,ip,dataPort);
            free(rtp_pack.pdata);
        }else{
            rtpbuffer.lock.unlock();
            usleep(1000);
        }
    }
    return NULL;
}
#endif
void TokenBucketSink::onStart()
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

bool TokenBucketSink::isNalu3Start(unsigned char *buffer)
{
    if (buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 1) {
        return false;
    } else {
        return true;
    }
}

bool TokenBucketSink::isNalu4Start(unsigned char *buffer)
{
    if (buffer[0] != 0 || buffer[1] != 0 || buffer[2] != 0 || buffer[3] != 1) {
        return false;
    } else {
        return true;
    }
}

void TokenBucketSink::sendFrame(uint8_t *buffer, int len, uint32_t userts)
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

void TokenBucketSink::onStop()
{
    socket.close();
}

void TokenBucketSink::onSetData(QVariantMap newData)
{
    dataPort=data["dataPort"].toInt();
    infoPort=data["infoPort"].toInt();
    ip.setAddress(data["ip"].toString());
    if(!newData.isEmpty() && infoSrcV.type!=StreamInfo::UnKnow)
        onNewInfoV(infoSelfV);
}

void TokenBucketSink::onNewInfoV(StreamInfo info)
{
    infoSrcV=info;

  //  if(!ip.isNull())
  //      socket.writeDatagram(Json::encode(info.info).toLatin1(),ip,infoPort);
}
//static bool lastIFrame = false;
static int count = 0;
//#include "hi_comm_venc.h"
//#include "mpi_venc.h"
void TokenBucketSink::onNewPacketV(HinkObject::Packet pkt)
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

    //if(pkt.len > 10000)
    //qDebug( "nal type = %d,len = %d.",(pkt.data[4] & 0x7E) >> 1,pkt.len);
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

void TokenBucketSink::onNewPacketA(HinkObject::Packet pkt)
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
