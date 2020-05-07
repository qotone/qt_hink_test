#include "channel.h"
#include <unistd.h>

Channel::Channel(QObject *parent) : QObject(parent)
{
    enable = false;
    id = -1;
}

void Channel::init(QVariantMap cfg)
{

}

void Channel::updateConfig(QVariantMap cfg)
{
    data = cfg;
    this->enable = data["enable"].toBool();
}
