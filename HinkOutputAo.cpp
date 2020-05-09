#include "HinkOutputAo.h"
#include "hi_comm_ao.h"
#include "mpi_ao.h"
#include "Hink.h"


HinkOutputAo::HinkOutputAo(QObject *parent):HinkObject(parent)
{

    data["interface"] = "";
    data["samplerate"] = 48000;
    data["resamplerate"] = -1;
    data["mode"] = "i2ss";
    data["channels"] = 1;
    data["chnCnt"] = 2;
    data["clkSel"] = true;
    data["buf"] = 5;
    data["time"] = 0;
    data["did"] = 0;
    data["cid"] = 0;
}


void HinkOutputAo::onStart()
{


    HI_S32 s32Ret = HI_SUCCESS;
    AUDIO_DEV aoDev;
    AI_CHN aoChn;
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
            data["mode"] = interfaceA["mode"].toString();
            data["clkSel"] = interfaceA["clkSel"].toBool();


            //qDebug()<<interfaceA;
            if(data["interface"].toString() == "Mini-Out"){ // tlv320

                  data["mode"] = interfaceA["mode"].toString();
            }else {
                data["mode"] = "i2sm";
                data["clkSel"] = false;
            }


            // init audio attritudes

            audioSample = (AUDIO_SAMPLE_RATE_E)data["samplerate"].toInt();
            aoDev = data["did"].toInt();
            aoChn = data["cid"].toInt();
            //s32Ret =  HI_MPI_AO_GetPubAttr(aoDev,&stAioAttr);
            //if(HI_SUCCESS != s32Ret){
            //    qDebug("Error: Ao Get  dev = %d pub attr failed return 0x%x.",aoDev,s32Ret);
            //}
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

            s32Ret = HI_MPI_AO_SetPubAttr(aoDev,&stAioAttr);
            if(HI_SUCCESS != s32Ret){
                qDebug("Error: Ao set pub attr failed return 0x%x.",s32Ret);
            }
            s32Ret = HI_MPI_AO_Enable(aoDev);
            if(HI_SUCCESS != s32Ret){

                qDebug()<<"Error: Ao enabled failed.";
            }
            s32Ret = HI_MPI_AO_EnableChn(aoDev,aoChn);
            if(HI_SUCCESS != s32Ret){

                qDebug()<<"Error: Ao enabled  chn failed.";
            }

            // test stere chn
            s32Ret = HI_MPI_AO_EnableChn(aoDev,aoChn+1);
            if(HI_SUCCESS != s32Ret){

                qDebug()<<"Error: Ao enabled  chn failed.";
            }
            // end test

            infoSelfA.type = StreamInfo::Raw;
            infoSelfA.info["modId"] = HI_ID_AO;
            infoSelfA.info["devId"] = aoDev;
            infoSelfA.info["chnId"] = aoChn;
        }
    }

}

void HinkOutputAo::onSetData(QVariantMap map)
{

}

void HinkOutputAo::onNewPacketA(HinkObject::Packet pkt)
{
    qDebug()<<"pkt.pts = "<<pkt.pts;
    if(data.contains("bufLenV")){
        qDebug()<<data["bufLenV"].toInt();
    }

}
