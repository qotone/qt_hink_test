#ifndef SINK_H
#define SINK_H

#include "Hink.h"
#include <QUdpSocket>

// ireader libs include
#include "rtp-payload.h"
#include "rtp-profile.h"
#include "rtp.h"

class Sink : public HinkObject
{
    Q_OBJECT
public:

    QHostAddress ip;
    int dataPort;
    QUdpSocket socket;
    explicit Sink(QObject *parent = 0);
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
signals:

public slots:
    void onStart();
    void onStop();
    void onSetData(QVariantMap newData);
    void onNewInfoV(StreamInfo info);
    void onNewPacketV(Packet pkt);
};

#endif // SINK_H
