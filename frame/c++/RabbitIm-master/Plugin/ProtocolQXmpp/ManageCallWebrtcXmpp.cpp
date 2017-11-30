#include "WebrtcConductor.h"

#include "ManageCallWebrtcXmpp.h"
#include "ClientXmpp.h"
#include "Global/Global.h"
#include "UserInfoXmpp.h"
#include "QXmppCallWebrtcManager.h"
#include "qxmpp/QXmppUtils.h"

CManageCallWebrtcXmpp::CManageCallWebrtcXmpp(QObject *parent)
    : CManageCall(parent)
{
}

CManageCallWebrtcXmpp::~CManageCallWebrtcXmpp()
{
    LOG_MODEL_DEBUG("CManageCallWebrtcXmpp",
                    "CManageCallWebrtcXmpp::~CManageCallWebrtcXmpp()");
}

int CManageCallWebrtcXmpp::LoginInit(const QString &szId)
{
    int nRet = 0;
    nRet = CManageCall::LoginInit(szId);
    if(nRet)
        return nRet;

    QSharedPointer<CClient> client = GETMANAGER->GetClient();
    CClientXmpp* pClient = (CClientXmpp*)client.data();
    if(!pClient)
    {
        LOG_MODEL_ERROR("Call", "pClient is null");
        return -1;
    }

    QXmppCallWebrtcManager* pCallManager = pClient->m_Client.findExtension<QXmppCallWebrtcManager>();
    if(!pCallManager)
    {
        pClient->m_Client.addExtension(new QXmppCallWebrtcManager(this));
        pCallManager = pClient->m_Client.findExtension<QXmppCallWebrtcManager>();
    }
    Q_ASSERT(pCallManager);

    CWebrtcConductor::InitWebrtcGlobal();
    return nRet;
}

int CManageCallWebrtcXmpp::LogoutClean()
{
    CManageCall::LogoutClean();
    
    QSharedPointer<CClient> client = GETMANAGER->GetClient();
    CClientXmpp* pClient = (CClientXmpp*)client.data();
    if(!pClient)
    {
        LOG_MODEL_ERROR("Call", "pClient is null");
        return -1;
    }

    QXmppCallWebrtcManager* pCallManager = pClient->m_Client.findExtension<QXmppCallWebrtcManager>();
    if(pCallManager)
    {
        pClient->m_Client.removeExtension(pCallManager); //它在内部delete指针  
    }

    CWebrtcConductor::CleanWebrtcGlobal();
    return 0;
}

int CManageCallWebrtcXmpp::Call(const QString &szId, bool bVideo)
{
    //检查是否是好友  
    QSharedPointer<CUser> roster = GLOBAL_USER->GetUserInfoRoster(szId);
    if(roster.isNull())
    {
        LOG_MODEL_ERROR("CManageCall", "Don't get roster:%s", qPrintable(szId));
        return -1;
    }
    QSharedPointer<CUserInfo> info = roster->GetInfo();
    CUserInfoXmpp* pInfo = (CUserInfoXmpp*)info.data();
    if(pInfo->GetResource().isEmpty())
    {
        LOG_MODEL_ERROR("Call", "CClientXmpp::Call the roster resource is null");
        roster->GetMessage()->AddMessage(szId,
                tr("The roster is offline, don't launch a call."), true);
        emit GET_CLIENT->sigMessageUpdate(szId);
        return -2;
    }

    return CManageCall::Call(szId, bVideo);
}

int CManageCallWebrtcXmpp::OnCall(const QString &szId,
       QSharedPointer<CCallObject> &call, bool bVideo)
{
    QSharedPointer<CClient> client = GETMANAGER->GetClient();
    CClientXmpp* pClient = (CClientXmpp*)client.data();
    if(!pClient)
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp", "pClient is null");
        return -1;
    }
    
    QXmppCallWebrtcManager* pCallManager = pClient->m_Client.findExtension<QXmppCallWebrtcManager>();
    if(!pCallManager)
        return -2;
    
    QSharedPointer<CUser> roster = GLOBAL_USER->GetUserInfoRoster(szId);
    if(roster.isNull())
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp",
                        "CManageCallWebrtcXmpp::OnCall the roster is null");
        return -3;
    }

    //因为 xmpp iq 协议需要用户的资源（jid）才会进行转发  
    QSharedPointer<CUserInfo> info = roster->GetInfo();
    CUserInfoXmpp* pInfo = (CUserInfoXmpp*)info.data();
    QSharedPointer<CCallObjectQXmppWebrtc> callWebrtc(
                new CCallObjectQXmppWebrtc(pInfo->GetJid(), bVideo, pCallManager));
    if(callWebrtc.isNull())
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp", "CManageCallWebrtcXmpp::CallVideo fail");
        return -3;
    }

    call = callWebrtc;

    return callWebrtc->Call();
}

int CManageCallWebrtcXmpp::WebrtcIqReceived(QXmppWebRtcIq &iq)
{
    if(iq.type() != QXmppIq::Set)
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp", "CManageCallWebrtcXmpp"
                           "::WebrtcIqReceived iq type isn't set");
        return -1;
    }
    QXmppWebRtcIq::ACTION action = iq.GetAction();
    if(QXmppWebRtcIq::Call == action)
    {
        OnReciveCall(iq);
        return 0;
    }
    
    QString szId = QXmppUtils::jidToBareJid(iq.from());
    QMap<QString, QSharedPointer<CCallObject> >::iterator it = m_Call.find(szId);
    if(m_Call.end() == it)
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp", "CManageCallWebrtcXmpp"
            "::WebrtcIqReceived don't find %s", szId.toStdString().c_str());
        return -2;
    }
    QSharedPointer<CCallObject> call = it.value();
    if(call.isNull())
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp", "CManageCallWebrtcXmpp"
            "::WebrtcIqReceived don't find %s", szId.toStdString().c_str());
        return -3;
    }

    CCallObjectQXmppWebrtc* pCall = (CCallObjectQXmppWebrtc*)call.data();
    switch (action)
    {
    case QXmppWebRtcIq::Accept:
        pCall->ReciveAccept();
        break;
    case QXmppWebRtcIq::Stop:
        pCall->ReciveStop();
        break;
    case QXmppWebRtcIq::DescriptionInfo:
        pCall->ReciveSeesionDescription(iq);
        break;
    case QXmppWebRtcIq::TransportInfo:
        pCall->ReciveTransportInfo(iq);
        break;
    default:
        break;
    }
    return 0;
}

int CManageCallWebrtcXmpp::OnReciveCall(QXmppWebRtcIq &iq)
{
    QString szId = iq.from();
    LOG_MODEL_DEBUG("CManageCallWebrtcXmpp", "szId:%s", szId.toStdString().c_str());
    bool bVideo = iq.IsVideo();
    QSharedPointer<CClient> client = GETMANAGER->GetClient();
    CClientXmpp* pClient = (CClientXmpp*)client.data();
    if(!pClient)
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp", "pClient is null");
        return -1;
    }
    QXmppCallWebrtcManager* pCallManager = pClient->m_Client.findExtension<QXmppCallWebrtcManager>();
    if(!pCallManager)
        return -2;
    QSharedPointer<CCallObjectQXmppWebrtc> call(new CCallObjectQXmppWebrtc(szId, bVideo, pCallManager));
    if(call.isNull())
    {
        LOG_MODEL_ERROR("CManageCallWebrtcXmpp", "CManageCallWebrtcXmpp::CallVideo fail");
        return -3;
    }
    slotCallReceived(call);    
    return 0;
}
