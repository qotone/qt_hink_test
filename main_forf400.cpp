#include <QCoreApplication>
#include <QTextCodec>
#include <QUrl>
#include <QDebug>
#include <QObject>
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

#include <signal.h>
#include "unixsignalhandler.h"
#include "unixsignalnotifier.h"

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
#if 1
   #if 0
    HinkObject *vo_mon=Hink::create("HinkOutputVo");
    QVariantMap dataVo;
    //dataVo["type"]="hdmi";
    dataVo["type"]="bt1120";
    //dataVo["output"] = "3840x2160_30";//"1080P60";
    dataVo["output"] = "1080P60";
    vo_mon->start(dataVo);
    #endif
    HinkObject *vo_hdmi = Hink::create("HinkOutputVo");
    QVariantMap dataVo_hdmi;
    dataVo_hdmi["type"] = "hdmi";
    dataVo_hdmi["output"] = "1080P60";
    vo_hdmi->start(dataVo_hdmi);


    HinkObject *vo_bt1120=Hink::create("HinkOutputVo");
    QVariantMap dataVo_bt1120;
    //dataVo["type"]="hdmi";
    dataVo_bt1120["type"]="bt1120";
    //dataVo["output"] = "3840x2160_30";//"1080P60";
    dataVo_bt1120["output"] = "1080P60";
    vo_bt1120->start(dataVo_bt1120);
#endif

#if 1
    HinkObject *vi = Hink::create("HinkInputVi");
    QVariantMap dataVi;

    dataVi["width"] = 1920;
    dataVi["height"] = 1080;
    dataVi["interface"]="HDMI-A";
    vi->start(dataVi);

    HinkObject *usbcam = Hink::create("HinkInputV4l2");
    QVariantMap dataUsb;
    dataUsb["width"] = 1920;
    dataUsb["height"] = 1080;
    dataUsb["framerate"] = 30;
    usbcam->start(dataUsb);
    HinkObject *vdec = Hink::create("HinkDecodeV");
    vdec->start();

    usbcam->linkV(vdec)->linkV(vo_hdmi);
    vi->linkV(vo_bt1120);
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
    //QObject::connect(new UnixSignalHandler(SIGTERM,&a), &UnixSignalHandler::raised, &a, &QCoreApplication::quit);
    //QObject::connect(new UnixSignalHandler(SIGINT,&a), &UnixSignalHandler::raised, &a, &QCoreApplication::quit);

    // Make sure that the application terminates cleanly on various unix signals.
    QObject::connect(UnixSignalNotifier::instance(), SIGNAL(unixSignal(int)), &a, SLOT(quit()));
    UnixSignalNotifier::instance()->installSignalHandler(SIGINT);
    UnixSignalNotifier::instance()->installSignalHandler(SIGTERM);
    UnixSignalNotifier::instance()->installSignalHandler(SIGQUIT);
    return a.exec();
}
