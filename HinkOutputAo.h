#ifndef HINKOUTPUTAO_H
#define HINKOUTPUTAO_H
#include "HinkObject.h"
#include "hi_comm_vb.h"
#include "mpi_vb.h"


class HinkOutputAo : public HinkObject
{
    Q_OBJECT
public:
    HinkOutputAo(QObject *parent = 0);
    ~HinkOutputAo();

private:
    HI_U32 *m_pVirAddr;
    HI_U32 m_phyAddr;
    HI_U32 m_vbSize;
    VB_BLK m_handleVB;
    VB_POOL poolID;
    HI_U32 m_seq;
    AUDIO_DEV aoDev;
    AI_CHN aoChn;

private slots:
    void onStart();
    void onSetData(QVariantMap data);
    void onNewPacketA(Packet);
};

#endif // HINKOUTPUTAO_H
