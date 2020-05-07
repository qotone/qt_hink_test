#include "group.h"
#include "Json.h"
#include "Config.h"
#include <QFile>
#include <QDomDocument>

Group::Group(QObject *parent) :
    QObject(parent),timer(this)
{
    rpc = new GroupRPC();
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimer()));

}

void Group::init()
{
    rpc->init();
    rpc->addMethod("hello",this,"hello");
    rpc->addMethod("getNetwork",this,"getNetwork");
    rpc->addMethod("setNetwork",this,"setNetwork");
    rpc->addMethod("setEncode",this,"setEncode");
    rpc->addMethod("setStream",this,"setStream");
    rpc->addMethod("reboot",this,"reboot");
//    rpc->addMethod("getEPG",this,"getEPG");
//    rpc->addMethod("syncEPG",this,"syncEPG");

    loadConfig();
    timer.start(3000);
//    clearMember();
    onTimer();
}

void Group::loadConfig()
{
    QVariantMap cfg = Json::loadFile(GRPPATH).toMap();
    rpc->setGroupId(cfg["groupId"].toInt());
    type = cfg["type"].toString();
    version = Json::loadFile("/link/config/version.json").toMap();
}

void Group::onTimer()
{
    QVariantList list = Json::loadFile(CFGPATH).toList();
    QVariantList nameList;
    for(int i = 0; i < list.count(); i++){
        if(list[i].toMap()["enable"].toBool())
            nameList<<list[i].toMap()["name"].toString();
    }

    memberInfo[rpc->getLocalMac()]["info"] = nameList;
    memberInfo[rpc->getLocalMac()]["type"] = type;
    memberInfo[rpc->getLocalMac()]["version"] = version;

    QVariantMap data;
    data["info"] = nameList;
    data["type"] = type;
    data["version"] = version;
    rpc->call("hello","any",data,true);
}

void Group::hello(QVariant data)
{
    QString mac = rpc->getFromMac();
    memberInfo[mac]["ip"] = rpc->getFromIp().toString().replace("::ffff:","");
    memberInfo[mac]["info"] = data.toMap()["info"];
    memberInfo[mac]["type"] = data.toMap()["type"];
    memberInfo[mac]["version"] = data.toMap()["version"];
}

bool Group::clearMember()
{
    memberInfo.clear();
    return true;
}

bool Group::update(QVariantMap json)
{
    Json::saveFile(json,GRPPATH);
    loadConfig();
    return true;
}

QVariant Group::setNetwork(QVariant cfg)
{
    Json::saveFile(cfg,NETPATH);
    system("/link/shell/setNetwork.sh");
    rpc->updateNet();
    return true;
}

QVariant Group::setEncode(QVariant cfg)
{
    QVariantList chnList = Json::loadFile(CFGPATH).toList();
    for(int i = 0; i < chnList.count(); i++){
        QVariantMap map = chnList[i].toMap();
        map["enca"] = cfg.toMap()["enca"].toMap();
        map["encv"] = cfg.toMap()["encv"].toMap();
        map["encv2"] = cfg.toMap()["encv2"].toMap();
        chnList[i] = map;
    }

    Json::saveFile(chnList,CFGPATH);
    Config::loadConfig(CFGPATH);
    return true;
}

QVariant Group::setStream(QVariant cfg)
{

    return true;
}

QVariant Group::setStream2(QVariant cfg)
{

    return true;
}

QVariant Group::getNetwork()
{
    return Json::loadFile(NETPATH);
}


//QVariantList Group::getList()
//{

//}

void Group::reboot()
{
    system("reboot");
}

bool Group::callSetNetwork(QString mac, QVariantMap json)
{
    return rpc->call("setNetwork",mac,json).toBool();
}

QVariantMap Group::callGetNetwork(QString mac)
{
    return rpc->call("getNetwork",mac).toMap();
}

bool Group::callSetEncode(QString mac, QVariantMap json)
{
    return rpc->call("setEncode",mac,json).toBool();
}

bool Group::callSetStream(QString mac, QVariantMap json)
{
    return rpc->call("setStream",mac,json).toBool();
}

bool Group::callReboot(QString mac)
{
    rpc->call("reboot",mac,QVariant(),true);
    return true;
}
