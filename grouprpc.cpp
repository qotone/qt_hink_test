#include "grouprpc.h"
#include <QNetworkInterface>
#include <sys/socket.h>
#include <QEventLoop>
#include <QTimer>
#include <QMetaMethod>

GroupRPC::GroupRPC(QObject *parent) : QObject(parent),socket(this)
{
    timeout = 200;
    connect(&socket,SIGNAL(readyRead()),this,SLOT(onRead()));
    groupId = 0;
    port = 5432;

}

bool GroupRPC::init(int p)
{
    port = p;
//    updateNet();
    if(!socket.bind(port))
        return false;

    socket.setSocketOption(QUdpSocket::SendBufferSizeSocketOption,1024*1024);
    socket.setSocketOption(QUdpSocket::ReceiveBufferSizeSocketOption,1024*1024);
    return true;

}

void GroupRPC::updateNet()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
    for(int i = 0; i < nets.count(); i++){
        if(!nets[i].flags().testFlag(QNetworkInterface::IsLoopBack)){
            localMac = nets[i].hardwareAddress();
            QList<QNetworkAddressEntry> addrs = nets[i].addressEntries();
            for(int k = 0; k < addrs.count(); k++){
                if(addrs[k].ip() != QHostAddress::LocalHost && addrs[k].ip().protocol() ==QAbstractSocket::IPv4Protocol){
                    localIp = addrs[k].ip();
                }
            }
            break;
        }
    }
}

void GroupRPC::addMethod(QString name, QObject *obj, const char *method)
{
    objectMap[name] = obj;
    methodMap[name] = method;
}

QVariant GroupRPC::call(QString method, QString to, QVariant data, bool noRespond)
{
    QVariant v;
    return v;
}

QString GroupRPC::getLocalMac()
{
    return  localMac;
}

void GroupRPC::setGroupId(int gid)
{
    groupId = gid;
}

QHostAddress GroupRPC::getFromIp()
{
    return fromIp;
}

QHostAddress GroupRPC::getLocalIp()
{
    return localIp;
}

QString GroupRPC::getFromMac()
{
    return fromMac;
}

QVariant GroupRPC::callbak(QVariantMap msg)
{
    QVariant v;
    return v;
}

void GroupRPC::send(QVariantMap &msg)
{
    QByteArray json = QJsonDocument::fromVariant(msg).toJson();
    socket.writeDatagram(json,QHostAddress::Broadcast,port);
}

//void GroupRPC::onRead()
//{

//}
