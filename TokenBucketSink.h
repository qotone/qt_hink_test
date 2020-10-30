#ifndef TOKENBUCKETSINK_H
#define TOKENBUCKETSINK_H

#include "Hink.h"
#include "sockutil.h"
#include <QUdpSocket>

// ireader libs include
#include "rtp-payload.h"
#include "rtp-profile.h"
#include "rtp.h"
#include "token_bucket.h"
#include <pthread.h>
//#include <ortp/ortp.h>

//typedef struct
//{
//    unsigned char *pdata;
//    int len;
//} PayloadData;
//
//typedef QList<PayloadData> QData;

class TokenBucketSink : public HinkObject
{
    Q_OBJECT
public:
    struct RtpPacket{
        char *pdata;
        int len;
        RtpPacket(){pdata = NULL;len = 0;}
    };

    struct RtpBuffer{
        QReadWriteLock lock;
        QList<RtpPacket> buffer;
        int len;
        RtpBuffer():lock(QReadWriteLock::Recursive){

        }
    };

    //RtpSession *pRtpSession;
    QHostAddress ip;
    int dataPort;
   // socket_t m_udp;//local udp to send rtp stream
    //struct sockaddr_storage m_ss;
   // socklen_t m_addrLen;
    QUdpSocket socket;
    RtpBuffer rtpbuffer;
    TokenBucketSink(QObject *parent = 0);
    ~TokenBucketSink();
    void sendFrame(uint8_t *bf,int len,uint32_t ts);
    bool isNalu3Start(unsigned char *buffer);
    bool isNalu4Start(unsigned char *buffer);
protected:
    static void* threadStub(void* clientData) { return ((TokenBucketSink *) clientData)->token_thread();}//;
    void *token_thread();

    static void* send_threadStub(void * clientData) { return ((TokenBucketSink *) clientData)->send_thread();}
    void *send_thread();
private:
    //QHostAddress ip;
    //int dataPort;
    int infoPort;
    //QUdpSocket socket;
    struct PktHeader{
        qint64 pts;
        qint64 dts;
        bool isIframe;
    };
    struct token_bucket *m_tb;
    bool m_bRuning;

    pthread_t m_thid;
    pthread_t m_thid_snd;
    //Buffer *m_buffer;
    //struct rtp_payload_t handler;
    //void *pEncoder;
    //socket_t m_udp;//local udp to send rtp stream
    //struct sockaddr_storage m_ss;
    //socklen_t m_addrLen;

signals:

public slots:
    void onStart();
    void onStop();
    void onSetData(QVariantMap newData);
    void onNewInfoV(StreamInfo info);
    void onNewPacketV(Packet pkt);
    void onNewPacketA(Packet pkt);

};

#endif // TOKENBUCKETSINK_H
