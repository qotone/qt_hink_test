#include "HinkInputAi.h"
#include "Hink.h"
#include "hi_comm_ai.h"
#include "mpi_ai.h"
#include "Tlv320.h"

HinkInputAi::HinkInputAi(QObject *parent):HinkObject(parent)
{
    data["interface"] = "";
    data["samplerate"] = 48000;
    data["resamplerate"] = -1;
    data["mode"] = "i2ss";
    data["channels"] = 1;
    data["chnCnt"] = 2;
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

void HinkInputAi::onStart()
{

    HI_S32 s32Ret = HI_SUCCESS;
    AUDIO_DEV aiDev;
    AI_CHN aiChn;
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
            stAioAttr.u32PtNumPerFrm = 320;
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

            infoSelfA.type = StreamInfo::Raw;
            infoSelfA.info["modId"] = HI_ID_AI;
            infoSelfA.info["devId"] = aiDev;
            infoSelfA.info["chnId"] = aiChn;
        }
    }

}

void HinkInputAi::onSetData(QVariantMap map)
{

}
