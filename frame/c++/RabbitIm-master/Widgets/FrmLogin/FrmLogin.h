#ifndef FRMLOGIN_H
#define FRMLOGIN_H

#include <QFrame>
#include <QMessageBox>
#include <QTimer>
#include "../../MainWindow.h"
#include "../DlgRegister/DlgRegister.h"

class MainWindow;

namespace Ui {
class CFrmLogin;
}

class CFrmLogin : public QFrame
{
    Q_OBJECT

public:
    explicit CFrmLogin(QWidget *parent = 0);
    ~CFrmLogin();

    int SetPrompt(QString szPrompt);
    int SetLoginInformation(QString szName, QString szPassword);
    int SaveConf();

private slots:
    void on_pbOk_clicked();
    void on_pbClose_clicked();
    void on_pbRegitster_clicked();

    void on_chkLogin_stateChanged(int state);
    void slotActionGroupStatusTriggered(QAction*pAct);

    void on_cmbUser_currentIndexChanged(int index);
    
    void on_pbSet_clicked();

    void slotClientError(CClient::ERROR_TYPE e);

    void on_cmbUser_currentTextChanged(const QString &arg1);

    void on_lnPassword_returnPressed();
    
public slots:
    void slotRefresh();

private:
    void changeEvent(QEvent *);
    void ComposeAvatar(const QString &id);

private:
    Ui::CFrmLogin *ui;

    QTimer m_tmAutoLogin;//用于自动登录  

    //密码加密函数  
    QString EncryptPassword(QString szPassword);
    QString DecryptPassword(QString szPassword);

    //初始化状态按钮  
    int ReinitStateButton();
    QMenu m_StateMenu;
    QMap<CUserInfo::USER_INFO_STATUS, QAction*> m_ActionStatus;
    QActionGroup m_ActionGroupStatus;
    CUserInfo::USER_INFO_STATUS m_Status;
};

#endif // FRMLOGIN_H
