#ifndef HINKDECODEV_H
#define HINKDECODEV_H
#include "HinkObject.h"

#include "hi_comm_vb.h" // for VB_BLK
#include "hi_comm_video.h" // for VIDEO_FRAME_INFO_S

class HinkDecodeV : public HinkObject
{
    Q_OBJECT
public:
    HinkDecodeV(QObject *parent = 0);
    ~HinkDecodeV();

private:
    VB_BLK m_vbBlk;
    VIDEO_FRAME_INFO_S m_vFrameInfo;
    bool m_enabledUserPic;

    int getVFrameFromYUV(QString file,uint32_t _w,uint32_t _h);
    int planToSemi(uint8_t *pY,uint32_t yStride,uint8_t *pU,uint32_t uStride,uint8_t *pV,uint32_t vStride,uint32_t picWidth,uint32_t picHeight);

private slots:
    void onStart();
    void onNewPacketV(Packet);
    void onNewInfoV(StreamInfo);
    void onSetData(QVariantMap data);
};

#endif // HINKDECODEV_H
