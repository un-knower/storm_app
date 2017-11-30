#ifndef TREEUSERLIST_H
#define TREEUSERLIST_H

#include <QTreeView>
#include <QTime>
#include <QStandardItem>

class CCustomTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit CCustomTreeView(QWidget *parent = 0);

protected:
    //显示上下文件菜单(右键菜单)  
    //控件contextMenuPolicy属性要设置为DefaultContextMenu，才能触这个事件  
    //如果设置为CustomContextMenu，触发槽on_tvUsers_customContextMenuRequested  
    void contextMenuEvent(QContextMenuEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);
    void resizeEvent(QResizeEvent * event);
    void changeEvent(QEvent*);

private:
    QTime m_MousePressTime;

};

#endif // TREEUSERLIST_H
