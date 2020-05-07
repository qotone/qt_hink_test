#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <channel.h>
#include <QList>

#define CFGPATH "./config.json"
#define GRPPATH "./group.json"
#define NETPATH "./net.json"


class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    static void loadConfig(QString path);
    static QList<Channel *> chns;
    static Channel* findChannelById(int id);

signals:

};

#endif // CONFIG_H
