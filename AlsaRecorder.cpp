#include "AlsaRecorder.h"
#include <cstring>
#include <QDebug>

AlsaRecorder::AlsaRecorder(unsigned int card,unsigned int device,uint32_t samplerate, uint8_t channels, char *mySoundBuffer)
{
   this->m_card = card;
    this->m_device = device;
    this->m_sampleRate =samplerate;
    this->m_channels = channels;
    m_captureEnabled = false;
    this->m_soundBuffer = mySoundBuffer;
    m_buf = nullptr; //new int16_t[m_recordSize * m_channels];
}

AlsaRecorder::~AlsaRecorder()
{
    if(m_buf != nullptr)
        delete[] m_buf;
    m_captureEnabled = false;
    // Wait until run(); has finished
    wait();
}

void AlsaRecorder::stop()
{
    m_captureEnabled = false;
}

bool AlsaRecorder::initSound()
{
#if 1
    unsigned int card = m_card;
    unsigned int device = m_device;
    int flags = PCM_IN;
    unsigned int size;
    unsigned int frames_read;
    unsigned int total_frames_read;
    unsigned int bytes_per_frame;
    struct pcm_config config;
    //memset(&config,0,sizeof (config));
//        const struct pcm_config config = {
//            .channels = 1,
//            .rate = 16000,
//            .format = PCM_FORMAT_S16_LE,
//            .period_size = 1024,
//            .period_count = 2,
//            .start_threshold = 1024,
//            .silence_threshold = 1024 * 2,
//            .stop_threshold = 1024 * 2
//    };
     config.channels = m_channels;
     config.rate = m_sampleRate;
     config.format = PCM_FORMAT_S16_LE;// 16bits little end
     config.period_size = 80;
     config.period_count = 4;
     config.start_threshold = 0;//1024;
     config.silence_threshold = 0;//1024 * 2;
     config.stop_threshold = 0;//1024 * 2;

     m_pcm = pcm_open(card,device,PCM_IN,&config);
     if(!m_pcm || !pcm_is_ready(m_pcm)){
         qWarning("Unable to open PCM device (%s).",pcm_get_error(m_pcm));
         return false;
     }
     size = pcm_frames_to_bytes(m_pcm,pcm_get_buffer_size(m_pcm));
     m_buf = new char[size];
     if(!m_buf){
         qWarning("Unable to allocate %u bytes.",size);
         pcm_close(m_pcm);
         return false;
     }
     qDebug("Capturing sample: %u ch, %u Hz, %u bit. frame size/record size: %u/%u.",config.channels,config.rate,pcm_format_to_bits(config.format),pcm_frames_to_bytes(m_pcm,1),size);

     //bytes_per_frame = pcm_frames_to_bytes(m_pcm,1);
#endif
     return true;
}

void AlsaRecorder::closeSound()
{
    pcm_close(m_pcm);
}

void AlsaRecorder::run()
{
    int frame_read = 0;
    //unsigned int bytes_per_frame = pcm_frames_to_bytes(m_pcm,1);
    // Stop will set this to false
    m_captureEnabled = true;
    //while(1){
    //    qDebug()<<"run.....";
    //    sleep(1);
    //}
    if(initSound()){

        unsigned int bytes_per_frame = pcm_frames_to_bytes(m_pcm,1);
        unsigned int buf_size = pcm_get_buffer_size(m_pcm);
        //int frame_want_read = 320/ bytes_per_frame;
        while(m_captureEnabled){
           //frame_read = pcm_readi(m_pcm,m_buf,pcm_get_buffer_size(m_pcm));
           frame_read = pcm_readi(m_pcm,m_soundBuffer,buf_size);

           //qDebug("pcm read [frame_read*bytes_per_frame]/%d*%d = %d bytes.",frame_read,bytes_per_frame,frame_read*bytes_per_frame);
           //for(uint16_t i = 0; i < frame_read * bytes_per_frame; i++){
           //    m_soundBuffer->write(i,m_buf[i]);
           //}
            //qDebug("pcm_readi %u bytes data.",frame_read);
           //memcpy(m_soundBuffer,m_buf,frame_read*bytes_per_frame);
           emit newDataIsReady(frame_read *bytes_per_frame);
        }
    }

    closeSound();
}
