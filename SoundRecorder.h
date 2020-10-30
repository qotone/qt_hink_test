#ifndef SOUNDRECORDER_H
#define SOUNDRECORDER_H

#include <QThread>

class SoundRecorder : public QThread
{
    Q_OBJECT
public:
    explicit SoundRecorder();
    virtual ~SoundRecorder();
    virtual void stop() = 0;

signals:
    void newDataIsReady(int);

};

#endif // SOUNDRECORDER_H
