#include "FrmLbsMain.h"
#include "ui_FrmLbsMain.h"
#include "Tool.h"

CFrmLbsMain::CFrmLbsMain(QWidget *parent) :
    QFrame(parent),
    m_Motion(this),
    m_Track(this),
    ui(new Ui::CFrmLbsMain)
{
    ui->setupUi(this);
    CTool::SetWindowsGeometry(this);
    ui->tabWidget->clear();
    
    ui->tabWidget->addTab(&m_Motion, QIcon(":/png/motion"), tr("Motion"));
    ui->tabWidget->addTab(&m_Track, QIcon(), tr("Track"));
    
    bool check = connect(&m_Motion, SIGNAL(sigClose()),
                 SLOT(close()));
    Q_ASSERT(check);
}

CFrmLbsMain::~CFrmLbsMain()
{
    delete ui;
}
