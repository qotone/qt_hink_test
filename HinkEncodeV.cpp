#include "HinkEncodeV.h"
#include "hi_common.h"
#include <QDebug>
#include <cstring>
#include <cstdlib>
#include "mpi_venc.h"
int HinkEncodeV::venChnNum = 0;
HI_U64 HinkEncodeV::pts = 0;

HinkEncodeV::HinkEncodeV(QObject *parent): HinkObject(parent)
{
    data["codec"] = "h264"; // "h264,h265,jpeg
    data["profile"] = "high";//"baseline, mp, hp, svc.....
    data["width"] = -1;
    data["height"] = -1;
    data["bitrate"] = 1024;
    data["framerate"] = 30;
    data["gop"] = 2;
    data["gopmode"] = 0;
    data["rcmode"] = "cbr"; // "cbr","vbr","avbr","fixqp"
    data["snap"] = true;
    data["minqp"] = 22;
    data["maxqp"] = 36;
    data["Iqp"] = 0;
    data["Pqp"] = 0;
    data["Bqp"] = 0;
    //data["roi"];
    data["rotate"] = 0;
    m_venChn = venChnNum;
    venChnNum++;
}

bool HinkEncodeV::resetVencChnAttr(QString payloadType,QString rcMode)
{

    bool ret = true;
    uint32_t _w = 1920, _h = 1080;
    VENC_CHN_ATTR_S stVencChnAttr;
    stVencChnAttr.stGopAttr.enGopMode = VENC_GOPMODE_NORMALP;
    stVencChnAttr.stGopAttr.stNormalP.s32IPQpDelta = 1;
    if("h264" == payloadType){
        VENC_ATTR_H264_S *_pstH264Attr = &stVencChnAttr.stVeAttr.stAttrH264e;
        stVencChnAttr.stVeAttr.enType = PT_H264;
        _pstH264Attr->u32MaxPicWidth = _w;
        _pstH264Attr->u32MaxPicHeight = _h;
        _pstH264Attr->u32BufSize = _w * _h * 3 / 2;
        _pstH264Attr->u32Profile = 0;
        _pstH264Attr->bByFrame = HI_TRUE;

        if("cbr" == rcMode){
            VENC_ATTR_H264_CBR_S *_pstH264Cbr = &stVencChnAttr.stRcAttr.stAttrH264Cbr;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
            _pstH264Cbr->u32Gop = 30;
            _pstH264Cbr->u32StatTime = 1;
            _pstH264Cbr->u32SrcFrmRate = 30;
            _pstH264Cbr->fr32DstFrmRate = 30;
            _pstH264Cbr->u32BitRate = 4000;
            _pstH264Cbr->u32FluctuateLevel = 1;
        } else if("fixqp" == rcMode){
            VENC_ATTR_H264_FIXQP_S *_pstH264FixQp = &stVencChnAttr.stRcAttr.stAttrH264FixQp;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264FIXQP;
            _pstH264FixQp->u32Gop = 30;
            _pstH264FixQp->u32SrcFrmRate = 30;
            _pstH264FixQp->fr32DstFrmRate = 30;
            _pstH264FixQp->u32IQp = 20;
            _pstH264FixQp->u32PQp = 23;
            _pstH264FixQp->u32BQp = 23;
        } else if("vbr" == rcMode){
            VENC_ATTR_H264_VBR_S *_pstH264Vbr = &stVencChnAttr.stRcAttr.stAttrH264Vbr;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264VBR;
            _pstH264Vbr->u32Gop = 30;
            _pstH264Vbr->u32StatTime = 1;
            _pstH264Vbr->u32SrcFrmRate = 30;
            _pstH264Vbr->fr32DstFrmRate = 30;
            _pstH264Vbr->u32MinQp = 10;
            _pstH264Vbr->u32MinIQp = 10;
            _pstH264Vbr->u32MaxQp = 40;
            _pstH264Vbr->u32MaxBitRate = 4000;
        } else if("avbr" == rcMode){
            VENC_ATTR_H264_AVBR_S *_pstH264Avbr = &stVencChnAttr.stRcAttr.stAttrH264AVbr;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H264AVBR;
            _pstH264Avbr->u32Gop = 30;
            _pstH264Avbr->u32StatTime = 1;
            _pstH264Avbr->u32SrcFrmRate = 30;
            _pstH264Avbr->fr32DstFrmRate = 30;
            _pstH264Avbr->u32MaxBitRate = 4000;
        } else {
            qDebug()<<tr("Not support this RC Mode = %1.").arg(rcMode);
            ret = false;
        }



    }else if("mjpeg" == payloadType){
        VENC_ATTR_MJPEG_S *_pstMjpegAttr = &stVencChnAttr.stVeAttr.stAttrMjpege;
        stVencChnAttr.stVeAttr.enType  = PT_MJPEG;
        _pstMjpegAttr->u32MaxPicWidth = _w;
        _pstMjpegAttr->u32MaxPicHeight = _h;
        _pstMjpegAttr->u32PicWidth = _w;
        _pstMjpegAttr->u32PicHeight = _h;
        _pstMjpegAttr->u32BufSize  = _w * _h * 3 / 2;
        _pstMjpegAttr->bByFrame = HI_TRUE;

        if("fixqp" == rcMode){
         VENC_ATTR_MJPEG_FIXQP_S *_pstMjpegFixQp = &stVencChnAttr.stRcAttr.stAttrMjpegeFixQp;
         stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGFIXQP;
         _pstMjpegFixQp->u32Qfactor = 90;
         _pstMjpegFixQp->u32SrcFrmRate = 30;
         _pstMjpegFixQp->fr32DstFrmRate = 30;
        } else if( "cbr" == rcMode){
         VENC_ATTR_MJPEG_CBR_S *_pstMjpegCbr = &stVencChnAttr.stRcAttr.stAttrMjpegeCbr;
         stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGCBR;
         _pstMjpegCbr->u32StatTime = 1;
         _pstMjpegCbr->u32SrcFrmRate = 30;
         _pstMjpegCbr->fr32DstFrmRate = 30;
         _pstMjpegCbr->u32FluctuateLevel = 1;
         _pstMjpegCbr->u32BitRate = 4000;
        } else if( "vbr" == rcMode){
         VENC_ATTR_MJPEG_VBR_S *_pstMjpegVbr = &stVencChnAttr.stRcAttr.stAttrMjpegeVbr;
         stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_MJPEGVBR;
         _pstMjpegVbr->u32StatTime = 1;
         _pstMjpegVbr->u32SrcFrmRate = 30;
         _pstMjpegVbr->fr32DstFrmRate = 5;
         _pstMjpegVbr->u32MinQfactor = 50;
         _pstMjpegVbr->u32MaxQfactor = 95;
         _pstMjpegVbr->u32MaxBitRate = 4000;
        } else {

            qDebug()<<tr("Not support this RC Mode = %1.").arg(rcMode);
            ret = false;
        }


    }else if("jpeg" == payloadType){
        VENC_ATTR_JPEG_S *_pstJpegAttr = &stVencChnAttr.stVeAttr.stAttrJpege;
        stVencChnAttr.stVeAttr.enType = PT_JPEG;
        _pstJpegAttr->u32PicWidth = _w;
        _pstJpegAttr->u32PicHeight = _h;
        _pstJpegAttr->u32MaxPicWidth = _w;
        _pstJpegAttr->u32MaxPicHeight = _h;
        _pstJpegAttr->u32BufSize = _w * _h * 3 / 2;
        _pstJpegAttr->bByFrame = HI_TRUE;
        _pstJpegAttr->bSupportDCF = HI_FALSE;


    } else if( "h265" == payloadType ) {
        VENC_ATTR_H265_S *_pstH265Attr = &stVencChnAttr.stVeAttr.stAttrH265e;
        stVencChnAttr.stVeAttr.enType = PT_H265;
        _pstH265Attr->u32MaxPicWidth = _w;
        _pstH265Attr->u32MaxPicHeight = _h;
        _pstH265Attr->u32PicWidth = _w;
        _pstH265Attr->u32PicHeight = _h;
        _pstH265Attr->u32BufSize = _w * _h * 3 / 2;
        _pstH265Attr->u32Profile = 0;
        _pstH265Attr->bByFrame = HI_TRUE;
        if("cbr" == rcMode){
            VENC_ATTR_H265_CBR_S *_pstH265Cbr = &stVencChnAttr.stRcAttr.stAttrH265Cbr;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
            _pstH265Cbr->u32Gop = 30;
            _pstH265Cbr->u32StatTime = 1;
            _pstH265Cbr->u32SrcFrmRate = 30;
            _pstH265Cbr->fr32DstFrmRate = 30;
            _pstH265Cbr->u32BitRate = 4000;
            _pstH265Cbr->u32FluctuateLevel = 3;
        }else if("fixqp" == rcMode){
            VENC_ATTR_H265_FIXQP_S *_pstH265FixQp = &stVencChnAttr.stRcAttr.stAttrH265FixQp;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265FIXQP;
            _pstH265FixQp->u32Gop = 30;
            _pstH265FixQp->u32SrcFrmRate = 30;
            _pstH265FixQp->fr32DstFrmRate = 30;
            _pstH265FixQp->u32IQp = 20;
            _pstH265FixQp->u32PQp = 23;
            _pstH265FixQp->u32BQp = 25;
        }else if("vbr" == rcMode){
            VENC_ATTR_H265_VBR_S *_pstH265Vbr = &stVencChnAttr.stRcAttr.stAttrH265Vbr;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265VBR;
            _pstH265Vbr->u32Gop = 30;
            _pstH265Vbr->u32StatTime = 1;
            _pstH265Vbr->u32SrcFrmRate = 30;
            _pstH265Vbr->fr32DstFrmRate = 30;
            _pstH265Vbr->u32MinQp = 10;
            _pstH265Vbr->u32MinIQp = 10;
            _pstH265Vbr->u32MaxQp = 40;
            _pstH265Vbr->u32MaxBitRate = 4000;
        }else if("avbr" == rcMode){
            VENC_ATTR_H265_AVBR_S *_pstH265Avbr = &stVencChnAttr.stRcAttr.stAttrH265AVbr;
            stVencChnAttr.stRcAttr.enRcMode = VENC_RC_MODE_H265AVBR;
            _pstH265Avbr->u32Gop = 30;
            _pstH265Avbr->u32StatTime = 1;
            _pstH265Avbr->u32SrcFrmRate = 30;
            _pstH265Avbr->fr32DstFrmRate = 30;
            _pstH265Avbr->u32MaxBitRate = 4000;
        }
    } else {
        qDebug()<<tr("Not support this payload:%1").arg(payloadType);
        ret = false;
    }
    if(ret)
        memcpy(&m_stVencChnAttr,&stVencChnAttr,sizeof(VENC_CHN_ATTR_S));

    return ret;
}

