#ifndef HINKINPUTAI_H
#define HINKINPUTAI_H
#include "HinkObject.h"
#include <QSocketNotifier>
#include "hi_comm_ai.h"
#include "mpi_ai.h"
class HinkInputAi : public HinkObject
{
    Q_OBJECT
public:
    HinkInputAi(QObject *parent = 0);
    ~HinkInputAi();

private slots:
    void onStart();
    void onSetData(QVariantMap data);
    void onReadData(void);
private:
    int m_fd;
    AUDIO_DEV aiDev;
    AI_CHN aiChn;
    QSocketNotifier *notifier;
    int8_t *m_buf;
};

#endif // HINKINPUTAI_H
