#include <QCoreApplication>
#include <QTextCodec>
#include <QUrl>
#include <QDebug>
#include <jcon/json_rpc_tcp_client.h>
#include <jcon/json_rpc_tcp_server.h>
#include <jcon/json_rpc_websocket_client.h>
#include <jcon/json_rpc_websocket_server.h>

#include "rpc.h"
#include <ctime>


#include "example_service.h"
#include "Hink.h"
#include "HinkObject.h"
#include "HinkOutputVo.h"
#include "HinkDecodeV.h"
#include "HinkInputV4l2.h"
#include "HinkInputVi.h"
#include "HinkEncodeV.h"
#include "HinkInputAi.h"
#include "HinkOutputAo.h"
#include "Sink.h"
#include "HinkDecodeA.h"
#include "HinkInputTinyalsa.h"
#include "filewrite.h"
#include "HinkEncodeA.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <alsa/asoundlib.h>
#include "ALSACapture.h"
#include "HinkOutputAlsa.h"

#include <signal.h>
#include "unixsignalhandler.h"
#include "unixsignalnotifier.h"

#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "mpi_sys.h"
#include "mpi_vb.h"

//#include "AlsaPlayback.h"




rpc *GRPC;

void someFunction()
{
    qDebug()<<"signal catched!";
}

#if 1
/* ---------------------------------------------------------------------------
**  get a "deviceid" from uevent sys file
** -------------------------------------------------------------------------*/
std::string getDeviceId(const std::string& evt) {
    std::string deviceid;
    std::istringstream f(evt);
    std::string key;
    while (getline(f, key, '=')) {
            std::string value;
        if (getline(f, value)) {
            if ( (key =="PRODUCT") || (key == "PCI_SUBSYS_ID") ) {
                deviceid = value;
                break;
            }
        }
    }
    return deviceid;
}

std::string  getAlsa(const std::string& v4l2device) {
    std::string audioDevice(v4l2device);
#if 0
    std::map<std::string,std::string> videodevices;
    std::string video4linuxPath("/sys/class/video4linux");
    DIR *dp = opendir(video4linuxPath.c_str());
    if (dp != NULL) {
        struct dirent *entry = NULL;
        while((entry = readdir(dp))) {
            std::string devicename;
            std::string deviceid;
            if (strstr(entry->d_name,"video") == entry->d_name) {
                std::string ueventPath(video4linuxPath);
                ueventPath.append("/").append(entry->d_name).append("/device/uevent");
                std::ifstream ifsd(ueventPath.c_str());
                deviceid = std::string(std::istreambuf_iterator<char>{ifsd}, {});
                deviceid.erase(deviceid.find_last_not_of("\n")+1);
            }

            if (!deviceid.empty()) {
                videodevices[entry->d_name] = getDeviceId(deviceid);
            }
        }
        closedir(dp);
    }
#endif
    std::map<std::string,std::string> audiodevices;
    int rcard = -1;
    while ( (snd_card_next(&rcard) == 0) && (rcard>=0) ) {
        void **hints = NULL;
        if (snd_device_name_hint(rcard, "pcm", &hints) >= 0) {
            void **str = hints;
            while (*str) {
                std::ostringstream os;
                os << "/sys/class/sound/card" << rcard << "/device/uevent";

                std::ifstream ifs(os.str().c_str());
                std::string deviceid = std::string(std::istreambuf_iterator<char>{ifs}, {});
                deviceid.erase(deviceid.find_last_not_of("\n")+1);
                deviceid = getDeviceId(deviceid);

                if (!deviceid.empty()) {
                    if (audiodevices.find(deviceid) == audiodevices.end()) {
                        std::string audioname = snd_device_name_get_hint(*str, "NAME");
                        audiodevices[deviceid] = audioname;
                    }
                }

                str++;
            }

            snd_device_name_free_hint(hints);
        }
    }
#if 0
    auto deviceId  = videodevices.find(getDeviceName(v4l2device));
    if (deviceId != videodevices.end()) {
        auto audioDeviceIt = audiodevices.find(deviceId->second);

        if (audioDeviceIt != audiodevices.end()) {
            audioDevice = audioDeviceIt->second;
            std::cout <<  v4l2device << "=>" << audioDevice << std::endl;
        }
    }
#endif

    return audioDevice;
}
#endif

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);


    if(!Hink::init()){
        return 0;
    }


//    jcon::JsonRpcClient * rpc_client;

    GRPC = new rpc();
    GRPC->init();
#if 0
    HinkObject *vo=Hink::create("HinkOutputVo");
    QVariantMap dataVo;
    dataVo["type"]="hdmi";
    //dataVo["type"]="bt1120";
    dataVo["output"] = "3840x2160_30";//"1080P60";
    //dataVo["output"] = "1080P60";
    vo->start(dataVo);
#endif

#if 1
    //HinkObject *vi = Hink::create("HinkInputVi");
    //QVariantMap dataVi;

    //dataVi["width"] = 3840;
    //dataVi["height"] = 2160;
    //dataVi["interface"]="HDMI-B";
    //vi->start(dataVi);
#else

    HinkObject *usbcam = Hink::create("HinkInputV4l2");
    QVariantMap dataUsb;
    dataUsb["width"] = 1920;
    dataUsb["height"] = 1080;
    dataUsb["framerate"] = 30;
    //usbcam->start();
    usbcam->start(dataUsb);
    HinkObject *vdec = Hink::create("HinkDecodeV");
    vdec->start();

    usbcam->linkV(vdec)->linkV(vo);
