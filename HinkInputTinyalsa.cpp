#include "HinkInputTinyalsa.h"
#include "Hink.h"
#include <QDebug>
#include <cstring>
#include "AlsaRecorder.h"
HinkInputTinyalsa::HinkInputTinyalsa(QObject *parent) : HinkObject(parent)
{

    m_pcm = NULL;
    m_buf = NULL;
    data["samplerate"] = 48000;
    data["channels"] = 1;
    data["card"] = 0;
    data["device"] = 0;
    data["memSize"] = 4096 * 5;
}

void HinkInputTinyalsa::onReadData(int size)
{
    HinkObject::Packet pkt;
    //int size = 640;

    //if(size == -2)
    //    return;

    pkt.data = m_buf;
    pkt.len = size;
    pkt.dts = 0;
    pkt.pts = 0;
    pkt.sender = this;
    //qDebug()<<tr("usbcam audio read %1 bytes.").arg(size);
    emit(newPacketA(pkt));

}
void HinkInputTinyalsa::onStart()
{
    unsigned int card = data["card"].toUInt();
    unsigned int device = data["device"].toUInt();
    unsigned int rate = data["samplerate"].toUInt();
    unsigned int chns = data["channels"].toUInt();
    memInit(data["memSize"].toUInt());
    qDebug()<<"memSize ="<<data["memSize"].toInt();
    m_buf = bufferData(NULL,2560);
    //m_alsa = new AlsaRecorder(card,device,rate,chns,m_buf);
    //m_alsa = new AlsaRecorder(0,0,16000,1,m_buf);
    m_alsa = new AlsaRecorder(0,0,48000,1,m_buf);
    //connect(m_alsa,SIGNAL(newDataIsReady),this,SLOT(onReadData));
    m_alsa->start();

    infoSelfA.type = StreamInfo::Raw;
    connect(m_alsa,SIGNAL(newDataIsReady(int)),this,SLOT(onReadData(int)));
    //int flags = PCM_IN;
    //unsigned int size;
    //unsigned int frames_read;
    //unsigned int total_frames_read;
    //unsigned int bytes_per_frame;
    //struct pcm_config config;
    //memset(&config,0,sizeof (config));
//  //      const struct pcm_config config = {
//  //          .channels = 1,
//  //          .rate = 16000,
//  //          .format = PCM_FORMAT_S16_LE,
//  //          .period_size = 1024,
//  //          .period_count = 2,
//  //          .start_threshold = 1024,
//  //          .silence_threshold = 1024 * 2,
//  //          .stop_threshold = 1024 * 2
//  //  };
    // config.channels = data["channels"].toUInt();
    // config.rate = data["samplerate"].toUInt();
    // config.format = PCM_FORMAT_S16_LE;// 16bits little end
    // config.period_size = 1024;
    // config.period_count = 2;
    // config.start_threshold = 1024;
    // config.silence_threshold = 1024 * 2;
    // config.stop_threshold = 1024 * 2;

    // m_pcm = pcm_open(card,device,PCM_IN,&config);
    // if(!m_pcm || !pcm_is_ready(m_pcm)){
    //     qDebug("Unable to open PCM device (%s).",pcm_get_error(m_pcm));
    //     return;
    // }
    // size = pcm_frames_to_bytes(m_pcm,pcm_get_buffer_size(m_pcm));
    // m_buf = malloc(size);
    // if(!m_buf){
    //     qDebug("Unable to allocate %u bytes.",size);
    //     pcm_close(m_pcm);
    //     return;
    // }
    // qDebug("Capturing sample: %u ch, %u Hz, %u bit.",config.channels,config.rate,pcm_format_to_bits(config.format));

    // bytes_per_frame = pcm_frames_to_bytes(m_pcm,1);

}

void HinkInputTinyalsa::onSetData(QVariantMap map)
{

}

void HinkInputTinyalsa::onNewPacketA(HinkObject::Packet pkt)
{

}
