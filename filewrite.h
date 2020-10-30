#ifndef FILEWRITE_H
#define FILEWRITE_H
#include "Hink.h"
#include "HinkObject.h"
#include <QFile>
#include <QDataStream>

class FileWrite : public HinkObject
{
    Q_OBJECT
public:
    FileWrite(QObject *parent = nullptr);
    ~FileWrite();
private:
    QFile *m_file;
    QDataStream *m_datastream;
public slots:
    void onStart();
    void onStop();
    void onSetData(QVariantMap newData);
    void onNewInfoA(StreamInfo info);
    void onNewPacketA(Packet pkt);
};

#endif // FILEWRITE_H
