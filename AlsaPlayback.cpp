#include "AlsaPlayback.h"
#include <QDebug>

ALSAPlayback* ALSAPlayback::createNew(const ALSAPlaybackParameters &params)
{
    ALSAPlayback* playback = new ALSAPlayback(params);
    if(playback){
        if(playback->getFd() == -1){
            delete playback;
            playback = NULL;
        }
    }
    return playback;
}

ALSAPlayback::~ALSAPlayback()
{
    qDebug()<<"call ALSAPlayback destruct.";
    this->close();
}

void ALSAPlayback::close()
{
    if (m_pcm != NULL)
    {
        snd_pcm_close (m_pcm);
        m_pcm = NULL;
    }
}


ALSAPlayback::ALSAPlayback(const ALSAPlaybackParameters & params):m_pcm(NULL),m_bufferSize(0),m_params(params)
{

    //LOG(NOTICE) << "Open ALSA device: \"" << params.m_devName << "\"";

    snd_pcm_hw_params_t *hw_params = NULL;
    int err = 0;

    // open PCM device
    if ((err = snd_pcm_open (&m_pcm, m_params.m_devName.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        LOG(ERROR) << "cannot open audio device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
    }

    // configure hw_params
    else if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        LOG(ERROR) << "cannot allocate hardware parameter structure device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }
    else if ((err = snd_pcm_hw_params_any (m_pcm, hw_params)) < 0) {
        LOG(ERROR) << "cannot initialize hardware parameter structure device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }
    else if ((err = snd_pcm_hw_params_set_access (m_pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        LOG(ERROR) << "cannot set access type device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }
    else if (this->configureFormat(hw_params) < 0) {
        this->close();
    }
    else if ((err = snd_pcm_hw_params_set_rate_near (m_pcm, hw_params, &m_params.m_sampleRate, 0)) < 0) {
        LOG(ERROR) << "cannot set sample rate device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }
    else if ((err = snd_pcm_hw_params_set_channels (m_pcm, hw_params, m_params.m_channels)) < 0) {
        LOG(ERROR) << "cannot set channel count device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
        //snd_pcm_hw_params_set_rate_resample(m_pcm,hw_params,1);
    }
    else if ((err = snd_pcm_hw_params_set_rate_resample( m_pcm, hw_params,1)) < 0) {
        //snd_pcm_hw_params_set_rate_resample(m_pcm,hw_params,1);
        LOG(ERROR) << "cannot set resample device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }
    else if ((err = snd_pcm_hw_params (m_pcm, hw_params)) < 0) {
        LOG(ERROR) << "cannot set parameters device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }


    // get buffer size
    else if ((err = snd_pcm_get_params(m_pcm, &m_bufferSize, &m_periodSize)) < 0) {
        LOG(ERROR) << "cannot get parameters device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }

    // start capture
    else if ((err = snd_pcm_prepare (m_pcm)) < 0) {
        LOG(ERROR) << "cannot prepare audio interface for use device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }
#if 0
    else if ((err = snd_pcm_start (m_pcm)) < 0) {
        LOG(ERROR) << "cannot start audio interface for use device: " << m_params.m_devName << " error:" <<  snd_strerror (err);
        this->close();
    }
#endif
    snd_pcm_hw_params_free(hw_params);
    LOG(NOTICE) << "ALSA device: \"" << m_params.m_devName << "\" buffer_size:" << m_bufferSize << " period_size:" << m_periodSize << " rate:" << m_params.m_sampleRate;
}

int ALSAPlayback::configureFormat(snd_pcm_hw_params_t *hw_params)
{

    // try to set format, widht, height
    std::list<snd_pcm_format_t>::iterator it;
    for (it = m_params.m_formatList.begin(); it != m_params.m_formatList.end(); ++it) {
        snd_pcm_format_t format = *it;
        int err = snd_pcm_hw_params_set_format (m_pcm, hw_params, format);
        if (err < 0) {
            LOG(NOTICE) << "cannot set sample format device: " << m_params.m_devName << " to:" << format << " error:" <<  snd_strerror (err);
        } else {
            //LOG(NOTICE) << "set sample format device: " << m_params.m_devName << " to:" << format << " ok";
            m_fmt = format;
            return 0;
        }
    }
    return -1;
}

//size_t ALSAPlayback::read(char* buffer, size_t bufferSize)
//{
//	size_t size = 0;
//	int fmt_phys_width_bytes = 0;
//	if (m_pcm != 0)
//	{
//		int fmt_phys_width_bits = snd_pcm_format_physical_width(m_fmt);
//		fmt_phys_width_bytes = fmt_phys_width_bits / 8;
//
//		snd_pcm_sframes_t ret = snd_pcm_readi (m_pcm, buffer, m_periodSize*fmt_phys_width_bytes);
//		LOG(DEBUG) << "ALSA buffer in_size:" << m_periodSize*fmt_phys_width_bytes << " read_size:" << ret;
//		if (ret > 0) {
//			size = ret;
//
//			// swap if capture in not in network order
//			if (!snd_pcm_format_big_endian(m_fmt)) {
//				for(unsigned int i = 0; i < size; i++){
//					char * ptr = &buffer[i * fmt_phys_width_bytes * m_params.m_channels];
//
//					for(unsigned int j = 0; j < m_params.m_channels; j++){
//						ptr += j * fmt_phys_width_bytes;
//						for (int k = 0; k < fmt_phys_width_bytes/2; k++) {
//							char byte = ptr[k];
//							ptr[k] = ptr[fmt_phys_width_bytes - 1 - k];
//							ptr[fmt_phys_width_bytes - 1 - k] = byte;
//						}
//					}
//				}
//			}
//		}
//	}
//	return size * m_params.m_channels * fmt_phys_width_bytes;
//}

size_t ALSAPlayback::write(char* buffer,size_t bufferSize)
{
    int frames_writen ;
#if 0
    int size;
    int count = (bufferSize % m_periodSize) ? (bufferSize / m_periodSize) + 1 : (bufferSize / m_periodSize);
    int last_size = (bufferSize % m_periodSize) ? (bufferSize % m_periodSize) : m_periodSize;
    for(int i = 0; i < count; i++){

        if(i != count -1)
            size = m_periodSize;
        else
            size = last_size;

        frames_writen = snd_pcm_writei(m_pcm,buffer+(i*m_periodSize),size);
        if(frames_writen == -EPIPE){
            LOG(NOTICE)<<"underrun occurred. i = "<<i;
            snd_pcm_prepare(m_pcm);
        }else if(frames_writen < 0){
            LOG(NOTICE)<<"error from writei:"<<snd_strerror(frames_writen);
        }else if(frames_writen != size){
            LOG(NOTICE)<<"short write, write"<<frames_writen<<"frames.";
        }else{
            //LOG(NOTICE)<<"play "<<size << "bytes.";
        }

    }
#else
     frames_writen = snd_pcm_writei(m_pcm,buffer,m_periodSize);
    if(frames_writen == -EPIPE){
        LOG(NOTICE)<<"underrun occurred.";
        snd_pcm_prepare(m_pcm);
    }else if(frames_writen < 0){
        LOG(NOTICE)<<"error from writei:"<<snd_strerror(frames_writen);
    }else if(frames_writen != m_periodSize){
        LOG(NOTICE)<<"short write, write"<<frames_writen<<"frames.";
    }
#endif
    return frames_writen;
}

int ALSAPlayback::getFd()
{
    unsigned int nbfs = 1;
    struct pollfd pfds[nbfs];
    pfds[0].fd = -1;

    if (m_pcm != 0)
    {
        int count = snd_pcm_poll_descriptors_count (m_pcm);
        int err = snd_pcm_poll_descriptors(m_pcm, pfds, count);
        if (err < 0) {
            fprintf (stderr, "cannot snd_pcm_poll_descriptors (%s)\n", snd_strerror (err));
        }
    }
    return pfds[0].fd;
}

