#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>
#include <QMap>
#include <QVariantMap>

class Channel : public QObject
{
    Q_OBJECT
public:
    explicit Channel(QObject *parent = nullptr);
    virtual void init(QVariantMap cfg=QVariantMap());
    virtual void updateConfig(QVariantMap cfg);
//    void doSnap();
    QString type;
//    bool enableAVS;
    int id;
    QVariantMap data;
    //
    bool enable;
    //
//    QMap<QString

signals:

};

#endif // CHANNEL_H
