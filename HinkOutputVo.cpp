#include "HinkOutputVo.h"
#include "hink_vo.h"
#include "hink_comm.h"
#include <QVariantMap>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <QDebug>
#include <QString>
#include <QStringList>

#include "hi_comm_vo.h"
#include "mpi_vo.h"
#include "hink_vo.h"
#include "hink_comm.h"

int HinkOutputVo::voCount = 0;

HinkOutputVo::HinkOutputVo(QObject *parent): HinkObject(parent)
{
    data["type"] = "hdmi";
    data["mode"] = -1;
    data["output"] = "1080P60";
    data["ui"] = false;
    data["flip"] = false;
    //qDebug()<<"HinkOutputVo constructor runed.";

    initModeList();
    typeMap["hdmi"] = VO_INTF_HDMI;
    typeMap["bt1120"] = VO_INTF_BT1120;
    typeMap["cvbs"] = VO_INTF_CVBS;
    typeMap["ypbpr"] = VO_INTF_YPBPR;
    typeMap["vga"] = VO_INTF_VGA;
    typeMap["bt656"] = VO_INTF_BT656;
    typeMap["lcd"] = VO_INTF_LCD;
    typeMap["bt656_l"] = VO_INTF_BT656_L;
    typeMap["bt656_h"] = VO_INTF_BT656_H;






}

void HinkOutputVo::initModeList()
{
    QStringList _modeList;
    int i = 0;
    _modeList<<"PAL"<<"NTSC"<<"960H_PAL"<<"960H_NTSC"<<"1080P24"<<"1080P25"<<"1080P30"<<"720P50"<<"720P60"<<"1080I50"<<"1080I60"<<"1080P50"<<"1080P60";
    _modeList<<"576P50"<<"480P60"<<"640x480_60"<<"800x600_60"<<"1024x768_60"<<"1280x1024_60"<<"1366x768_60"<<"1440x900_60"<<"1280x800_60"<<"1680x1050_60"<<"1920x2160_30"<<"1600x1200_60"<<"1920x1200_60"<<"2560x1440_30"<<"2560x1440_60"<<"2560x1600_60"<<"3840x2160_25"<<"3840x2160_30"<<"3840x2160_50"<<"3840x2160_60"<<"USER"<<"BUTT";

    foreach (QString str, _modeList) {
        outputMap[str] = i++;
    }

}

void HinkOutputVo::onSetData(QVariantMap data)
{

    if(this->state == HinkObject::created){ // 如果是第一次配置模块参数

    }else if(this->state == HinkObject::started){ // 不是第一次配置模块参数

    }else if(this->state == HinkObject::stoped){ // 模块停止了，是不是要和 started 归一齐呀？

    }else{
        qDebug()<<"No way! terriblity thing to be here!";
    }
}

void HinkOutputVo::onStart()
{
#if 0
    HI_S32 s32Ret = HI_SUCCESS;
    VO_PUB_ATTR_S stVoAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN voChn = 0;
    HINK_RECT_S stRect = {.x = 0, .y = 0, .w = 1920, .h = 1080};

    stVoAttr.enIntfSync = VO_OUTPUT_1080P60;
    stVoAttr.enIntfType = VO_INTF_HDMI;
    stVoAttr.u32BgColor = 0xFFFFFF;
    memset(&stLayerAttr,0,sizeof(VO_VIDEO_LAYER_ATTR_S));

    stLayerAttr.bClusterMode = HI_FALSE;
    stLayerAttr.bDoubleFrame = HI_FALSE;
    s32Ret = hink_vo_getWH(stVoAttr.enIntfSync,&stLayerAttr.stImageSize.u32Width,&stLayerAttr.stImageSize.u32Height, &stLayerAttr.u32DispFrmRt);
    if (HI_SUCCESS != s32Ret){
        HINK_PRT("hink_vo_getWH failed!\n");
        return HI_FAILURE;
    }

    stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    stLayerAttr.stDispRect.s32X       = 0;
    stLayerAttr.stDispRect.s32Y       = 0;
    stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
    stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;

    s32Ret = hink_vo_startDev(HINK_VO_DEV_DHD0, &stVoAttr);

    s32Ret = hink_vo_startLayer(HINK_VO_LAYER_VHD0,&stLayerAttr);

    s32Ret = hink_vo_startChn(HINK_VO_LAYER_VHD0,voChn,&stRect);

    if(1 /* stVoAttr.enIntfSync & VO_INTF_HDMI */){
        s32Ret = hink_vo_startHdmi(stVoAttr.enIntfSync, HI_TRUE);
    }
#endif

    //qDebug()<<"name() :"<<this->name();
    HI_S32 s32Ret = HI_SUCCESS;
    HINK_RECT_S stRect;
    VO_PUB_ATTR_S stVoAttr;
    VO_VIDEO_LAYER_ATTR_S stLayerAttr;
    VO_CHN voChn = 0;
//    VO_DEV voDev = HINK_VO_DEV_DHD0;
    int _type = 0;
    int _mode = outputMap.value(data["output"].toString());

    QStringList _typeList = data["type"].toStringList();

    foreach (QString str, _typeList) {
        _type |= typeMap.value(str);
    }

    data["mode"] = _mode;
    stVoAttr.enIntfSync = (VO_INTF_SYNC_E)_mode;
    stVoAttr.enIntfType = _type;
    stVoAttr.u32BgColor = 0xFFFFFF;
    memset(&(stLayerAttr), 0 , sizeof(VO_VIDEO_LAYER_ATTR_S));
    stLayerAttr.bClusterMode = HI_FALSE;
    stLayerAttr.bDoubleFrame = HI_FALSE;
    s32Ret = hink_vo_getWH(stVoAttr.enIntfSync,&stLayerAttr.stImageSize.u32Width,&stLayerAttr.stImageSize.u32Height, &stLayerAttr.u32DispFrmRt);
    if(HI_SUCCESS != s32Ret){
        qDebug()<<"hink_vo_getWH failed!";
    }
    stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    stLayerAttr.stDispRect.s32X       = 0;
    stLayerAttr.stDispRect.s32Y       = 0;
    stLayerAttr.stDispRect.u32Width   = stLayerAttr.stImageSize.u32Width;
    stLayerAttr.stDispRect.u32Height  = stLayerAttr.stImageSize.u32Height;
    stRect.x = 0;
    stRect.y = 0;
    stRect.w = stLayerAttr.stImageSize.u32Width;
    stRect.h = stLayerAttr.stImageSize.u32Height;

    s32Ret = hink_vo_startDev(voCount,&stVoAttr);
    s32Ret = hink_vo_startLayer(voCount,&stLayerAttr);
    s32Ret = hink_vo_startChn(voCount,voChn,&stRect);
    if(_type & VO_INTF_HDMI){
        s32Ret = hink_vo_startHdmi(stVoAttr.enIntfSync,HI_TRUE);
    }

    infoSelfV.type = StreamInfo::VPSS;
    infoSelfV.info["modId"] = HI_ID_VOU;
    infoSelfV.info["devId"] = voCount;
    infoSelfV.info["chnId"] = voCount;
    voCount++;
}

void HinkOutputVo::onNewPacketV(HinkObject::Packet pkt)
{
    qDebug()<<"pkt.pts = "<<pkt.pts;
    if(data.contains("bufLenV")){
        qDebug()<<data["bufLenV"].toInt();
    }

}


