#include "Json.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

Json::Json()
{

}

QVariant Json::loadFile(QString path)
{
    QVariant v ;
    QFile file(path);
    QJsonParseError jsonParserError;
    QJsonDocument jsonDoc;
    if( !file.open(QIODevice::ReadOnly)){

        qDebug()<<"File: %s open failed!\n";
        return v;
    }

    jsonDoc = QJsonDocument::fromJson(file.readAll(),&jsonParserError);

    if( !jsonDoc.isNull() && jsonParserError.error == QJsonParseError::NoError){
        //
        v = jsonDoc.toVariant();
    }
    file.close();
    return v;
}

bool Json::saveFile(QVariant data, QString path)
{

    QByteArray byteArray;
    QFile file(path);


QJsonDocument jsonDoc = QJsonDocument::fromVariant(data);

    byteArray = jsonDoc.toJson();

    qDebug()<<"save:"<<jsonDoc.toJson();

    if( !file.open(QIODevice::WriteOnly)){
        qDebug()<<"File: %s open failed!\n";
        return false;
    }

    if( !file.write(byteArray) ){
        qDebug()<<"File:%s write failed!\n";
        file.close();
        return false;
    }
    file.close();

    return true;

}


