#ifndef ALSAPLAYBACK_H
#define ALSAPLAYBACK_H

#include <list>
#include <cstddef>
#include <alsa/asoundlib.h>
#include <logger.h>

struct ALSAPlaybackParameters
{

    ALSAPlaybackParameters(const char* devname, const std::list<snd_pcm_format_t> & formatList, unsigned int sampleRate, unsigned int channels, int verbose) :
        m_devName(devname), m_formatList(formatList), m_sampleRate(sampleRate), m_channels(channels), m_verbose(verbose) {

    }

    std::string      m_devName;
    std::list<snd_pcm_format_t> m_formatList;
    unsigned int     m_sampleRate;
    unsigned int     m_channels;
    int              m_verbose;
};

class ALSAPlayback
{
public:
    static ALSAPlayback* createNew(const ALSAPlaybackParameters &params);
    virtual ~ALSAPlayback();
    void close();
protected:
    ALSAPlayback(const ALSAPlaybackParameters &params);
    int configureFormat(snd_pcm_hw_params_t *hw_params);

public:
    public:
        virtual size_t write(char* buffer, size_t bufferSize);
        virtual int getFd();

        virtual unsigned long getBufferSize() { return m_bufferSize; }
        virtual int getWidth()  {return -1;}
        virtual int getHeight() {return -1;}
        virtual int getCaptureFormat() {return -1;}

        unsigned long getSampleRate() { return m_params.m_sampleRate; }
        unsigned long getChannels  () { return m_params.m_channels;   }
        snd_pcm_format_t getFormat () { return m_fmt;                 }

    private:
        snd_pcm_t*            m_pcm;
        unsigned long         m_bufferSize;
        unsigned long         m_periodSize;
        ALSAPlaybackParameters m_params;
        snd_pcm_format_t      m_fmt;

};

#endif // ALSAPLAYBACK_H