void HinkEncodeV::onSetData(QVariantMap map)
{

   map["memSize"] = map["width"].toUInt() * map["height"].toUInt() * 2; // for test......
   Profile profile;
   if(map["profile"].toString() == "baseline"){
       profile = BASELINE;
   }else if(map["profile"].toString() == "high"){
       profile = HP;
   }else if(map["profile"].toString() == "main"){
       profile = MP;
   }else {
       profile = HP;
   }

   if(resetVencChnAttr(map["codec"].toString(),map["rcmode"].toString())){

      if(map["codec"].toString() == "h264"){
         VENC_ATTR_H264_S *_pstH264Attr = &m_stVencChnAttr.stVeAttr.stAttrH264e;
          if(map["width"].toInt() != -1 && map["height"].toInt() != -1){
              _pstH264Attr->u32MaxPicWidth = _pstH264Attr->u32PicWidth = map["width"].toUInt();
              _pstH264Attr->u32MaxPicHeight = _pstH264Attr->u32PicHeight = map["height"].toUInt();
              _pstH264Attr->u32BufSize = map["width"].toUInt() * map["height"].toUInt() * 3 / 2;
              if("cbr" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH264Cbr.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264Cbr.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH264Cbr.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264Cbr.u32BitRate = map["bitrate"].toUInt();
              }else if("fixqp" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH264FixQp.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264FixQp.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH264FixQp.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264FixQp.u32BQp = map["Bqp"].toUInt();

                  m_stVencChnAttr.stRcAttr.stAttrH264FixQp.u32IQp = map["Iqp"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264FixQp.u32PQp = map["Pqp"].toUInt();
              }else if("vbr" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH264Vbr.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264Vbr.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH264Vbr.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264Vbr.u32MaxQp = map["maxqp"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264Vbr.u32MinQp = map["minqp"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264Vbr.u32MaxBitRate = map["bitrate"].toUInt();
              }else if("avbr" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH264AVbr.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH264AVbr.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264AVbr.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH264AVbr.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH264AVbr.u32MaxBitRate = map["bitrate"].toUInt();
              }else{

              }
          }
      }else if(map["codec"].toString() == "h265"){

         VENC_ATTR_H265_S *_pstH265Attr = &m_stVencChnAttr.stVeAttr.stAttrH265e;
          if(map["width"].toInt() != -1 && map["height"].toInt() != -1){
              _pstH265Attr->u32MaxPicWidth = _pstH265Attr->u32PicWidth = map["width"].toUInt();
              _pstH265Attr->u32MaxPicHeight = _pstH265Attr->u32PicHeight = map["height"].toUInt();
              _pstH265Attr->u32BufSize = map["width"].toUInt() * map["height"].toUInt() * 3 / 2;
              if("cbr" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH265Cbr.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265Cbr.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH265Cbr.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265Cbr.u32BitRate = map["bitrate"].toUInt();
              }else if("fixqp" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH265FixQp.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH265FixQp.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265FixQp.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH265FixQp.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265FixQp.u32BQp = map["Bqp"].toUInt();

                  m_stVencChnAttr.stRcAttr.stAttrH265FixQp.u32IQp = map["Iqp"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265FixQp.u32PQp = map["Pqp"].toUInt();
              }else if("vbr" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH265Vbr.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH265Vbr.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265Vbr.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH265Vbr.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265Vbr.u32MaxQp = map["maxqp"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265Vbr.u32MinQp = map["minqp"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265Vbr.u32MaxBitRate = map["bitrate"].toUInt();
              }else if("avbr" == map["rcmode"].toString()){

                  m_stVencChnAttr.stRcAttr.stAttrH265AVbr.fr32DstFrmRate = m_stVencChnAttr.stRcAttr.stAttrH265AVbr.u32SrcFrmRate = map["framerate"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265AVbr.u32Gop =  m_stVencChnAttr.stRcAttr.stAttrH265AVbr.u32SrcFrmRate * map["gop"].toUInt();
                  m_stVencChnAttr.stRcAttr.stAttrH265AVbr.u32MaxBitRate = map["bitrate"].toUInt();
              }else{

              }
          }
      }else if(map["codec"].toString() == "jpeg"){

          m_stVencChnAttr.stVeAttr.stAttrJpege.u32PicWidth = m_stVencChnAttr.stVeAttr.stAttrJpege.u32MaxPicWidth = map["width"].toUInt();
          m_stVencChnAttr.stVeAttr.stAttrJpege.u32PicHeight = m_stVencChnAttr.stVeAttr.stAttrJpege.u32MaxPicHeight = map["height"].toUInt();
          m_stVencChnAttr.stVeAttr.stAttrJpege.u32BufSize =  m_stVencChnAttr.stVeAttr.stAttrJpege.u32MaxPicWidth * m_stVencChnAttr.stVeAttr.stAttrJpege.u32MaxPicHeight * 3 / 2;
      }else{

      }
   }
}

void HinkEncodeV::onStart()
{
    HI_S32 s32Ret = HI_SUCCESS;
    VENC_STREAM_BUF_INFO_S stStreamBufInfo;
    s32Ret = HI_MPI_VENC_CreateChn(m_venChn,&m_stVencChnAttr);
    if(HI_SUCCESS != s32Ret){
        qDebug("HI_MPI_VENC_CreateChn [%d] failed with %#x!",m_venChn,s32Ret);
    }
    s32Ret = HI_MPI_VENC_StartRecvPic(m_venChn);

    if(HI_SUCCESS != s32Ret){
        qDebug("HI_MPI_VENC_StartRecvPic [%d] failed with %#x!",m_venChn,s32Ret);
    }

    m_vencFd = HI_MPI_VENC_GetFd(m_venChn);
    if(m_vencFd < 0){
        qDebug("HI_MPI_VENC_GetFd(%d) failed!",m_vencFd);
        return;
    }

    s32Ret = HI_MPI_VENC_GetStreamBufInfo(m_venChn,&stStreamBufInfo);
    if(HI_SUCCESS != s32Ret){
        qDebug("HI_MPI_VENC_GetStreamBufInfo failed with %#x!",s32Ret);
        return;
    }


    this->notifier = new QSocketNotifier(m_vencFd,QSocketNotifier::Read,this);
    connect(this->notifier,SIGNAL(activated(int)),this,SLOT(onReadVencData()));


    infoSelfV.type = StreamInfo::H264;
    infoSelfV.info["modId"] = HI_ID_VENC;
    infoSelfV.info["devId"] = 0;
    infoSelfV.info["chnId"] = m_venChn;
}

void HinkEncodeV::onReadVencData()
{

    HinkObject::Packet pkt;
    VENC_CHN_STAT_S stState;
    int size = 0;
    int buf_offset = 0;
    HI_S32 s32Ret = HI_SUCCESS;
    memset(&m_stStream,0,sizeof(m_stStream));
    s32Ret = HI_MPI_VENC_Query(m_venChn,&stState);
    if(HI_SUCCESS != s32Ret){
        qDebug("HI_MPI_VENC_Query chn[%d] failed with %#x!",m_venChn,s32Ret);
        return;
    }
    if(0 == stState.u32CurPacks){
        qDebug("%s:Current frame is Null!",__FILE__);
        return;
    }
    m_stStream.pstPack = (VENC_PACK_S *)malloc(sizeof(VENC_PACK_S) * stState.u32CurPacks);
    if(NULL == m_stStream.pstPack){
        qDebug("malloc stream pack failed!");
        return;
    }
    m_stStream.u32PackCount = stState.u32CurPacks;
    s32Ret = HI_MPI_VENC_GetStream(m_venChn,&m_stStream,200);
    if(HI_SUCCESS != s32Ret){
        free(m_stStream.pstPack);
        m_stStream.pstPack = NULL;
        qDebug("HI_MPI_VENC_GetStream failed with %#x!",s32Ret);
        return;
    }

    for(HI_U32 i = 0; i < m_stStream.u32PackCount; i++)
    {
       size += m_stStream.pstPack[i].u32Len - m_stStream.pstPack[i].u32Offset;
    }

    char *pbuf = (char *)malloc(size);//bufferData(NULL,size);
    for(HI_U32 i=0;i<m_stStream.u32PackCount;i++)//将数据装入buf
    {
       memcpy(pbuf+buf_offset,m_stStream.pstPack[i].pu8Addr + m_stStream.pstPack[i].u32Offset,m_stStream.pstPack[i].u32Len - m_stStream.pstPack[i].u32Offset);
       buf_offset+=m_stStream.pstPack[i].u32Len - m_stStream.pstPack[i].u32Offset;
    }

    pkt.data = pbuf;
    pkt.len = size;
    pkt.dts = 0;
    pkt.pts = pts;
    pkt.sender = this;
    //qDebug()<<tr("venc chn read %1 bytes.").arg(size);
    emit(newPacketV(pkt));
    pts += 3000;
    s32Ret = HI_MPI_VENC_ReleaseStream(m_venChn,&m_stStream);
    free(m_stStream.pstPack);
    free(pbuf);
    m_stStream.pstPack = NULL;
}