#endif
    //vi->linkV(vo);
#if 0
    HinkObject *venc = Hink::create("HinkEncodeV");
    QVariantMap dataVenc;
    dataVenc["codec"] = "h265"; // "h264,h265,jpeg
    dataVenc["width"] = 1920;
    dataVenc["height"] = 1080;
    dataVenc["framerate"] = 30;

    dataVenc["bitrate"] = 2048;
    dataVenc["gop"] = 2;
    venc->start(dataVenc);

    HinkObject *sink = new Sink();
    QVariantMap dataSink;
    dataSink["ip"] = "192.168.1.119";
    dataSink["dataPort"] = 8008;
    dataSink["payload"] = 110;
    dataSink["payloadType"] = "H265";
    sink->start(dataSink);

    vi->linkV(venc)->linkV(sink);
    //usbcam->linkV(vdec)->linkV(venc)->linkV(sink);
#endif
    //HinkObject *ai = Hink::create("HinkInputAi");
    //QVariantMap dataAi;

    //dataAi["interface"] = "Mini-In";
    ////dataAi["ptNumPerFrm"] = 1024;
    //ai->start(dataAi);

#if 0
    HinkObject *ao = Hink::create("HinkOutputAo");
    QVariantMap dataAo;
    dataAo["interface"] = "Mini-Out";
    ao->start(dataAo);
    ai->linkA(ao);
#else
    //HinkObject *ai_alsa = Hink::create("HinkInputTinyalsa");
    HinkObject *ai_alsa = Hink::create("HinkInputAlsa");
    ai_alsa->start();
    //HinkObject *filewrite = new FileWrite();
    //QVariantMap dataFw;
    //dataFw["filename"] = "sound.pcm";
    //filewrite->start(dataFw);
    //ai_alsa->linkA(filewrite);
//    //HinkObject *adec = Hink::create("HinkDecodeA");
//    //adec->start();

    //HinkObject *ao = Hink::create("HinkOutputAo");
    //QVariantMap dataAo;
    //dataAo["interface"] = "Mini-Out";
    //ao->start(dataAo);

    //ai_alsa->linkA(ao);

    //HinkObject *aenc = Hink::create("HinkEncodeA");
    //aenc->start();

    HinkObject *sink = new Sink();
    QVariantMap dataSink;
    dataSink["ip"] = "192.168.16.119";
    dataSink["dataPort"] = 8008;
    dataSink["payload"] = 8;//108;//8;//101
    dataSink["payloadType"] ="PCMA";//"MP4A-LATM";//"PCMA";//"G726-16";//"H265";
    sink->start(dataSink);
    ai_alsa->linkA(sink);

#if 0
    // Init audio capture
    LOG(NOTICE) << "Create ALSA Source..." << audioDev;

    ALSACaptureParameters param(audioDev.c_str(), audioFmtList, audioFreq, audioNbChannels, verbose);
    ALSACapture* audioCapture = ALSACapture::createNew(param);
    if (audioCapture) {

    }
#endif

    //ai_alsa->linkA(aenc)->linkA(sink);

    //ai->linkA(aenc)->linkA(sink);
#if 0
    std::list<snd_pcm_format_t> audioFmtList;
    audioFmtList.push_back(SND_PCM_FORMAT_S16_LE);
    ALSAPlaybackParameters param("hw:0,0",audioFmtList,48000,2,1);
    ALSAPlayback *playback = ALSAPlayback::createNew(param);
#else


    //HinkObject *ao_alsa = Hink::create("HinkOutputAlsa");
    //ao_alsa->start();
    //ai->linkA(ao_alsa);
#if 0
    HinkObject *file_write = new FileWrite();
    QVariantMap dataFile;
    dataFile["filename"] = "audio_chn1_8K_stero.pcm";
    file_write->start(dataFile);
    ai->linkA(file_write);
#endif

#endif // if 0

#endif

    QObject::connect(QCoreApplication::instance(),&QCoreApplication::aboutToQuit,[=](){

        //delete ao_alsa;
        //delete ai;
        delete  ai_alsa;
        //delete filewrite;
        //delete aenc;
        //aenc->deleteLater();
        delete sink;
        //delete playback;
        //ao_alsa->deleteLater();
        //delete ao;

        HI_MPI_SYS_Exit();
        for(int i=0;i<VB_MAX_USER;i++)
        {
             HI_MPI_VB_ExitModCommPool(i);
        }
        for(int i=0; i<VB_MAX_POOLS; i++)
        {
             HI_MPI_VB_DestroyPool(i);
        }
        HI_MPI_VB_Exit();
    });
    // Make sure that the application terminates cleanly on various unix signals.
    QObject::connect(UnixSignalNotifier::instance(), SIGNAL(unixSignal(int)), &a, SLOT(quit()));
    //QObject::connect(UnixSignalNotifier::instance(), &UnixSignalNotifier::unixSignal,someFunction) ;
    UnixSignalNotifier::instance()->installSignalHandler(SIGINT);
    UnixSignalNotifier::instance()->installSignalHandler(SIGTERM);
    UnixSignalNotifier::instance()->installSignalHandler(SIGQUIT);


    return a.exec();
}
