#ifndef HINKOUTPUTAO_H
#define HINKOUTPUTAO_H
#include "HinkObject.h"

class HinkOutputAo : public HinkObject
{
    Q_OBJECT
public:
    HinkOutputAo(QObject *parent = 0);

private slots:
    void onStart();
    void onSetData(QVariantMap data);
    void onNewPacketA(Packet);
};

#endif // HINKOUTPUTAO_H
