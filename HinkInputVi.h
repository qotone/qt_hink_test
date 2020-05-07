#ifndef HINKINPUTVI_H
#define HINKINPUTVI_H
#include "HinkObject.h"
#include "hi_comm_vi.h"

class HinkInputVi : public HinkObject
{
    Q_OBJECT
public:
    HinkInputVi(QObject *parent = 0);


private slots:
    void onStart();
    void onSetData(QVariantMap data);

private:
    VI_DEV_ATTR_S stDevAttr;

    void vi_setMask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr);
};

#endif // HINKINPUTVI_H
