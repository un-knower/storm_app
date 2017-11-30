#-------------------------------------------------
#
# Project created by QtCreator 2015-12-18T13:15:27
#
#-------------------------------------------------

QT *= core

TARGET = ProtocolQXmpp
TEMPLATE = lib
DISTFILES += \
    ProtocolQXmpp.json

RABBITIM_PLUG_NAME=CPluginProtocolQXMPP
include(../ConfigProtocol.pri)

SOURCES += \
    ClientXmpp.cpp \
    ManageGroupChatQxmpp.cpp \
    ManageUserQXmpp.cpp \
    UserInfoXmpp.cpp \
    GroupChatQxmpp.cpp \
    PluginProtocolQXmpp.cpp \
    FileTransferQXmpp.cpp

HEADERS += \
    ClientXmpp.h \
    ManageGroupChatQxmpp.h \
    ManageUserQXmpp.h \
    UserInfoXmpp.h \
    GroupChatQxmpp.h \
    PluginProtocolQXmpp.h \
    FileTransferQXmpp.h

equals(RABBITIM_USE_WEBRTC, 1){
    SOURCES += QXmppWebRtcIq.cpp \
        ManageCallWebrtcXmpp.cpp \
        CallObjectQXmppWebrtc.cpp \ 
        QXmppCallWebrtcManager.cpp \ 
        WebrtcConductor.cpp \
        VideoRenderer.cpp \ 
        WebrtcFilter.cpp \ 
        WebrtcQtSocketServer.cpp


    HEADERS += \
        QXmppWebRtcIq.h \
        ManageCallWebrtcXmpp.h \ 
        CallObjectQXmppWebrtc.h \ 
        QXmppCallWebrtcManager.h \ 
        WebrtcConductor.h \
        MediaConstraints.h \
        VideoRenderer.h \
        WebrtcFilter.h \ 
        WebrtcQtSocketServer.h
}else{
    SOURCES +=  ManageCallXmpp.cpp \
        CallObjectQXmpp.cpp
    
    HEADERS +=  ManageCallXmpp.h \
        CallObjectQXmpp.h 
}
