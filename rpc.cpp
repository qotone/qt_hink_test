#include "rpc.h"
#include <QMap>
#include <jcon/json_rpc_server.h>
#include <QDateTime>
#include <QFile>

#include "Config.h"

rpc::rpc(QObject *parent):QObject(parent)
{

}

void rpc::init()
{
    group = new Group(this);
    //group->init();
    rpcServer = new jcon::JsonRpcWebSocketServer();
    jcon::JsonRpcServer::ServiceMap map;
    map[this]="enc";
    map[group] = "group";
    rpcServer->registerServices(map,"/");
//    rpcServer->registerServices({this});
    rpcServer->listen(6001);

}

QVariantMap rpc::getNetState()
{
    static qlonglong lastRx=0;
    static qlonglong lastTx=0;
    static qint64 lastPTS=0;

    qint64 now=QDateTime::currentDateTime().toMSecsSinceEpoch();
    qint64 span=now-lastPTS;
    lastPTS=now;
    QFile file("/proc/net/dev");
    file.open(QFile::ReadOnly);
    QString line;
    for(int i=0;i<3;i++)
        line=file.readLine();

    line=file.readLine();
    qlonglong rx=line.split(' ',QString::SkipEmptyParts).at(1).toLongLong()*8;
    qlonglong tx=line.split(' ',QString::SkipEmptyParts).at(9).toLongLong()*8;
    file.close();

    int speedrx=(rx-lastRx)*1000/span/1024;
    int speedtx=(tx-lastTx)*1000/span/1024;
    lastRx=rx;
    lastTx=tx;

    QVariantMap rt;
    rt["rx"]=speedrx;
    rt["tx"]=speedtx;
    return rt;

}

int rpc::getRandomInt(int limit)
{
    return qrand() % limit;
}

int rpc::setVoMode(int wnd_num)
{
    qDebug()<<"index ="<<wnd_num;
}
