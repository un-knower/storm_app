#ifndef DLGOPTIONS_H
#define DLGOPTIONS_H

#include <QDialog>

namespace Ui {
class CDlgOptions;
}

class CDlgOptions : public QDialog
{
    Q_OBJECT
public:
    explicit CDlgOptions(QWidget *parent = 0);
    virtual ~CDlgOptions();

signals:
    void sigRefresh();

private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    void on_pbLocalUserColor_clicked();
    void on_pbRosterColor_clicked();
    void on_pbLocalUserMessageColor_clicked();
    void on_pbRosterMessageColor_clicked();

    void on_pbUnreadMessageCountColor_clicked();

    void on_pbResetConfigure_clicked();

protected:
    virtual void showEvent(QShowEvent* );
    virtual void closeEvent(QCloseEvent*);
    virtual void changeEvent(QEvent*);
    virtual void resizeEvent(QResizeEvent *e);

private:
    Ui::CDlgOptions *ui;
};

#endif // DLGOPTIONS_H
