#ifndef HINK_H
#define HINK_H
#include "HinkObject.h"
#include <QObject>
#include <QMap>
#include <QHash>
#include <QVariant>
#include <QByteArray>
#include <QThread>
#include <QDebug>

/*Object factory
* [1](https://www.mimec.org/blog/object-factory)
* [2](https://www.cnblogs.com/ztzheng/p/4122975.html)
* [3](https://blog.csdn.net/xiaoxiaoyusheng2012/article/details/45438899)
* [4]()
*/

class Hink : public QObject
{
    Q_OBJECT
public:
    friend class HinkObject;
    explicit Hink(QObject *parent = 0);
    template<typename T>
    static int registerClass();

    static bool init(bool sys = true);
    static HinkObject * create(QString className, QString name = "");
    static HinkObject* find(QString name);
    static QVariantMap getConfig();
    static bool setConfig(QString path);
//    static QVariant o2v(HinkObject *obj);
//    static HinkObject* v2o(QVariant var);
    static QThread *newThread(QString &name,QThread::Priority priority=QThread::NormalPriority);
//    static void delThread(QString name);
//    static QVariantMap getVersion();

private:
    typedef HinkObject* (*Constructor)( QObject* parent );
    template<typename T>
    static HinkObject* constructorHelper( QObject* parent );
    static QHash<QString, Constructor>& constructors()
    {
        static QHash<QString, Constructor> instance;
        return instance;
    }
    static bool initSys();

//    static HinkObject* linkA(HinkObject *src, HinkObject *dst);
    static HinkObject* linkV(HinkObject *src, HinkObject *dst);
//    static HinkObject* linkE(HinkObject *src, HinkObject *dst);
//    static void unLinkA(HinkObject *src, HinkObject *dst);
//    static void unLinkV(HinkObject *src, HinkObject *dst);
//    static void unLinkE(HinkObject *src, HinkObject *dst);
//    static void delObject(HinkObject *obj);

private:
    static QMap<QString,int> classCount;
    static QMap<QString,HinkObject*> objectMap;

    static QMap<QString,QThread*> threadMap;
    static QMap<QString,int> threadCount;
    static QMutex lockThread;

    static QVariantMap Config;

signals:

};

#endif // HINK_H
