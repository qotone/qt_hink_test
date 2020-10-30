#include "HinkEncodeA.h"
#include "Hink.h"
#include "hi_comm_vb.h"
#include "mpi_vb.h"
#include "mpi_sys.h"
#include "audio_aac_adp.h"

#include <QTime>

static AAC_TYPE_E 	gs_enAacType = AAC_TYPE_AACLC;
static AAC_BPS_E 	gs_enAacBps  = AAC_BPS_48K;
static AAC_TRANS_TYPE_E gs_enAacTransType = AAC_TRANS_TYPE_ADTS;


HinkEncodeA::HinkEncodeA(QObject *parent):HinkObject(parent)
{

    m_chn = 0;

    m_stAencAttr.enType = PT_AAC;//PT_G711A;//PT_G726;
    m_stAencAttr.u32BufSize = 100;
    if(m_stAencAttr.enType == PT_AAC)
        m_stAencAttr.u32PtNumPerFrm = 1024;
    else
        m_stAencAttr.u32PtNumPerFrm = 320;

    infoSelfA.type = StreamInfo::Raw;
    infoSelfA.info["modId"] = HI_ID_AENC;
    infoSelfA.info["devId"] = 0;
    infoSelfA.info["chnId"] = m_chn;
    m_seq = 0;
    m_pkt = 0;
    m_lastTime = 0;
}

HinkEncodeA::~HinkEncodeA()
{
    for (int i=0;i<AENC_MAX_CHN_NUM;i++)
    {
         HI_MPI_AENC_DestroyChn(i);
    }

    for (int i=0;i<ADEC_MAX_CHN_NUM;i++)
    {
         HI_MPI_ADEC_ClearChnBuf(i);
         HI_MPI_ADEC_DestroyChn(i);
    }
    qDebug()<<"HinkEncodeA destruct....";
}

void HinkEncodeA::onStart()
{
        HI_S32 s32Ret;
        HI_S32 encFd;
        AENC_ATTR_ADPCM_S stAdpcmAenc;
        AENC_ATTR_G711_S stAencG711;
        AENC_ATTR_G726_S stAencG726;
        AENC_ATTR_LPCM_S stAencLpcm;
        AENC_ATTR_AAC_S stAencAac;


        /* set AENC chn attr */
        s32Ret = HI_MPI_AENC_AacInit();
            if(HI_SUCCESS != s32Ret)
            {
                qDebug("%s: aac aenc init failed with %d!\n", __FUNCTION__, s32Ret);
               // return HI_FAILURE;
            }
            s32Ret = HI_MPI_ADEC_AacInit();
            if(HI_SUCCESS != s32Ret)
            {
                qDebug("%s: aac adec init failed with %d!\n", __FUNCTION__, s32Ret);
                //return HI_FAILURE;
            }


        if (PT_ADPCMA == m_stAencAttr.enType)
        {
            m_stAencAttr.pValue       = &stAdpcmAenc;
            stAdpcmAenc.enADPCMType = ADPCM_TYPE_DVI4;
        }
        else if (PT_G711A == m_stAencAttr.enType || PT_G711U == m_stAencAttr.enType)
        {
            m_stAencAttr.pValue       = &stAencG711;
        }
        else if (PT_G726 == m_stAencAttr.enType)
        {
            m_stAencAttr.pValue       = &stAencG726;
            stAencG726.enG726bps    = MEDIA_G726_16K;
        }
        else if (PT_LPCM == m_stAencAttr.enType)
        {
            m_stAencAttr.pValue = &stAencLpcm;
        }
        else if (PT_AAC== m_stAencAttr.enType)
        {

            m_stAencAttr.pValue = &stAencAac;
            stAencAac.enAACType = gs_enAacType;
            stAencAac.enBitRate = gs_enAacBps;
            stAencAac.enBitWidth = AUDIO_BIT_WIDTH_16;
            stAencAac.enSmpRate = AUDIO_SAMPLE_RATE_48000;
            stAencAac.enSoundMode = AUDIO_SOUND_MODE_STEREO;
            stAencAac.enTransType = gs_enAacTransType;
            stAencAac.s16BandWidth = 0;
        }
        else
        {
            qWarning("%s: invalid aenc payload type:%d\n", __FUNCTION__, m_stAencAttr.enType);
        }

        /* create aenc chn*/
        s32Ret = HI_MPI_AENC_CreateChn(m_chn, &m_stAencAttr);
        if (HI_SUCCESS != s32Ret)
        {
            qWarning("%s: HI_MPI_AENC_CreateChn(%d) failed with %#x!\n", __FUNCTION__,
                  m_chn, s32Ret);
        }
        encFd = HI_MPI_AENC_GetFd(m_chn);
        this->notifier = new QSocketNotifier(encFd,QSocketNotifier::Read,this);
        connect(this->notifier,SIGNAL(activated(int)),this,SLOT(onReadData()));

}

