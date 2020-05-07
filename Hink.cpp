#include "Hink.h"
#include "Json.h"
#include <cstring>
#include <QMetaObject>
#include <QHash>
#include "HinkOutputVo.h"
#include "HinkDecodeV.h"
#include "HinkInputV4l2.h"
#include "HinkInputVi.h"
#include "HinkEncodeV.h"
#include "HinkOutputAo.h"
#include "HinkInputAi.h"

//#include "hi_comm_sys.h"
//#include "mpi_sys.h"




//extern "C" {
#include "hink_comm.h"
#include "hink_sys.h"
#include "hink_vdec.h"
//}



#define BOARD_CONFIG_PATH "/hink/configs/board.json"

QVariantMap  Hink::Config;
QMap<QString,int> Hink::classCount;
QMap<QString,HinkObject*> Hink::objectMap;
QMap<QString,QThread*> Hink::threadMap;
QMap<QString,int> Hink::threadCount;


Hink::Hink(QObject *parent) : QObject(parent)
{

}


template<typename T>
HinkObject* Hink::constructorHelper( QObject* parent )
{
    return new T( parent );
}

template<typename T>
int Hink::registerClass()
{
    qDebug()<<" T::staticMetaObject.className()="<< T::staticMetaObject.className();
    constructors().insert( T::staticMetaObject.className(), &constructorHelper<T> );
}



HinkObject * Hink::create(QString className, QString name)
{
    HinkObject* hinkObj = NULL;
    Constructor constructor = constructors().value( className );
    if ( constructor == NULL ){
        qDebug()<<"constructor == NULL";
        return NULL;
    }
    if(name.isEmpty()){
        name = className + tr("_%1").arg(classCount.value(className));
    }else{

    }
    if(classCount.contains(className)){
        classCount[className]++;
    }else{
        classCount.insert(className,1);
    }

    qDebug()<<"name ="<<name;
    hinkObj = (*constructor)(NULL);
    hinkObj->setObjectName(name);
    objectMap.insert(name,hinkObj);
    return hinkObj;
}

HinkObject * Hink::find(QString name)
{
    if(objectMap.contains(name)){
        objectMap.value(name);
    }else{
        return NULL;
    }
}


bool Hink::init(bool sys)
{
    if(!setConfig(BOARD_CONFIG_PATH)){
        qDebug()<<tr("board configure file:%1 not exit.").arg(BOARD_CONFIG_PATH);
        return false;
    }
    if(sys){
        if(!initSys()){
            qDebug()<<tr("hisi sys init failed.");
            return false;
        }
    }

    registerClass<HinkOutputVo>();
    registerClass<HinkDecodeV>();
    registerClass<HinkInputV4l2>();
    registerClass<HinkInputVi>();
    registerClass<HinkEncodeV>();

    registerClass<HinkInputAi>();
    registerClass<HinkOutputAo>();

    return true;
}


bool Hink::setConfig(QString path)
{
    QVariant v;
    v = Json::loadFile(path);
    if(!v.isNull()){
        Config = v.toMap();

    }else{
        return false;
    }

    return true;

}

QVariantMap Hink::getConfig()
{
    return Config;
}

HinkObject * Hink::linkV(HinkObject *src, HinkObject *dst)
{
    //connect(src,SIGNAL(newPacketV(Packet)),dst,SLOT(onNewPacketV(Packet)));
    return src->linkV(dst);
}


QThread * Hink::newThread(QString &name, QThread::Priority priority)
{
    QThread *thread = new QThread ();
    //thread->setPriority(priority);
    threadMap.insert(name,thread);

    return thread;

}

bool Hink::initSys()
{
    VB_CONF_S stModVbConf;
    QVariantMap video_buf = Config["videoBuffer"].toMap();
    QVariantList sysList = video_buf["sys"].toList();
    QVariantList decList = video_buf["dec"].toList();
    int sysBufSize,sysBufCnt;
    int decBufSize,decBufCnt;
    sysBufSize = sysList[0].toMap()["size"].toInt();
    sysBufCnt = sysList[0].toMap()["cnt"].toInt();
    decBufCnt = decList[0].toMap()["cnt"].toInt();
    decBufSize = decList[0].toMap()["size"].toInt();

    hink_sys_init((HI_U32)sysBufSize,(HI_U32)sysBufCnt);
    //hink_sys_init(3110400,40);//3110400
    //hink_sys_init(12441600,18);



    memset(&stModVbConf, 0, sizeof(VB_CONF_S));
    stModVbConf.u32MaxPoolCnt = 2;


    stModVbConf.astCommPool[0].u32BlkSize = decBufSize;
    stModVbConf.astCommPool[0].u32BlkCnt  = 5*decBufCnt;

    //stModVbConf.astCommPool[1].u32BlkSize = decList[1].toMap()["size"].toUInt();
    //stModVbConf.astCommPool[1].u32BlkCnt  = decList[1].toMap()["cnt"].toUInt();

    hink_vb_init(&stModVbConf,HINK_VBS_COMM);



    return true;


}


//QHash<QString, Constructor>& Hink::constructors()
//{

//}

