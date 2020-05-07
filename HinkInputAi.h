#ifndef HINKINPUTAI_H
#define HINKINPUTAI_H
#include "HinkObject.h"

class HinkInputAi : public HinkObject
{
    Q_OBJECT
public:
    HinkInputAi(QObject *parent = 0);

private slots:
    void onStart();
    void onSetData(QVariantMap data);
};

#endif // HINKINPUTAI_H
