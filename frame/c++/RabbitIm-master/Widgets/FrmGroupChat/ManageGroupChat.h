#ifndef MANAGEGROUPCHAT_H
#define MANAGEGROUPCHAT_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include "GroupChat.h"
#include "Manage/Manage.h"
#include "Global/GlobalDir.h"

/**
  *@defgroup RABBITIM_INTERFACE_MANAGEGROUPCHAT 组管理接口类  
  *@ingroup RABBITIM_INTERFACE_MANAGER
  *@brief 组管理接口类  
  */

/**
 * @ingroup RABBITIM_INTERFACE_MANAGEGROUPCHAT RABBITIM_INTERFACE
 * @brief 组管理接口类  
 */
class RABBITIM_SHARED_LIBRARY CManageGroupChat : public CManage
{
    Q_OBJECT
public:
    explicit CManageGroupChat(QObject *parent = 0);
    virtual ~CManageGroupChat();

    /**
     * @brief 登录后初始化  
     *
     * @param szId
     * @return int
     */
    virtual int LoginInit(const QString& szId);
    virtual int LogoutClean();

    virtual int Create(
                   const QString &szName,
                   const QString &szSubject,
                   const QString &szPassword = QString(),
                   const QString &szDescription = QString(),
                   bool bProtracted = false,
                   bool bPrivated = false,
                   const QString &szNick = QString()
            ) = 0;
    virtual int Join(const QString &szId, const QString &szPassword = QString(), const QString &szNick = QString()) = 0;
    virtual QSharedPointer<CGroupChat> Get(const QString &szId) = 0;
    virtual bool IsJoined(const QString &szId) = 0;

signals:
    void sigJoined(const QString& szId);
    void sigLeave(const QString& szId);
    void sigError(const QString& szId, CGroupChat::Condition c, int errorcode);
    void sigInvitation(const QString &szGroupChatId, const QString &szInviter, const QString& szReason);
    void sigUpdateMessage(const QString &szId);
    void sigMessageClean(const QString &szId);
    void sigGroupChatInformationChange(const QString &szId);

public slots:
    int slotLeave(const QString& szId);
    int slotError(const QString& szId, CGroupChat::Condition c, int errorcode);
    
    /// This signal is emitted when an invitation to a chat room is received.
    virtual void slotInvitationReceived(const QString &Id, const QString &inviter, const QString &reason);

protected:
    /**
     * @brief 从存储中加载信息  
     *
     * @param szLocaleJid  
     * @return int  
     */
    virtual int LoadFromStorage(const QString &szId);
    /**
     * @brief 保存信息到存储  
     *
     * @return int  
     */
    virtual int SaveToStorage();

protected:
    QMap<QString, QSharedPointer<CGroupChat> > m_GroupChat;

};

#endif // MANAGEGROUPCHAT_H
