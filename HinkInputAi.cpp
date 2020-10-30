#include "HinkInputAi.h"
#include "Hink.h"
#include "Tlv320.h"

HinkInputAi::HinkInputAi(QObject *parent):HinkObject(parent),m_fd(-1),m_buf(NULL)
{
    data["interface"] = "";
    data["samplerate"] = 48000;
    data["resamplerate"] = -1;
    data["mode"] = "i2ss";
    data["channels"] = 1;
    data["chnCnt"] = 2;
    data["ptNumPerFrm"] = 85;//256;//320; ----->//16K//48K//hisi origal ,this value is equal to the alsa Period_size
    data["clkSel"] = true;
    data["buf"] = 5;
    data["delay"] = 0;
    data["delay2"] = 0;
    data["time"] = 0;
    data["anr"] = 0;
    data["did"] = 0;
    data["cid"] = 0;
    data["cid2"] = 0;
    data["aecdid"] = 0;
    data["aeccid"] = 0;
}

HinkInputAi::~HinkInputAi()
{
    qDebug()<<"HinkInputAi Destruct.";

    HI_MPI_AI_DisableChn(this->aiDev,this->aiChn);
    HI_MPI_AI_Disable(this->aiDev);
    if(m_fd != -1){
        //this->notifier->disconnect()
        delete this->notifier;
    }

    if(m_buf!=NULL){
        delete [] m_buf;
    }

}

void HinkInputAi::onStart()
{

    HI_S32 s32Ret = HI_SUCCESS;
    AIO_ATTR_S stAioAttr;



    if(!(data["interface"].toString().isEmpty())){
        //qDebug()<<Hink::getConfig()["interfaceV"].toMap()[data["interface"].toString()].toMap();
        if(!(Hink::getConfig()["interfaceA"].toMap()[data["interface"].toString()].toMap().isEmpty())){
            QVariantMap interfaceA = Hink::getConfig()["interfaceA"].toMap()[data["interface"].toString()].toMap();
            AUDIO_SAMPLE_RATE_E audioSample;
            data["did"] = interfaceA["did"].toInt();
            data["cid"] = interfaceA["cid"].toInt();
            data["samplerate"] = interfaceA["samplerate"].toInt();
            data["channels"] = interfaceA["channels"].toInt();
            data["chnCnt"] = interfaceA["chnCnt"].toInt();


            if(data["interface"].toString() == "Mini-In"){ // tlv320

              if(interfaceA["chip"].toString() == "tlv32"){
                  data["mode"] = interfaceA["mode"].toString();
                  // config tlv32
                  if(data["mode"].toString() == "i2ss"){
                      Tlv320::init(true,data["samplerate"].toUInt());
                  }else{

                      Tlv320::init(false,data["samplerate"].toUInt());
                  }
              }else{
                  qDebug()<<"Not support chip :"<<interfaceA["chip"].toString();
              }
            }else if(data["interface"].toString().startsWith("HDMI-")){
            //	data["mode"] = interfaceA["mode"].toString();
            //	data["clkSel"] = interfaceA["clkSel"].toBool();
            }


            // init audio attritudes

            audioSample = (AUDIO_SAMPLE_RATE_E)data["samplerate"].toInt();
            aiDev = data["did"].toInt();
            aiChn = data["cid"].toInt();
            HI_MPI_AI_GetPubAttr(aiDev,&stAioAttr);
            stAioAttr.enSamplerate = audioSample;//AUDIO_SAMPLE_RATE_8000;
            stAioAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
            if(data["mode"].toString() == "i2ss"){
                stAioAttr.enWorkmode = AIO_MODE_I2S_SLAVE;
            }else if(data["mode"].toString() == "i2sm"){
                stAioAttr.enWorkmode = AIO_MODE_I2S_MASTER;
            }else{
                qDebug()<<"wrong config for audio input.";
                return;
            }
            if(data["channels"].toInt() == 1 )
                stAioAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
            else
                stAioAttr.enSoundmode = AUDIO_SOUND_MODE_STEREO;
            stAioAttr.u32EXFlag = 1;
            stAioAttr.u32FrmNum = 30;
            stAioAttr.u32PtNumPerFrm = data["ptNumPerFrm"].toUInt();//320;
            stAioAttr.u32ChnCnt = data["chnCnt"].toUInt();
            stAioAttr.u32ClkChnCnt = stAioAttr.u32ChnCnt;
            if(data["clkSel"].toBool())
                stAioAttr.u32ClkSel = 1;
            else
                stAioAttr.u32ClkSel = 0;

            s32Ret = HI_MPI_AI_SetPubAttr(aiDev,&stAioAttr);
            if(HI_SUCCESS != s32Ret){

            }
            s32Ret = HI_MPI_AI_Enable(aiDev);
            if(HI_SUCCESS != s32Ret){

            }
            s32Ret = HI_MPI_AI_EnableChn(aiDev,aiChn);

            if(HI_SUCCESS != s32Ret){

            }

#if 0
            infoSelfA.type = StreamInfo::VPSS;
            infoSelfA.info["modId"] = HI_ID_AI;
            infoSelfA.info["devId"] = aiDev;
            infoSelfA.info["chnId"] = aiChn;
#else
            {

               infoSelfA.type = StreamInfo::Raw;
               AI_CHN_PARAM_S stAiChnPara;
                s32Ret = HI_MPI_AI_GetChnParam(aiDev,aiChn,&stAiChnPara);

               if(HI_SUCCESS != s32Ret){

               }
               stAiChnPara.u32UsrFrmDepth = 30;
               s32Ret = HI_MPI_AI_SetChnParam(aiDev, aiChn, &stAiChnPara);
               if(HI_SUCCESS != s32Ret){

               }

               m_fd = HI_MPI_AI_GetFd(aiDev,aiChn);
               m_buf = new int8_t[data["ptNumPerFrm"].toUInt()*2*2];
               //qDebug()<<"begin...m_fd = "<<m_fd;
               this->notifier = new QSocketNotifier(m_fd,QSocketNotifier::Read,this);
               //qDebug()<<"new ...";
               connect(this->notifier,SIGNAL(activated(int)),this,SLOT(onReadData()));

               //qDebug()<<"connect...";

            }
#endif
        }
    }

}

