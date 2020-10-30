#include "filewrite.h"

FileWrite::FileWrite(QObject *parent):HinkObject(parent)
{

    data["filename"]="audio.g726";

}

FileWrite::~FileWrite()
{

    onStop();
}
void FileWrite::onStart()
{
    m_file = new QFile(data["filename"].toString(),this);
    m_file->open(QIODevice::ReadWrite | QFile::Truncate);
}

void FileWrite::onNewPacketA(HinkObject::Packet pkt)
{
    qDebug("file write %u bytes.",pkt.len);

    m_file->write(pkt.data,pkt.len);
}

void FileWrite::onStop()
{
    m_file->close();
}

void FileWrite::onSetData(QVariantMap newData)
{

}

void FileWrite::onNewInfoA(StreamInfo info)
{

}
