QT += core network testlib websockets xml
QT -= gui

CONFIG += c++14 console
CONFIG -= app_bundle


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

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


INCLUDEPATH += $$HISI_MPP_LIBS_PATH/include
INCLUDEPATH += $$JCON_LIB_PATH
INCLUDEPATH += $$HINK_LIB_PATH/include
INCLUDEPATH += $$UVC_LIB_PATH/include
# ireader libs include
INCLUDEPATH += $$MEDIA_SERVER_PATH/librtp/include
INCLUDEPATH += $$MEDIA_SERVER_PATH/librtp/payload
INCLUDEPATH += $$SDK_LIB_PATH/include


LIBS += -L$$HISI_MPP_LIBS_PATH/lib -lmpi -lhdmi -ltde -lVoiceEngine  -ljpeg6b   -live -lupvqe  -ldnvqe
LIBS += -L$$JCON_LIB_PATH/lib -ljcon
LIBS += -L$$HINK_LIB_PATH/lib -lhink
LIBS += -L$$UVC_LIB_PATH/lib -luvc
#ireader libs
LIBS += -L$$SDK_LIB_PATH/lib -laio -lhttp
LIBS += -L$$MEDIA_SERVER_PATH/lib -lrtp




SOURCES += \
        Config.cpp \
        Hink.cpp \
        HinkDecodeV.cpp \
        HinkEncodeV.cpp \
        HinkInputAi.cpp \
        HinkInputV4l2.cpp \
        HinkInputVi.cpp \
        HinkObject.cpp \
        HinkOutputAo.cpp \
        HinkOutputVo.cpp \
        Json.cpp \
        Sink.cpp \
        channel.cpp \
        example_service.cpp \
        group.cpp \
        grouprpc.cpp \
        main.cpp \
        rpc.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Config.h \
    Hink.h \
    HinkDecodeV.h \
    HinkEncodeV.h \
    HinkInputAi.h \
    HinkInputV4l2.h \
    HinkInputVi.h \
    HinkObject.h \
    HinkOutputAo.h \
    HinkOutputVo.h \
    Json.h \
    Sink.h \
    channel.h \
    example_service.h \
    group.h \
    grouprpc.h \
    rpc.h
