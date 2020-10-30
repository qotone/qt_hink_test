#ifndef HINKENCODEA_H
#define HINKENCODEA_H
#include "HinkObject.h"
#include "hi_comm_aenc.h"
#include "mpi_aenc.h"
#include <QSocketNotifier>
class HinkEncodeA : public HinkObject
{
    Q_OBJECT
public:
    HinkEncodeA(QObject *parent = 0);
    ~HinkEncodeA();
private:

    QSocketNotifier *notifier;
    AENC_CHN m_chn;
    AENC_CHN_ATTR_S m_stAencAttr;
    HI_U64 m_seq;
    HI_U64 m_pkt;
    qint64 m_lastTime;
private slots:

    void onStart();
    void onNewPacketA(Packet) ;
    void onNewInfoA(StreamInfo);
    void onSetData(QVariantMap);
public slots:
    void onReadData(void);
};

#endif // HINKENCODEA_H
