QT += core network testlib websockets xml
QT -= gui

CONFIG += c++14 console
CONFIG -= app_bundle


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS HAVE_ALSA

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#hi3531d_v600 or hi3531d_v500
# defined at the project setup
#HISI_VERSION = hi3531d_v500
CONFIG += $$HISI_VERSION
hi3531d_v500{
HISI_MPP_LIBS_PATH = /media/luoyang/Workspace/Hi3531DV100_SDK_V1.0.4.0/mpp
}else{
HISI_MPP_LIBS_PATH = /media/luoyang/Workspace/Hi3531DV100_SDK_V2.0.4.0/mpp
}
CROSS_LIBS_PATH = /home/luoyang/Dev/cross/$$HISI_VERSION

JCON_LIB_PATH = $$CROSS_LIBS_PATH/jcon
HINK_LIB_PATH = $$CROSS_LIBS_PATH/hink
UVC_LIB_PATH = $$CROSS_LIBS_PATH/libuvc
SDK_LIB_PATH = $$CROSS_LIBS_PATH/sdk
MEDIA_SERVER_PATH = $$CROSS_LIBS_PATH/media-server
TINYALSA_LIB_PATH = $$CROSS_LIBS_PATH/tinyalsa
ORTP_LIB_PATH = $$CROSS_LIBS_PATH/ortp
ALSA_LIB_PATH = $$CROSS_LIBS_PATH/alsa

INCLUDEPATH += $$HISI_MPP_LIBS_PATH/include
INCLUDEPATH += $$JCON_LIB_PATH
INCLUDEPATH += $$HINK_LIB_PATH/include
INCLUDEPATH += $$UVC_LIB_PATH/include
# tinyalsa
INCLUDEPATH += $$TINYALSA_LIB_PATH/include
# ireader libs include
INCLUDEPATH += $$MEDIA_SERVER_PATH/librtp/include
INCLUDEPATH += $$MEDIA_SERVER_PATH/librtp/payload
INCLUDEPATH += $$SDK_LIB_PATH/include
# ortp libs include
INCLUDEPATH += $$ORTP_LIB_PATH/include
# alsa libs include
INCLUDEPATH += $$ALSA_LIB_PATH/include

LIBS += -L$$HISI_MPP_LIBS_PATH/lib -lmpi -lhdmi -ltde -lVoiceEngine  -ljpeg6b   -live -lupvqe  -ldnvqe -laacdec -laacenc
LIBS += -L$$JCON_LIB_PATH/lib -ljcon
LIBS += -L$$HINK_LIB_PATH/lib -lhink
LIBS += -L$$UVC_LIB_PATH/lib -luvc
LIBS += -L$$TINYALSA_LIB_PATH/lib -ltinyalsa
LIBS += -L$$ALSA_LIB_PATH/lib -lasound
#ireader libs
LIBS += -L$$SDK_LIB_PATH/lib -laio -lhttp
LIBS += -L$$MEDIA_SERVER_PATH/lib -lrtp
# ortp libs
LIBS += -L$$ORTP_LIB_PATH/lib -lortp

LIBS += -lpthread



SOURCES += \
        ALSACapture.cpp \
        AlsaPlayback.cpp \
        AlsaRecorder.cpp \
        Config.cpp \
        Hink.cpp \
        HinkDecodeA.cpp \
        HinkDecodeV.cpp \
        HinkEncodeA.cpp \
        HinkEncodeV.cpp \
        HinkInputAi.cpp \
        HinkInputAlsa.cpp \
        HinkInputTinyalsa.cpp \
        HinkInputV4l2.cpp \
        HinkInputVi.cpp \
        HinkObject.cpp \
        HinkOutputAlsa.cpp \
        HinkOutputAo.cpp \
        HinkOutputVo.cpp \
        Json.cpp \
        Sink.cpp \
        SoundBuffer.cpp \
        SoundRecorder.cpp \
        Source.cpp \
        Tlv320.cpp \
        TokenBucketSink.cpp \
        audio_aac_adp.c \
        channel.cpp \
        example_service.cpp \
        filewrite.cpp \
        group.cpp \
        grouprpc.cpp \
        logger.cpp \
        main_usbcam.cpp \
        rpc.cpp \
        token_bucket.c \
        unixsignalhandler.cpp \
        unixsignalnotifier.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ALSACapture.h \
    AlsaPlayback.h \
    AlsaRecorder.h \
    Config.h \
    Hink.h \
    HinkDecodeA.h \
    HinkDecodeV.h \
    HinkEncodeA.h \
    HinkEncodeV.h \
    HinkInputAi.h \
    HinkInputAlsa.h \
    HinkInputTinyalsa.h \
    HinkInputV4l2.h \
    HinkInputVi.h \
    HinkObject.h \
    HinkOutputAlsa.h \
    HinkOutputAo.h \
    HinkOutputVo.h \
    Json.h \
    Sink.h \
    SoundBuffer.h \
    SoundRecorder.h \
    Source.h \
    Tlv320.h \
    TokenBucket.h \
    TokenBucketSink.h \
    channel.h \
    example_service.h \
    filewrite.h \
    group.h \
    grouprpc.h \
    logger.h \
    rpc.h \
    token_bucket.h \
    unixsignalhandler.h \
    unixsignalnotifier.h
