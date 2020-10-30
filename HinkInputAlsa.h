#ifndef HINKINPUTALSA_H
#define HINKINPUTALSA_H
#include <HinkObject.h>
#include "ALSACapture.h"
#include <QSocketNotifier>

class HinkInputAlsa : public HinkObject
{
    Q_OBJECT
public:
    HinkInputAlsa(QObject *parent = 0);
    ~HinkInputAlsa();

public slots:
    void onReadCamData(void);
private:
    std::list<snd_pcm_format_t> m_audioFmtList;
    ALSACapture *m_capture;
    QSocketNotifier *notifier;
    qint64 m_pkt;
    qint64 m_time;
private slots:
        void onStart();

};

#endif // HINKINPUTALSA_H
