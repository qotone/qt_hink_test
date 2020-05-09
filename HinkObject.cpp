#include "HinkObject.h"
#include <QDebug>
#include <cstring>
#include <malloc.h>
#include "hi_comm_sys.h"
#include "mpi_sys.h"
#include "hink_comm.h"
#include <cstdio>
#include <QMetaType>

HinkObject::HinkObject(QObject *parent) : QObject(parent)
{
    data["memSize"] = 0;
    data["bufLenA"] = 20;
    data["bufLenV"] = 20;
    state = created;

    //https://www.cnblogs.com/Pan-Z/p/6223440.html
    qRegisterMetaType<Packet>("Packet");
}

HinkObject::StreamInfo HinkObject::getStreamInfoV()
{
    return this->infoSelfV;
}

HinkObject::StreamInfo HinkObject::getStreamInfoA()
{
    return this->infoSrcA;
}



/*
 * 运行模块。
 * 该接口为异步接口，返回 true 并不代表模块内部工作正常，模块内部的状态可以通过 invoke 或 getData 查询
 */
bool HinkObject::start(QVariantMap da)
{
    if(!setData(da))
        return false;

    this->onStart();
    state = started;
    return true;

}

bool HinkObject::start()
{
    if(!setData(data))
        return false;

    this->onStart();
    state = started;
    return true;
}

/*
 * 设定模块的参数
 * 该接口通常在模块start 之前调用。大部分模块支持热更新，因此也可以在 start 之后随时通过该接口更新模块参数。接口内部会自动
比对输入值与当前值，重复无变化的输入参数不会引起模块的更新操作。模块的大部分参数拥有默认值，可以不设定。
*/
bool HinkObject::setData(QVariantMap map)
{
    foreach (QString key, map.keys()) {
        data[key] = map.value(key);
    }




    onSetData(data);
    //qDebug()<<__FILE__;

    return true;
}


void HinkObject::memInit(int size)
{
    memSize = size;
    memPtr = calloc(1,size);
}

char* HinkObject::bufferData(char *data,int size, int offset)
{
    return memPtr+offset;
}

void HinkObject::onStart()
{

    ///  此处，在父类中不会调用
    qDebug()<<"*****HinkObject::onStart()";
}

void HinkObject::onSetData(QVariantMap map)
{
     ////    此处应该是模块专属的参数设置，通过检测 state 来确定是全部更更新还是部分更新
     ///  此处，在父类中不会调用
    qDebug()<<"******HinkObject::onSetData";
}

QString HinkObject::name()
{
    return objectName();
}

void HinkObject::onNewPacketV(Packet p)
{

}

void HinkObject::onNewInfoV(StreamInfo info)
{

}

HinkObject * HinkObject::linkV(HinkObject *dst)
{
    connect(this,SIGNAL(newInfoV(StreamInfo)),dst,SLOT(onNewInfoV(StreamInfo)));
    if(this->infoSelfV.type == StreamInfo::VPSS){
        MPP_CHN_S stSrcChn;
        MPP_CHN_S stDestChn;
        stSrcChn.enModId = (MOD_ID_E)infoSelfV.info["modId"].toInt();
        stSrcChn.s32DevId = infoSelfV.info["devId"].toInt();
        stSrcChn.s32ChnId = infoSelfV.info["chnId"].toInt();

        stDestChn.enModId = (MOD_ID_E)(dst->infoSelfV.info["modId"].toInt());
        stDestChn.s32DevId = dst->infoSelfV.info["devId"].toInt();
        stDestChn.s32ChnId  = dst->infoSelfV.info["chnId"].toInt();

        qDebug()<<"self:"<<infoSelfV.info;
        qDebug()<<"dest:"<<dst->infoSelfV.info;

        HINK_CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");

    }else {
        connect(this,SIGNAL(newPacketV(Packet)),dst,SLOT(onNewPacketV(Packet)));
    }


    return dst;
}

HinkObject * HinkObject::linkA(HinkObject *dst)
{

    connect(this,SIGNAL(newInfoA(StreamInfo)),dst,SLOT(onNewInfoA(StreamInfo)));
    if(this->infoSelfA.type == StreamInfo::Raw){
        MPP_CHN_S stSrcChn;
        MPP_CHN_S stDestChn;
        stSrcChn.enModId = (MOD_ID_E)infoSelfA.info["modId"].toInt();
        stSrcChn.s32DevId = infoSelfA.info["devId"].toInt();
        stSrcChn.s32ChnId = infoSelfA.info["chnId"].toInt();

        stDestChn.enModId = (MOD_ID_E)(dst->infoSelfA.info["modId"].toInt());
        stDestChn.s32DevId = dst->infoSelfA.info["devId"].toInt();
        stDestChn.s32ChnId  = dst->infoSelfA.info["chnId"].toInt();

        qDebug()<<"self:"<<infoSelfA.info;
        qDebug()<<"dest:"<<dst->infoSelfA.info;

        HINK_CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");

        // test stere chn
        stSrcChn.enModId = (MOD_ID_E)infoSelfA.info["modId"].toInt();
        stSrcChn.s32DevId = infoSelfA.info["devId"].toInt();
        stSrcChn.s32ChnId = infoSelfA.info["chnId"].toInt();

        stDestChn.enModId = (MOD_ID_E)(dst->infoSelfA.info["modId"].toInt());
        stDestChn.s32DevId = dst->infoSelfA.info["devId"].toInt();
        stDestChn.s32ChnId  = dst->infoSelfA.info["chnId"].toInt() + 1;

        HINK_CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");
        // end test
    }else {
        connect(this,SIGNAL(newPacketA(Packet)),dst,SLOT(onNewPacketA(Packet)));
    }


    return dst;
    //MPP_CHN_S stSrcChn,stDestChn;

    //stSrcChn.enModId = HI_ID_AI;
    //stSrcChn.s32ChnId = aiChn;
    //stSrcChn.s32DevId = aiDev;
    //stDestChn.enModId = HI_ID_AO;
    //stDestChn.s32DevId = aoDev;
    //stDestChn.s32ChnId = aoChn;

    //return HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);

}

void HinkObject::sendPacketV(Packet pkt)
{
    emit(newPacketV(pkt));
}

void HinkObject::sendInfoV(StreamInfo info)
{
    emit(newInfoV(info));
}

QString HinkObject::getState()
{
    QString ret;
    switch (state) {
    case created:
        ret = "created";
        break;
    case started:
        ret = "started";
        break;
    case stoped:
        ret = "stoped";
        break;
    case destroyed:
        ret = "destoryed";
        break;
//    default:
//        ret = "created";
//        break;

    }

    return ret;
}
