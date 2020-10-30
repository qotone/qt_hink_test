#ifndef HINKINPUTTINYALSA_H
#define HINKINPUTTINYALSA_H

#include "HinkObject.h"
//#include <tinyalsa/pcm.h>
#include "SoundRecorder.h"
#include <QSocketNotifier>

class HinkInputTinyalsa : public HinkObject
{
    Q_OBJECT
public:
    HinkInputTinyalsa(QObject *parent = 0);

private slots:
    void onStart();
    void onSetData(QVariantMap);
public slots:
    void onReadData(int);
    void onNewPacketA(Packet);

private:
    QSocketNotifier *notifier;
    struct pcm *m_pcm;
    char *m_buf;
    SoundRecorder *m_alsa;
};

#endif // HINKINPUTTINYALSA_H
