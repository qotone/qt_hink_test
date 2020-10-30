#include "HinkOutputAlsa.h"

HinkOutputAlsa::HinkOutputAlsa(QObject* parent):HinkObject(parent)
{
    m_audioFmtList.push_back(SND_PCM_FORMAT_S16_LE);
    // samplerate convert :https://www.cnblogs.com/eleclsc/p/10790108.html
    ALSAPlaybackParameters param("rate_convert",m_audioFmtList,16000,2,1); // rate_convert see the share/alsa.conf
    //ALSAPlaybackParameters param("hw:0,0",m_audioFmtList,48000,2,1);
    m_playback = ALSAPlayback::createNew(param);
}

HinkOutputAlsa::~HinkOutputAlsa()
{

    //m_playback->close();
    delete m_playback;
}

void HinkOutputAlsa::onStart()
{

}


void HinkOutputAlsa::onNewPacketA(HinkObject::Packet pkt)
{


   m_playback->write(pkt.data,pkt.len);
   // m_playback->write(buf,128);
}
