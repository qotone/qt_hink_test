#include "HinkInputVi.h"
#include "Hink.h"
#include "mpi_vi.h"

#include "mpi_vpss.h"
#include "hi_comm_vpss.h"
#include "hink_vpss.h"
#include <cstdio>
#include <cstring>


VI_DEV_ATTR_S VI_DEV_ATTR_7441_BT1120_STANDARD_BASE =
{
    /*interface mode*/
    VI_MODE_BT1120_STANDARD,
    /*work mode, 1/2/4 multiplex*/
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0xFF0000},

    /* for single/double edge, must be set when double edge*/
    VI_CLK_EDGE_SINGLE_UP,
    /* VI_CLK_EDGE_DOUBLE, */

    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, just support yuv*/
    VI_INPUT_DATA_UVUV,

    /*sync info*/
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,

    /*timing info*/
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1920,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            1080,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /*whether use isp*/
    VI_PATH_BYPASS,
    /*data type*/
    VI_DATA_TYPE_YUV

};



void HinkInputVi::vi_setMask(VI_DEV ViDev, VI_DEV_ATTR_S *pstDevAttr)
{
        switch (ViDev % 2)
        {
        case 0:
                pstDevAttr->au32CompMask[0] = 0xFF000000;
                if (VI_MODE_BT1120_STANDARD == pstDevAttr->enIntfMode)
                {
                        pstDevAttr->au32CompMask[0] = 0xFF000000;
                        pstDevAttr->au32CompMask[1] = 0xFF000000>>8;
                }
                else
                {
                        pstDevAttr->au32CompMask[1] = 0x0;
                }
                break;

        case 1:
                pstDevAttr->au32CompMask[0] = 0xFF000000>>8;
                pstDevAttr->au32CompMask[1] = 0x0;
                break;
        default:
                break;
        }

}



HinkInputVi::HinkInputVi(QObject *parent): HinkObject(parent)
{

    data["interface"] = "";
    //pic指定的NoSignal图像必须为YUV420sp格式的原始图像，1920x1080。
    data["pic"] = "/hink/configs/nosignal.yuv";
    data["avalible"] = false;
    data["width"] = 1920;
    data["height"] = 1080;
    data["rotate"] = 0;
    data["double"] = false;
    data["mode"] = "bt1120s";
    data["interlace"] = false;
    data["srcFramerate"] = 30;
    data["dstFramerate"] = 30;
    data["did"] = 0;
    data["cid"] = 0;
    data["crop"];

    infoSelfV.type = StreamInfo::VPSS;
    infoSelfV.info["modId"] = HI_ID_VPSS;
    infoSelfV.info["devId"] = 1;
    infoSelfV.info["chnId"] = 0;

#if 0
    memset(&stDevAttr,0,sizeof (VI_DEV_ATTR_S));
    memcpy(&stDevAttr, &VI_DEV_ATTR_7441_BT1120_STANDARD_BASE,sizeof(VI_DEV_ATTR_S));
#endif
    stDevAttr = {
        /*interface mode*/
        VI_MODE_BT1120_STANDARD,
        /*work mode, 1/2/4 multiplex*/
        VI_WORK_MODE_1Multiplex,
        /* r_mask    g_mask    b_mask*/
        {0xFF000000,    0xFF0000},

        /* for single/double edge, must be set when double edge*/
        VI_CLK_EDGE_SINGLE_UP,
        /* VI_CLK_EDGE_DOUBLE, */

        /*AdChnId*/
        {-1, -1, -1, -1},
        /*enDataSeq, just support yuv*/
        VI_INPUT_DATA_UVUV,

        /*sync info*/
        {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,

        /*timing info*/
        /*hsync_hfb    hsync_act    hsync_hhb*/
        {0,            1920,        0,
        /*vsync0_vhb vsync0_act vsync0_hhb*/
         0,            1080,        0,
        /*vsync1_vhb vsync1_act vsync1_hhb*/
         0,            0,            0}
        },
        /*whether use isp*/
        VI_PATH_BYPASS,
        /*data type*/
        VI_DATA_TYPE_YUV,
        HI_FALSE

    };

}

