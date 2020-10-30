#include "HinkDecodeV.h"
#include "Hink.h"
#include <QDebug>
#include "hink_vdec.h"
#include "hink_vpss.h"
#include "hink_sys.h"
#include "hink_comm.h"
#include <QFile>

#include <cstdio> // for FILE
#include <cstring>
#include <cstdlib>

#define NOVIDEO_PATH "/hink/configs/nosignal.yuv"

HinkDecodeV::HinkDecodeV(QObject *parent): HinkObject(parent)
{
    data["width"] = 1920;
    data["height"] = 1080;
    data["block"] = true;//false;
    data["lowLatency"] = false;

    infoSelfV.type = StreamInfo::VPSS;
    infoSelfV.info["modId"] = HI_ID_VPSS;
    infoSelfV.info["devId"] = 0;
    infoSelfV.info["chnId"] = 0;

    m_vbBlk = VB_INVALID_POOLID;
    m_enabledUserPic = false;
    memset(&m_vFrameInfo,0,sizeof(VIDEO_FRAME_INFO_S));

}

HinkDecodeV::~HinkDecodeV()
{
    qDebug()<<"HinkDecodeV de struct ....";
    if(m_vbBlk != VB_INVALID_POOLID){
        HI_MPI_VB_ReleaseBlock(m_vbBlk);
    }
}

