#ifndef HINKENCODEV_H
#define HINKENCODEV_H
#include "HinkObject.h"
#include "hi_comm_venc.h"
#include <QSocketNotifier>

class HinkEncodeV : public HinkObject
{
    Q_OBJECT
public:
    HinkEncodeV(QObject *parent = 0);
    enum Profile {
        BASELINE = 0,
        MP,
        HP,
        SVC
    };

public slots:
    void onReadVencData(void);

private slots:
    void onStart();
    void onSetData(QVariantMap);

private:
    VENC_STREAM_S m_stStream;
    VENC_CHN m_venChn;
    HI_S32 m_vencFd;
    VENC_CHN_ATTR_S m_stVencChnAttr;
    static int venChnNum;
    static HI_U64 pts;
    QSocketNotifier *notifier;

    bool resetVencChnAttr(QString payloadType,QString rcMode);
};

#endif // HINKENCODEV_H
