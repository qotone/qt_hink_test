#ifndef HINKOUTPUTVO_H
#define HINKOUTPUTVO_H
#include "HinkObject.h"

class HinkOutputVo : public HinkObject
{
    Q_OBJECT
public:
    HinkOutputVo(QObject *parent = 0);


private:
    void initModeList(void);

    int mode;
    QString output_str;
    QMap<QString,int> outputMap;
    QMap<QString,int> typeMap;
    static int voCount;

private slots:
    void onStart();
    void onSetData(QVariantMap data);
    void onNewPacketV(Packet);
};

#endif // HINKOUTPUTVO_H
