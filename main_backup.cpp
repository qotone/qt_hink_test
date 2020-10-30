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

rpc *GRPC;

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
    HinkObject *ai = Hink::create("HinkInputAi");
    QVariantMap dataAi;

    dataAi["interface"] = "Mini-In";
    ai->start(dataAi);

#if 0
    HinkObject *ao = Hink::create("HinkOutputAo");
    QVariantMap dataAo;
    dataAo["interface"] = "Mini-Out";
    ao->start(dataAo);
    ai->linkA(ao);
#else
    HinkObject *ai_alsa = Hink::create("HinkInputTinyalsa");
    ai_alsa->start();
    HinkObject *filewrite = new FileWrite();
    filewrite->start();
    //ai_alsa->linkA(filewrite);
    //HinkObject *adec = Hink::create("HinkDecodeA");
    //adec->start();

    HinkObject *ao = Hink::create("HinkOutputAo");
    QVariantMap dataAo;
    dataAo["interface"] = "Mini-Out";
    ao->start(dataAo);

    //ai_alsa->linkA(ao);

    HinkObject *aenc = Hink::create("HinkEncodeA");
    aenc->start();

    HinkObject *sink = new Sink();
    QVariantMap dataSink;
    dataSink["ip"] = "192.168.16.115";
    dataSink["dataPort"] = 8008;
    dataSink["payload"] = 101;
    dataSink["payloadType"] = "G726-16";//"H265";
    sink->start(dataSink);
    //ai_alsa->linkA(aenc)->linkA(sink);

    ai->linkA(aenc)->linkA(sink);

#endif
    return a.exec();
}
