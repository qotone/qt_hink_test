#ifndef ALSARECORDER_H
#define ALSARECORDER_H
#include "SoundRecorder.h"
//#include "SoundBuffer.h"
#include <tinyalsa/pcm.h>

class AlsaRecorder : public SoundRecorder
{
    Q_OBJECT
public:
    //AlsaRecorder();
    //AlsaRecorder(uint32_t samplerate, uint8_t channels, char *mySoundBuffer,uint16_t recordsize);
    AlsaRecorder(unsigned int card,unsigned int device,uint32_t samplerate, uint8_t channels, char *mySoundBuffer);
    virtual ~AlsaRecorder();
    virtual void run();
    virtual void stop();

private:
    struct pcm *m_pcm;
    uint16_t m_recordSize;
    uint32_t m_sampleRate;
    bool m_captureEnabled;
    uint8_t m_channels;
    char *m_buf;
    uint m_card;
    uint m_device;
    char *m_soundBuffer;

private:
    bool initSound();
    void closeSound();
};

#endif // ALSARECORDER_H
