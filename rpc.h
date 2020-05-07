#ifndef RPC_H
#define RPC_H

#include <QObject>
#include <jcon/json_rpc_websocket_server.h>
#include "group.h"


class rpc:public QObject
{
    Q_OBJECT
public:
    explicit rpc(QObject *parent = 0);
    void init();
    Q_INVOKABLE int getRandomInt(int limit);
private:
    jcon::JsonRpcWebSocketServer *rpcServer;
    Group *group;

signals:

public slots:
    QVariantMap getNetState();
    int setVoMode(int wnd_num);
    //int getRandomInt(int limit);

};

extern rpc *GRPC;

#endif // RPC_H