void HinkInputVi::onStart()
{
    HI_S32 s32Ret = HI_SUCCESS;
//    stDevAttr.stSynCfg.stTimingBlank.u32HsyncAct = data["width"].toUInt();
//    stDevAttr.stSynCfg.stTimingBlank.u32VsyncVact = data["height"].toUInt();
    if(!(data["interface"].toString().isEmpty())){
        //qDebug()<<Hink::getConfig()["interfaceV"].toMap()[data["interface"].toString()].toMap();
        if(!(Hink::getConfig()["interfaceV"].toMap()[data["interface"].toString()].toMap().isEmpty())){
            QVariantMap interfaceV = Hink::getConfig()["interfaceV"].toMap()[data["interface"].toString()].toMap();
            data["did"] = interfaceV["did"].toInt();
            data["cid"] = interfaceV["cid"].toInt();
            data["dstFramerate"] = interfaceV["dstFramerate"].toInt();
            data["avalible"]= true;

            vi_setMask(data["did"].toInt(),&stDevAttr);



            s32Ret = HI_MPI_VI_SetDevAttr(data["did"].toInt(),&stDevAttr);
            if(HI_SUCCESS != s32Ret){
                qDebug("HI_MPI_VI_SetDevattr failed with %#x!",s32Ret);
            }

            s32Ret = HI_MPI_VI_EnableDev(data["did"].toInt());
            if(HI_SUCCESS != s32Ret){
                qDebug("HI_MPI_VI_EnableDev failed with %#x!",s32Ret);
            }

            VI_CHN_ATTR_S stChnAttr;


            stChnAttr.stCapRect.s32X = 0;
            stChnAttr.stCapRect.s32Y = 0;
            stChnAttr.enCapSel = VI_CAPSEL_BOTH;
            stChnAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;   /* sp420 or sp422 */
            stChnAttr.bMirror = HI_FALSE;
            stChnAttr.bFlip   = HI_FALSE;
            stChnAttr.stCapRect.u32Width= data["width"].toUInt();
            stChnAttr.stCapRect.u32Height= data["height"].toUInt();
            stChnAttr.stDestSize.u32Width= data["width"].toUInt();
            stChnAttr.stDestSize.u32Height= data["height"].toUInt();
            stChnAttr.enScanMode = VI_SCAN_PROGRESSIVE;		//VI_SCAN_INTERLACED
            stChnAttr.s32SrcFrameRate = -1 /*data["srcFramerate"].toInt()*/;//-1;
            stChnAttr.s32DstFrameRate = -1 /*data["dstFramerate"].toInt()*/;//-1;

            s32Ret = HI_MPI_VI_SetChnAttr(data["cid"].toInt(),&stChnAttr);
            if(HI_SUCCESS != s32Ret){
                    qDebug("HI_MPI_VI_SetChnAttr failed with %#x!",s32Ret);
            }else{
                    qDebug("HI_MPI_VI_SetChnAttr(%d) ready.",data["cid"].toInt());
            }

            s32Ret = HI_MPI_VI_EnableChn(data["cid"].toInt());
            if (s32Ret != HI_SUCCESS)
            {
                    qDebug("Enable chn failed with error code %#x!", s32Ret);
            }

            VPSS_GRP_ATTR_S stVPSSAttr;
            SIZE_S stSize;
            stSize.u32Width = data["width"].toUInt();
            stSize.u32Height = data["height"].toUInt();
            hink_vpss_getDefGrpAttr(&stVPSSAttr,stSize.u32Width,stSize.u32Height);
            hink_vpss_quickStart(1,0,stSize.u32Width,stSize.u32Height);
            MPP_CHN_S stSrcChn;
            MPP_CHN_S stDestChn;
            stSrcChn.enModId = HI_ID_VIU;
            stSrcChn.s32DevId = 0/*data["did"].toInt()*/;
            stSrcChn.s32ChnId = data["cid"].toInt();

            stDestChn.enModId = HI_ID_VPSS;
            stDestChn.s32DevId = 1;
            stDestChn.s32ChnId  = 0;

            HINK_CHECK_RET(HI_MPI_SYS_Bind(&stSrcChn, &stDestChn), "HI_MPI_SYS_Bind");
        }

    }


}

void HinkInputVi::onSetData(QVariantMap map)
{

}
