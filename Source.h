#ifndef SOURCE_H
#define SOURCE_H

#include "Hink.h"
#include "HinkObject.h"

#include <QUdpSocket>

class Source : public HinkObject
{
    Q_OBJECT
public:
    explicit Source(QObject *parent = 0);
    //static void* threadStub(void *obj) { return ((Source *) obj)->decode_thread();}
    //void* decode_thread();
    QUdpSocket dataSocket;
    QUdpSocket infoSocket;
    void sendPacket(Packet pkg);
private:
    struct PktHeader{
        qint64 pts;
        qint64 dts;
        bool isIframe;
    };
signals:

public slots:
    void onStart();
    void onReadData();
    void onReadInfo();
};


#endif // SOURCE_H
