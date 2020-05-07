#ifndef HINKDECODEV_H
#define HINKDECODEV_H
#include "HinkObject.h"

class HinkDecodeV : public HinkObject
{
    Q_OBJECT
public:
    HinkDecodeV(QObject *parent = 0);

private:

private slots:
    void onStart();
    void onNewPacketV(Packet);
    void onNewInfoV(StreamInfo);
    void onSetData(QVariantMap data);
};

#endif // HINKDECODEV_H
