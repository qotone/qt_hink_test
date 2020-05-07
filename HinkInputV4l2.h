#ifndef HINKINPUTV4L2_H
#define HINKINPUTV4L2_H
#include "HinkObject.h"
#include "libuvc.h"
#include <QSocketNotifier>

class HinkInputV4l2 : public HinkObject
{
    Q_OBJECT
public:
    HinkInputV4l2(QObject *parent = 0);


public slots:
    void onReadCamData(void);

private slots:
    void onStart();
    void onSetData(QVariantMap data);
    void onNewPacketV(Packet);


private:
    struct uvc_ctx *uvc;
    struct uvc_param uvcParam;
    QSocketNotifier *notifier;

};

#endif // HINKINPUTV4L2_H