void HinkInputAi::onSetData(QVariantMap map)
{

}

void HinkInputAi::onReadData()
{
    HinkObject::Packet pkt;
    HI_S32 s32Ret;
    AUDIO_FRAME_S stFrame;
    s32Ret = HI_MPI_AI_GetFrame(aiDev,aiChn, &stFrame, NULL, HI_FALSE);
    if (HI_SUCCESS != s32Ret ){
        qDebug("%s error!",__FUNCTION__);
        return;
    }
    //qDebug()<<"ai read ....."<<stFrame.u32Len<<" betyes.";
#if 1
    for (uint32_t i = 0; i < stFrame.u32Len; i+=2){
        memcpy(m_buf+2*i,(void *)(stFrame.pVirAddr[0] + i), 2);
        memcpy(m_buf+2*(i+1),(void *)(stFrame.pVirAddr[1] + i), 2);
    }
#endif
    //memcpy(m_buf,stFrame.pVirAddr[0],stFrame.u32Len);
    //memcpy(m_buf+stFrame.u32Len,stFrame.pVirAddr[1],stFrame.u32Len);
    pkt.data = m_buf;
    pkt.len = stFrame.u32Len * 2;
    pkt.pts = stFrame.u64TimeStamp;
    pkt.dts = 0;
    pkt.sender = this;
    emit(newPacketA(pkt));

    /* finally you must release the stream */
    s32Ret = HI_MPI_AI_ReleaseFrame(aiDev, aiChn, &stFrame, NULL);
    if (HI_SUCCESS != s32Ret )
    {
         qDebug("%s: HI_MPI_AI_ReleaseFrame(%d, %d), failed with %#x!\n",\
                __FUNCTION__, aiDev, aiChn, s32Ret);
    }

}
