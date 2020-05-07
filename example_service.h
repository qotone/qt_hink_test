#ifndef EXAMPLESERVICE_H
#define EXAMPLESERVICE_H

#include <QObject>

class ExampleService : public QObject
{
    Q_OBJECT
public:
    explicit ExampleService(QObject *parent = nullptr);
    virtual ~ExampleService();

    Q_INVOKABLE int getRandomInt(int limit);
    Q_INVOKABLE QString printMessage(const QString& msg);
    Q_INVOKABLE void printNotification(const QString& msg);
    Q_INVOKABLE void namedParams(const QVariantMap& m);

signals:

};

#endif // EXAMPLESERVICE_H
