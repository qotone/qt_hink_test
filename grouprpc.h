#ifndef GROUPRPC_H
#define GROUPRPC_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QMap>
#include <QList>


class GroupRPC : public QObject
{
    Q_OBJECT
public:
    GroupRPC(QObject *parent = 0);
    bool init(int p = 5432);
    void updateNet();
    void addMethod(QString name, QObject* obj, const char* method);
    QVariant call(QString method, QString to,QVariant data = QVariant(), bool noRespond = false);


    void setGroupId(int gid);
    QHostAddress getFromIp();
    QHostAddress getLocalIp();
    QString getFromMac();
    QString getLocalMac();

private:
    QVariant callbak(QVariantMap msg);
    void send(QVariantMap &msg);

private:
    QMap<QString,QObject*> objectMap;
    QMap<QString,const char *> methodMap;
    //QList<GroupRequest*> requestList;
    QString localMac;
    QHostAddress localIp;
    QUdpSocket socket;
    QHostAddress fromIp;
    QString fromMac;
    int timeout;
    int port;
    int groupId;

signals:

public slots:
    //void onRead();
};

#endif // GROUPRPC_H
