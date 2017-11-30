#include "FrmPlayer.h"
#include <QPainter>
#include <QImage>
#include "../../Global/Global.h"
#include "Media/Camera/CameraFactory.h"

CFrmPlayer::CFrmPlayer(QWidget *parent, Qt::WindowFlags f) :
    QWidget(parent, f)
{
    bool check = connect(&m_Process,
                         SIGNAL(sigFrameConvertedToRGB32Frame(QVideoFrame)),
                         SLOT(slotPresentRGB32(QVideoFrame)));
    Q_ASSERT(check);
}

CFrmPlayer::~CFrmPlayer()
{
    LOG_MODEL_DEBUG("Video", "CFrmPlayer::~CFrmPlayer");
}

void CFrmPlayer::changeEvent(QEvent *)
{}

void CFrmPlayer::paintEvent(QPaintEvent *)
{
    if(this->isHidden())
        return;
    QPainter painter(this);

    if(!m_Image.isNull())
    {
        painter.drawImage(this->rect(), m_Image);
        return;
    }

    if(!m_VideoFrame.isValid())
        return;
    if(!m_VideoFrame.map(QAbstractVideoBuffer::ReadOnly))
        return;
    do{
        QImage::Format f = QVideoFrame::imageFormatFromPixelFormat(
                    m_VideoFrame.pixelFormat());
        if(QImage::Format_Invalid == f)
            break;
        //LOG_MODEL_DEBUG("CFrmPlayer", "m_VideoFrame.bytesPerLine():%d", m_VideoFrame.bytesPerLine());
        QImage image(m_VideoFrame.bits(),
                     m_VideoFrame.width(),
                     m_VideoFrame.height(),
                     m_VideoFrame.width() << 2,
                     f);
        painter.drawImage(this->rect(), image);
    }while(0);
    m_VideoFrame.unmap();
}

void CFrmPlayer::slotPresent(const QVideoFrame &frame)
{
    if(!(frame.pixelFormat() == QVideoFrame::Format_RGB32 
         || frame.pixelFormat() == QVideoFrame::Format_ARGB32))
    {
        m_Process.slotFrameConvertedToRGB32(frame);
        return;
    }
    m_VideoFrame = frame;
    update();
}

void CFrmPlayer::slotPresent(const QImage &frame)
{
    m_Image = frame;
    update();
}

void CFrmPlayer::slotPresentRGB32(const QVideoFrame &frame)
{
    m_VideoFrame = frame;
    update();
}

void CFrmPlayer::mouseReleaseEvent(QMouseEvent *)
{
#ifdef DEBUG
    //TestCamera();
#endif
}

#ifdef DEBUG
#include <vector>
#include "FrameProcess.h"
/***********************************************************************/
/* 视频处理类                                                            */
/***********************************************************************/
class Hander : public CCamera::CHanderFrame
{
public:
    Hander(CFrmPlayer* player, CFrameProcess* pProcess)
    {
        m_pPlayer = player;
        m_pProcess = pProcess;
    }

    virtual int OnFrame(const QVideoFrame &frame)
    {
        //直接调用  
        m_pPlayer->slotPresent(frame);
        //通过信号调用  
        //m_pProcess->slotCaptureFrame(frame);
        return 0;
    }
private:
    CFrmPlayer* m_pPlayer;
    CFrameProcess* m_pProcess;
};

int CFrmPlayer::TestCamera()
{
    static Hander* pHander = NULL;
    if(pHander)
        return 0;
    std::vector<CCameraInfo::CamerInfo> lstInfo;
    int nRet = CCameraFactory::Instance()->EnumDevice(lstInfo);
    std::vector<CCameraInfo::CamerInfo>::iterator it;
    for (it = lstInfo.begin(); it != lstInfo.end(); it++)
    {
        LOG_MODEL_DEBUG("CFrmPlayer", "index:%d;name:%s",
                        it->nIndex,
                        it->szName.c_str()
                        );
        
    }
    CFrameProcess* pProcess = new CFrameProcess();
    bool check = connect(
                pProcess, SIGNAL(sigCaptureFrame(QVideoFrame)),
            /*pProcess, SLOT(slotFrameConvertedToRGB32(QVideoFrame)));
    Q_ASSERT(check);
    check = connect(pProcess,
                    SIGNAL(sigFrameConvertedToRGB32Frame(QVideoFrame)),*/
                    this, SLOT(slotPresent(QVideoFrame)));
    Q_ASSERT(check);
    pHander = new Hander(this, pProcess);
    VideoInfo vi;
    vi.Format = VIDEO_FORMAT_RGB24;
    vi.nHeight = 480;
    vi.nWidth = 640;
    vi.nRatio = 15;
    CCameraFactory::Instance()->GetCamera(0)->Open(pHander);
    CCameraFactory::Instance()->GetCamera(0)->Start();
    return nRet;
}

#endif
