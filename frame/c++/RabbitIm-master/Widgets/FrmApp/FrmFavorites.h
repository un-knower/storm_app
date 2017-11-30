#ifndef FRMFAVORITES_H
#define FRMFAVORITES_H

#include <QMenu>
#include <QWidget>
#include <QStandardItemModel>
#include "../FrmCustom/CustomTreeView.h"

namespace Ui {
class CFrmFavorites;
}

class CFrmFavorites : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmFavorites(QWidget *parent = 0);
    ~CFrmFavorites();

public slots:
    void slotAddFavorites(const QString& szApp);
    void slotRemoveFavorites(const QString& szApp);

private:
    void resizeEvent(QResizeEvent *e);
    
private:
    QMenu m_Menu;      //列表操作菜单  
    int InitMenu();
private slots:
    //显示上下文件菜单(右键菜单)  
    //控件contextMenuPolicy属性要设置为CustomContextMenu，才能触customContextMenuRequested事件，再联接这个槽  
    //如果设置为DefaultContextMenu，则触发右键菜单事件 void contextMenuEvent(QContextMenuEvent * event);  
    void slotCustomContextMenuRequested(const QPoint &pos);
    //打开应用  
    void slotOpenApp();   
    void slotCloseApp();
    void slotAboutApp();
    void slotRemoveApp();

private slots:
    //树形列表控件点击事件 
    void clicked(const QModelIndex & index);
    //树形列表控件双击事件  
    void doubleClicked(const QModelIndex & index);
    
private:
    int InitList();

    enum _USERLIST_ROLE
    {
        ROLE_GROUP = Qt::UserRole + 1,
        ROLE_APPLICATION = ROLE_GROUP + 1
    };
private:
    Ui::CFrmFavorites *ui;

    CCustomTreeView m_List;
    QStandardItemModel *m_pModel;           //好友列表树型控件
};

#endif // FRMFAVORITES_H
