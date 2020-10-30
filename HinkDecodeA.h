#ifndef HINKDECODEA_H
#define HINKDECODEA_H
#include "HinkObject.h"
#include "hi_comm_adec.h"
#include "mpi_adec.h"

class HinkDecodeA : public HinkObject
{
    Q_OBJECT
public:
    HinkDecodeA(QObject *parent = 0);
private:
    ADEC_CHN_ATTR_S m_stAdecAttr;
    ADEC_CHN m_chn;
private slots:
    void onStart();
    void onNewPacketA(Packet) ;
    void onNewInfoA(StreamInfo);
    void onSetData(QVariantMap);
};

#endif // HINKDECODEA_H
