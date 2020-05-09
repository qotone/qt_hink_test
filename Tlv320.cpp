#include "Tlv320.h"
#include "tlv320aic31.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <QDebug>
#include "hi_type.h"
#include "hi_comm_aio.h"

#define TLV320_FILE "/dev/tlv320aic31"

Tlv320::Tlv320()
{

}

int Tlv320::init(bool isMaster, unsigned int sampleRate)
{
    int fd = -1;
    HI_BOOL bMaster = HI_TRUE;
    HI_BOOL b44100HzSeries = HI_FALSE;
    HI_S32 s32Samplerate;
    Audio_Ctrl audio_ctrl;
    int vol = 100;
    fd = ::open(TLV320_FILE,O_RDWR);
    if(fd < 0){
        qDebug()<<"tlv320 file not exist!";
        return -1;
    }
    audio_ctrl.chip_num = 0;
    if(::ioctl(fd,SOFT_RESET,&audio_ctrl)){
        qDebug()<<"tlv320 soft reset failed.";
        ::close(fd);
        return -1;
    }

    switch (sampleRate) {
        case 8000:
        s32Samplerate = AC31_SET_8K_SAMPLERATE;
        break;
    case 12000:
        s32Samplerate = AC31_SET_12K_SAMPLERATE;
        break;
    case 11025:
        b44100HzSeries = HI_TRUE;
        s32Samplerate = AC31_SET_11_025K_SAMPLERATE;
        break;
    case 16000:
        s32Samplerate = AC31_SET_16K_SAMPLERATE;
        break;
    case 22050:
        b44100HzSeries = HI_TRUE;
        s32Samplerate = AC31_SET_22_05K_SAMPLERATE;
        break;
    case 24000:
        s32Samplerate = AC31_SET_24K_SAMPLERATE;
        break;
    case 32000:
        s32Samplerate = AC31_SET_32K_SAMPLERATE;
        break;
    case 44100:
        s32Samplerate = AC31_SET_44_1K_SAMPLERATE;
        b44100HzSeries = HI_TRUE;
        break;
    case 48000:
        s32Samplerate = AC31_SET_48K_SAMPLERATE;
        break;
    default:
        qDebug()<<"tlv320 not support this samplerate.";
        close(fd);
        return -1;
    }

    if(isMaster)
        bMaster = HI_TRUE;
    else
        bMaster = HI_FALSE;

    /* set master/slave mode, 1:master*/
         audio_ctrl.ctrl_mode = bMaster;
         audio_ctrl.if_44100hz_series = b44100HzSeries;
         audio_ctrl.sample = s32Samplerate;
         audio_ctrl.sampleRate = sampleRate;
         ioctl(fd,SET_CTRL_MODE,&audio_ctrl);

         /* set transfer mode 0:I2S 1:PCM */
         audio_ctrl.trans_mode = 0;
         if (ioctl(fd,SET_TRANSFER_MODE,&audio_ctrl))
         {
             qDebug("set tlv320aic31 trans_mode err\n");
             close(fd);
             return -1;
         }

         /*set sample of DAC and ADC */
         if (ioctl(fd,SET_DAC_SAMPLE,&audio_ctrl))
         {
             qDebug("ioctl err1\n");
             close(fd);
             return -1;
         }

         if (ioctl(fd,SET_ADC_SAMPLE,&audio_ctrl))
         {
             qDebug("ioctl err2\n");
             close(fd);
             return -1;
         }

         /*set volume control of left and right DAC */
         audio_ctrl.if_mute_route = 0;
         audio_ctrl.input_level = 0;
         ioctl(fd,LEFT_DAC_VOL_CTRL,&audio_ctrl);
         ioctl(fd,RIGHT_DAC_VOL_CTRL,&audio_ctrl);

         /*Right/Left DAC Datapath Control */
         audio_ctrl.if_powerup = 1;/*Left/Right DAC datapath plays left/right channel input data*/
         ioctl(fd,LEFT_DAC_POWER_SETUP,&audio_ctrl);

         /* data bit width (0:16bit 1:20bit 2:24bit 3:32bit) */
         audio_ctrl.data_length = 0;
         ioctl(fd,SET_DATA_LENGTH,&audio_ctrl);

         /*DACL1 TO LEFT_LOP/RIGHT_LOP VOLUME CONTROL 82 92*/
         audio_ctrl.if_mute_route = 1;/* route*/
         audio_ctrl.input_level = vol; /*level control*/
         ioctl(fd,DACL1_2_LEFT_LOP_VOL_CTRL,&audio_ctrl);
         ioctl(fd,DACR1_2_RIGHT_LOP_VOL_CTRL,&audio_ctrl);

         /* LEFT_LOP/RIGHT_LOP OUTPUT LEVEL CONTROL 86 93*/
         audio_ctrl.if_mute_route = 1;
         audio_ctrl.if_powerup = 1;
         audio_ctrl.input_level = 0;
         ioctl(fd,LEFT_LOP_OUTPUT_LEVEL_CTRL,&audio_ctrl);
         ioctl(fd,RIGHT_LOP_OUTPUT_LEVEL_CTRL,&audio_ctrl);

         /*config AD*/
         /* LEFT/RIGHT ADC PGA GAIN CONTROL 15 16*/
         audio_ctrl.if_mute_route =0;
         audio_ctrl.input_level = 0;
         ioctl(fd,LEFT_ADC_PGA_CTRL,&audio_ctrl);
         ioctl(fd,RIGHT_ADC_PGA_CTRL,&audio_ctrl);

         /*INT2L TO LEFT/RIGTH ADCCONTROL 17 18*/
         audio_ctrl.input_level = 0;
         ioctl(fd,IN2LR_2_LEFT_ADC_CTRL,&audio_ctrl);
         ioctl(fd,IN2LR_2_RIGTH_ADC_CTRL,&audio_ctrl);

         /*IN1L_2_LEFT/RIGTH_ADC_CTRL 19 22*/
         /*audio_ctrl.input_level = 0xf;
         audio_ctrl.if_powerup = 1;
         printf("audio_ctrl.input_level=0x%x,audio_ctrl.if_powerup=0x%x\n",audio_ctrl.input_level,audio_ctrl.if_powerup);
         if (ioctl(s_fdTlv,IN1L_2_LEFT_ADC_CTRL,&audio_ctrl)==0)
             perror("ioctl err\n");
         getchar();
         printf("audio_ctrl.input_level=0x%x,audio_ctrl.if_powerup=0x%x\n",audio_ctrl.input_level,audio_ctrl.if_powerup);
         ioctl(s_fdTlv,IN1R_2_RIGHT_ADC_CTRL,&audio_ctrl);
         getchar();
         printf("set 19 22\n");*/

        audio_ctrl.if_mute_route = 1;
        audio_ctrl.input_level = 9;
        audio_ctrl.if_powerup = 1;
        ioctl(fd,HPLOUT_OUTPUT_LEVEL_CTRL,&audio_ctrl);
        ioctl(fd,HPROUT_OUTPUT_LEVEL_CTRL,&audio_ctrl);

        close(fd);
        qDebug("Set aic31 ok: bMaster = %d, enSamplerate = %d\n",
                 bMaster,  sampleRate);
        return 0;

}
