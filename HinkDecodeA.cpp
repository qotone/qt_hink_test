#include "HinkDecodeA.h"
#include "HinkObject.h"
#include "Hink.h"

HinkDecodeA::HinkDecodeA(QObject *parent) :HinkObject(parent)
{

    m_chn = 0;
    m_stAdecAttr.u32BufSize = 20;
    m_stAdecAttr.enMode = ADEC_MODE_STREAM;
    m_stAdecAttr.enType = PT_LPCM;

    infoSelfA.type = StreamInfo::VPSS;
    infoSelfA.info["modId"] = HI_ID_ADEC;
    infoSelfA.info["devId"] = 0;
    infoSelfA.info["chnId"] = m_chn;
}

void HinkDecodeA::onStart()
{
    ADEC_ATTR_ADPCM_S stAdpcm;
    HI_S32 s32Ret;
        ADEC_ATTR_G711_S stAdecG711;
        ADEC_ATTR_G726_S stAdecG726;
        ADEC_ATTR_LPCM_S stAdecLpcm;
        //ADEC_ATTR_AAC_S stAdecAac;

        //stAdecAttr.enType = enType;
        //stAdecAttr.u32BufSize = 20;
        //stAdecAttr.enMode = ADEC_MODE_STREAM;/* propose use pack mode in your app */

        if (PT_ADPCMA == m_stAdecAttr.enType)
        {
            m_stAdecAttr.pValue = &stAdpcm;
            stAdpcm.enADPCMType = ADPCM_TYPE_DVI4;//AUDIO_ADPCM_TYPE ;
        }
        else if (PT_G711A == m_stAdecAttr.enType || PT_G711U == m_stAdecAttr.enType)
        {
            m_stAdecAttr.pValue = &stAdecG711;
        }
        else if (PT_G726 == m_stAdecAttr.enType)
        {
            m_stAdecAttr.pValue = &stAdecG726;
            stAdecG726.enG726bps = MEDIA_G726_40K;//G726_BPS ;
        }
        else if (PT_LPCM == m_stAdecAttr.enType)
        {
            m_stAdecAttr.pValue = &stAdecLpcm;
            m_stAdecAttr.enMode = ADEC_MODE_PACK;/* lpcm must use pack mode */
        }
        //else if (PT_AAC == stAdecAttr.enType)
        //{
        //    m_stAdecAttr.pValue = &stAdecAac;
        //    m_stAdecAttr.enMode = ADEC_MODE_STREAM;   /* aac should be stream mode */
        //    m_stAdecAac.enTransType = gs_enAacTransType;
        //}
        else
        {
            qWarning("%s: invalid aenc payload type:%d\n", __FUNCTION__, m_stAdecAttr.enType);
        }

        /* create adec chn*/
        s32Ret = HI_MPI_ADEC_CreateChn(m_chn, &m_stAdecAttr);
}

void HinkDecodeA::onNewPacketA(HinkObject::Packet pkt)
{
    //qDebug()<<"packa recved:"<<pkt.len;
    AUDIO_STREAM_S stAudioStream;
    HI_S32 s32Ret;
    stAudioStream.u32Len = pkt.len;
    stAudioStream.pStream = pkt.data;
    qDebug("dec read %u bytes.",pkt.len);
    s32Ret = HI_MPI_ADEC_SendStream(m_chn,&stAudioStream,HI_TRUE);
    if(HI_SUCCESS != s32Ret){
        qDebug("%s: HI_MPI_ADEC_SendStream(%d) failed with %#x!",__FUNCTION__,m_chn,s32Ret);
    }
}

void HinkDecodeA::onSetData(QVariantMap map)
{

}

void HinkDecodeA::onNewInfoA(StreamInfo info)
{

    //switch(info){
    //case StreamInfo::PCMA:
    //case StreamInfo::PCMU:
    //    break;
    //case StreamInfo::AAC:
    //    break;
    //}
}
