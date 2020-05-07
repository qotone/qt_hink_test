#include "HinkDecodeV.h"
#include "Hink.h"
#include <QDebug>
#include "hink_vdec.h"
#include "hink_vpss.h"
#include "hink_sys.h"
#include "hink_comm.h"

HinkDecodeV::HinkDecodeV(QObject *parent): HinkObject(parent)
{
    data["width"] = 1920;
    data["height"] = 1080;
    data["block"] = false;
    data["lowLatency"] = false;

    infoSelfV.type = StreamInfo::VPSS;
    infoSelfV.info["modId"] = HI_ID_VPSS;
    infoSelfV.info["devId"] = 0;
    infoSelfV.info["chnId"] = 0;

}

void HinkDecodeV::onStart()
{
    VPSS_GRP_ATTR_S stVPSSAttr;
    VDEC_CHN_ATTR_S stVdecChnAttr;
    PAYLOAD_TYPE_E enType = PT_MJPEG;//PT_H264;
    SIZE_S stSize;
    stSize.u32Width = data["width"].toInt();
    stSize.u32Height = data["height"].toInt();
    hink_vdec_getDefChnAttr(&stVdecChnAttr, enType, &stSize);
    hink_vdec_start(0, &stVdecChnAttr,data["block"].toBool() ? VIDEO_DISPLAY_MODE_PLAYBACK : VIDEO_DISPLAY_MODE_PREVIEW, HINK_VBS_COMM);

    hink_vpss_getDefGrpAttr(&stVPSSAttr,stSize.u32Width,stSize.u32Height);
    hink_vpss_quickStart(0,0,stSize.u32Width,stSize.u32Height);

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    stSrcChn.enModId = HI_ID_VDEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = 0;

    stDestChn.enModId = HI_ID_VPSS;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId  = 0;

    HINK_CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");


}

void HinkDecodeV::onNewPacketV(HinkObject::Packet pkt)
{
    VDEC_STREAM_S stStream;
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.u64PTS = pkt.pts;
    stStream.pu8Addr = (HI_U8 *)pkt.data;
    stStream.u32Len = pkt.len;
    stStream.bEndOfStream = HI_FALSE;
    stStream.bEndOfFrame = HI_TRUE;
    //qDebug()<<tr("vdec recv %1 bytes.").arg(pkt.len);
    HI_MPI_VDEC_SendStream(0, &stStream, -1);
}

void HinkDecodeV::onNewInfoV(HinkObject::StreamInfo info)
{

}


void HinkDecodeV::onSetData(QVariantMap data)
{
    //qDebug()<<;
}