void HinkEncodeA::onReadData()
{

    HinkObject::Packet pkt;
    HI_S32 s32Ret;
    AUDIO_STREAM_S stStream;
    qint64 timeT = QDateTime::currentMSecsSinceEpoch();   //获取当前时间
    /* get stream from aenc chn */
    s32Ret = HI_MPI_AENC_GetStream(m_chn, &stStream, HI_FALSE);
    if (HI_SUCCESS != s32Ret )
    {
          qWarning("%s: HI_MPI_AENC_GetStream(%d), failed with %#x!\n",__FUNCTION__, m_chn, s32Ret);
    }

    if(m_stAencAttr.enType == PT_AAC){

        pkt.data = stStream.pStream;
        pkt.len = stStream.u32Len;

    }else{
        pkt.data = stStream.pStream + 4;
        pkt.len = stStream.u32Len - 4;
    }

    pkt.dts = 0;
#if 0
    pkt.pts = stStream.u64TimeStamp;
#else

    if(m_stAencAttr.enType == PT_AAC){

        if(m_lastTime == 0){
            m_lastTime = timeT;
         }
        m_pkt += (timeT - m_lastTime)* (48000/1000);//48 * 20;//pkt.len;
        m_lastTime = timeT;
    }else{
        m_pkt += pkt.len;
    }
    pkt.pts = m_pkt;
     //pkt.pts = timeT;
#endif
    pkt.sender = this;
    //qDebug()<<tr("[T] %1,audio encoder read %2 bytes.").arg(timeT).arg(pkt.len);
    emit(newPacketA(pkt));
    /* finally you must release the stream */
    s32Ret = HI_MPI_AENC_ReleaseStream(m_chn, &stStream);
    if (HI_SUCCESS != s32Ret )
    {
        qDebug("%s: HI_MPI_AENC_ReleaseStream(%d), failed with %#x!\n",__FUNCTION__, m_chn, s32Ret);
    }
}

void HinkEncodeA::onNewPacketA(HinkObject::Packet pkt)
{
    HI_S32 s32Ret;
    AUDIO_FRAME_S stFrame;

    HI_U32 vbSize = pkt.len;
    VB_BLK handleVB = VB_INVALID_HANDLE;
    VB_POOL poolID;
    HI_U32 phyAddr,*pVirAddr;

    qint64 timeT = QDateTime::currentMSecsSinceEpoch();   //获取当前时间
    do{
        handleVB = HI_MPI_VB_GetBlock(VB_INVALID_POOLID,vbSize,NULL);
    }while(VB_INVALID_HANDLE == handleVB);

    poolID = HI_MPI_VB_Handle2PoolId(handleVB);
    phyAddr = HI_MPI_VB_Handle2PhysAddr(handleVB);
    pVirAddr = (HI_U32 *)HI_MPI_SYS_Mmap(phyAddr,vbSize);
    memcpy(pVirAddr,pkt.data,pkt.len);
    stFrame.enBitwidth  = AUDIO_BIT_WIDTH_16;
    stFrame.enSoundmode = AUDIO_SOUND_MODE_MONO;
    stFrame.pVirAddr[0] = pVirAddr;
    //stFrame.pVirAddr[1] = m_pVirAddr;
    stFrame.u32PhyAddr[0] = phyAddr;
    //stFrame.u32PhyAddr[1] = m_phyAddr;
    stFrame.u64TimeStamp = timeT;//pkt.pts;
    stFrame.u32Seq = m_seq++;
    stFrame.u32Len = pkt.len;
    stFrame.u32PoolId[0] = poolID;
    s32Ret = HI_MPI_AENC_SendFrame(m_chn, &stFrame, NULL);
    //s32Ret = HI_MPI_AO_SendFrame(aoDev, aoChn, &stFrame, 1000);
    if (HI_SUCCESS != s32Ret )
    {
         qWarning("%s: HI_MPI_AO_SendFrame, failed with %#x!",__FUNCTION__,s32Ret);
    }

    HI_MPI_SYS_Munmap(pVirAddr,vbSize);
    HI_MPI_VB_ReleaseBlock(handleVB);
}

void HinkEncodeA::onSetData(QVariantMap map)
{

}


void HinkEncodeA::onNewInfoA(StreamInfo info)
{

}
