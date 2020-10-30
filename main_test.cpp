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
#include "Source.h"

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
    HinkObject *vi = Hink::create("HinkInputVi");
    QVariantMap dataVi;

    dataVi["width"] = 1920;
    dataVi["height"] = 1080;
    dataVi["interface"]="HDMI-A";
    vi->start(dataVi);

    HinkObject *venc = Hink::create("HinkEncodeV");
    QVariantMap dataVenc;
    dataVenc["codec"] = "h265"; // "h264,h265,jpeg
    dataVenc["width"] = 1920;
    dataVenc["height"] = 1080;
    dataVenc["framerate"] = 30;

    dataVenc["bitrate"] = 1088;
    dataVenc["gop"] = 1;
    venc->start(dataVenc);

    HinkObject *sink = new Sink();
    QVariantMap dataSink;
    dataSink["ip"] = "192.168.16.119";
    dataSink["dataPort"] = 8008;
    dataSink["payload"] = 97;
    dataSink["payloadType"] = "H265";
    sink->start(dataSink);

    vi->linkV(venc)->linkV(sink);
    //usbcam->linkV(vdec)->linkV(venc)->linkV(sink);
#else


    HinkObject *vo=Hink::create("HinkOutputVo");
    QVariantMap dataVo;
    dataVo["type"]="hdmi";
    //dataVo["type"]="bt1120";
    dataVo["output"] = "1080P30";
    vo->start(dataVo);
    HinkObject *vdec = Hink::create("HinkDecodeV");
    vdec->start();
    HinkObject *src = new Source();
    src->start();


    src->linkV(vdec)->linkV(vo);

#endif
    return a.exec();
}