int HinkDecodeV::getVFrameFromYUV(QString file, uint32_t _w, uint32_t _h)
{
       HI_U32             u32LStride;
       HI_U32             u32CStride;
       HI_U32             u32LumaSize;
       HI_U32             u32ChrmSize;
       HI_U32             u32Size;
       HI_U32 u32PhyAddr = 0;
       HI_U8 *pVirAddr = NULL;
       FILE *pYUVFile;
       HI_S32 s32Ret;

       u32LStride  = _w;
       u32CStride  = _w;

       u32LumaSize = (u32LStride * _h);
       u32ChrmSize = (u32CStride * _h) >> 2;/* YUV 420 */
       u32Size = u32LumaSize + (u32ChrmSize << 1);

       /* alloc video buffer block ---------------------------------------------------------- */
       m_vbBlk = HI_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
       if (VB_INVALID_HANDLE == m_vbBlk)
       {
           qDebug("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);
           return -1;
       }
       u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(m_vbBlk);
       if (0 == u32PhyAddr)
       {
           return -1;
       }

       pVirAddr = (HI_U8 *) HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
       if (NULL == pVirAddr)
       {
           return -1;
       }

       m_vFrameInfo.u32PoolId = HI_MPI_VB_Handle2PoolId(m_vbBlk);
       if (VB_INVALID_POOLID == m_vFrameInfo.u32PoolId)
       {
           HI_MPI_SYS_Munmap(pVirAddr, u32Size);
           return -1;
       }
       qDebug("pool id :%d, phyAddr:%x,virAddr:%x\n" ,m_vFrameInfo.u32PoolId,u32PhyAddr,(int)pVirAddr);

       m_vFrameInfo.stVFrame.u32PhyAddr[0] = u32PhyAddr;
       m_vFrameInfo.stVFrame.u32PhyAddr[1] = m_vFrameInfo.stVFrame.u32PhyAddr[0] + u32LumaSize;
       m_vFrameInfo.stVFrame.u32PhyAddr[2] = m_vFrameInfo.stVFrame.u32PhyAddr[1] + u32ChrmSize;

       m_vFrameInfo.stVFrame.pVirAddr[0] = pVirAddr;
       m_vFrameInfo.stVFrame.pVirAddr[1] = m_vFrameInfo.stVFrame.pVirAddr[0] + u32LumaSize;
       m_vFrameInfo.stVFrame.pVirAddr[2] = m_vFrameInfo.stVFrame.pVirAddr[1] + u32ChrmSize;

       m_vFrameInfo.stVFrame.u32Width  = _w;
       m_vFrameInfo.stVFrame.u32Height = _h;
       m_vFrameInfo.stVFrame.u32Stride[0] = u32LStride;
       m_vFrameInfo.stVFrame.u32Stride[1] = u32CStride;
       m_vFrameInfo.stVFrame.u32Stride[2] = u32CStride;
       m_vFrameInfo.stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
       m_vFrameInfo.stVFrame.u32Field = VIDEO_FIELD_FRAME;//VIDEO_FIELD_INTERLACED;/* Intelaced D1,otherwise VIDEO_FIELD_FRAME */
       m_vFrameInfo.stVFrame.enVideoFormat = VIDEO_FORMAT_LINEAR;
       m_vFrameInfo.stVFrame.enCompressMode = COMPRESS_MODE_NONE;

       //  /* read Y U V data from file to the addr ----------------------------------------------*/

       HI_U8 *pY,*pU,*pV,*pDst;
       HI_U32 u32Row,stride,stride2;
       pYUVFile = fopen(file.toStdString().c_str(),"rb");
       if(!pYUVFile){
           qDebug("open YUV file %s failed.",file.toStdString().c_str());
           return -1;
       }
       pY = m_vFrameInfo.stVFrame.pVirAddr[0];
       pU = m_vFrameInfo.stVFrame.pVirAddr[1];
       pV = m_vFrameInfo.stVFrame.pVirAddr[2];
       stride = m_vFrameInfo.stVFrame.u32Stride[0];
       stride2 = m_vFrameInfo.stVFrame.u32Stride[1] >> 1;

       pDst = pY;
       for ( u32Row = 0; u32Row < _h; u32Row++ )
       {
           fread( pDst, _w, 1, pYUVFile );
           pDst += stride;
       }

       pDst = pU;
       for ( u32Row = 0; u32Row < _h/2; u32Row++ )
       {
           fread( pDst, _w/2, 1, pYUVFile );
           pDst += stride2;
       }

       pDst = pV;
       for ( u32Row = 0; u32Row < _h/2; u32Row++ )
       {
           fread( pDst, _w/2, 1, pYUVFile );
           pDst += stride2;
       }
 /* convert planar YUV420 to sem-planar YUV420 -----------------------------------------*/
      #if 1
       planToSemi(m_vFrameInfo.stVFrame.pVirAddr[0], m_vFrameInfo.stVFrame.u32Stride[0],
             m_vFrameInfo.stVFrame.pVirAddr[1], m_vFrameInfo.stVFrame.u32Stride[1],
             m_vFrameInfo.stVFrame.pVirAddr[2], m_vFrameInfo.stVFrame.u32Stride[1],
             m_vFrameInfo.stVFrame.u32Width, m_vFrameInfo.stVFrame.u32Height);
#endif
           HI_MPI_SYS_Munmap(pVirAddr, u32Size);

           fclose(pYUVFile);
    return 0;
}

    ///******************************************************************************
    //* function : Plan to Semi
    //******************************************************************************
int HinkDecodeV::planToSemi(uint8_t *pY, uint32_t yStride, uint8_t *pU, uint32_t uStride, uint8_t *pV, uint32_t vStride, uint32_t picWidth, uint32_t picHeight)
{
        HI_S32 i;
        HI_U8* pTmpU, *ptu;
        HI_U8* pTmpV, *ptv;
        HI_S32 s32HafW = uStride >>1 ;
        HI_S32 s32HafH = picHeight >>1 ;
        HI_S32 s32Size = s32HafW*s32HafH;

        if(!s32Size)
        {
            return -1;
        }
        pTmpU = malloc( s32Size ); ptu = pTmpU;
        pTmpV = malloc( s32Size ); ptv = pTmpV;

        if(NULL == pTmpU || NULL == pTmpV)
        {
            free( ptu );
            free( ptv );
            return -1;
        }

        memcpy(pTmpU,pU,s32Size);
        memcpy(pTmpV,pV,s32Size);

        for(i = 0;i<s32Size>>1;i++)
        {
            *pU++ = *pTmpV++;
            *pU++ = *pTmpU++;

        }
        for(i = 0;i<s32Size>>1;i++)
        {
            *pV++ = *pTmpV++;
            *pV++ = *pTmpU++;
        }

        free( ptu );
        free( ptv );

        return 0;


}

void HinkDecodeV::onStart()
{
    int ret = 0;
    HI_S32 s32Ret;
    VPSS_GRP_ATTR_S stVPSSAttr;
    VDEC_CHN_ATTR_S stVdecChnAttr;
    PAYLOAD_TYPE_E enType = PT_MJPEG;//PT_H264;
    SIZE_S stSize;
    stSize.u32Width = data["width"].toInt();
    stSize.u32Height = data["height"].toInt();
    hink_vdec_getDefChnAttr(&stVdecChnAttr, enType, &stSize);
    // added for stream mode
    //stVdecChnAttr.stVdecVideoAttr.enMode = VIDEO_MODE_STREAM;
    hink_vdec_start(0, &stVdecChnAttr,data["block"].toBool() ? VIDEO_DISPLAY_MODE_PLAYBACK : VIDEO_DISPLAY_MODE_PREVIEW, HINK_VBS_COMM);
#if 1
    // set novideo pic begin .....
    // https://www.ebaina.com/questions/100000009944
    // .................
    //ret = getVFrameFromYUV(NOVIDEO_PATH,704,576);
    ret = getVFrameFromYUV(NOVIDEO_PATH,1920,1080);
    if(ret == 0){
        s32Ret =HI_MPI_VDEC_DisableUserPic(0);
        if(HI_SUCCESS != s32Ret)
            qDebug("HI_MPI_VDEC_DisableUserPic error with 0x%x",s32Ret);
        else{

            s32Ret = HI_MPI_VDEC_SetUserPic(0,&m_vFrameInfo);
            if(HI_SUCCESS != s32Ret)
                qDebug("HI_MPI_VDEC_SetUserPic error with 0x%x",s32Ret);
            else
            {
                HI_MPI_VDEC_StopRecvStream(0);
                s32Ret = HI_MPI_VDEC_EnableUserPic(0,HI_TRUE);
                if(HI_SUCCESS != s32Ret)
                    qDebug("HI_MPI_VDEC_EnableUserPic error with 0x%x",s32Ret);
                else{
                    qDebug("Set user pic ok.");
                    m_enabledUserPic = true;
                }
            }
        }
    }
#endif
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
    HI_S32 s32Ret;
    memset(&stStream, 0, sizeof(VDEC_STREAM_S) );
    stStream.u64PTS = 0;//pkt.pts; // stream mode pts must be 0
    stStream.pu8Addr = (HI_U8 *)pkt.data;
    stStream.u32Len = pkt.len;
    stStream.bEndOfStream = HI_FALSE;
    stStream.bEndOfFrame = HI_TRUE;
    //qDebug()<<tr("vdec recv %1 bytes.").arg(pkt.len);
    if(m_enabledUserPic){
        s32Ret = HI_MPI_VDEC_DisableUserPic(0);
        if(HI_SUCCESS != s32Ret){
            qDebug("HI_MPI_VDEC_DisableUserPic failed with 0x%x.",s32Ret);
            return;
        }else{
            s32Ret = HI_MPI_VDEC_StartRecvStream(0);
            if(HI_SUCCESS != s32Ret){
                qDebug("HI_MPI_VDEC_StartRecvStream failed with 0x%x.",s32Ret);
                return;
            }else{

                m_enabledUserPic = true;
            }
        }

    }
    s32Ret = HI_MPI_VDEC_SendStream(0, &stStream, 0);
    if(HI_SUCCESS != s32Ret){
        qDebug("Vdec send stream error = 0x%x",s32Ret);
    }
}

void HinkDecodeV::onNewInfoV(HinkObject::StreamInfo info)
{

}


void HinkDecodeV::onSetData(QVariantMap data)
{
    //qDebug()<<;
}
