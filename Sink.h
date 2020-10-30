#ifndef SINK_H
#define SINK_H

#include "Hink.h"
#include "sockutil.h"
#include <QUdpSocket>

// ireader libs include
#include "rtp-payload.h"
#include "rtp-profile.h"
#include "rtp.h"

//#include <ortp/ortp.h>

class Sink : public HinkObject
{
    Q_OBJECT
public:
    //RtpSession *pRtpSession;
    QHostAddress ip;
    int dataPort;
   // socket_t m_udp;//local udp to send rtp stream
    //struct sockaddr_storage m_ss;
   // socklen_t m_addrLen;
    QUdpSocket socket;
    explicit Sink(QObject *parent = 0);
    ~Sink();
    void sendFrame(uint8_t *bf,int len,uint32_t ts);
   bool isNalu3Start(unsigned char *buffer);
   bool isNalu4Start(unsigned char *buffer);
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

#endif // SINK_H
