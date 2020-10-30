#ifndef HINKOUTPUTALSA_H
#define HINKOUTPUTALSA_H
#include <HinkObject.h>
#include "AlsaPlayback.h"

class HinkOutputAlsa : public HinkObject
{
    Q_OBJECT
public:
    HinkOutputAlsa(QObject *parent = 0);
    ~HinkOutputAlsa();
private:
    //ALSAPlaybackParameters m_param;
    std::list<snd_pcm_format_t> m_audioFmtList;
    ALSAPlayback *m_playback;
private slots:
    void onStart();
public slots:
    void onNewPacketA(Packet);

};

#endif // HINKOUTPUTALSA_H
