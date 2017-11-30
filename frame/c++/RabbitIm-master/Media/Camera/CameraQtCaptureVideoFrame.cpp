#include "CameraQtCaptureVideoFrame.h"
#include <QThread>
#include <QTime>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QImage>
#include "CameraQt.h"

CCameraQtCaptureVideoFrame::CCameraQtCaptureVideoFrame(QObject *parent) :
    QAbstractVideoSurface(parent)
{
}

CCameraQtCaptureVideoFrame::~CCameraQtCaptureVideoFrame()
{
}

//选择需要捕获视频帧的格式  
QList<QVideoFrame::PixelFormat> 
CCameraQtCaptureVideoFrame::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    LOG_MODEL_DEBUG("CCameraQtCaptureVideoFrame",
         "CCameraQtCaptureVideoFrame::supportedPixelFormats handleType:%d",
         handleType);
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32  //windows 平台、linux 平台默认都支持 RGB32 格式  
                << QVideoFrame::Format_RGB24
                << QVideoFrame::Format_ARGB32; /*
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555; //*/
		//lst.push_back(QVideoFrame::Format_YUYV);//Qt现在不支持此格式，因为Qt内部用了QImage来处理视频帧。
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}
/*
bool CCameraQtCaptureVideoFrame::isFormatSupported(const QVideoSurfaceFormat &format) const
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();
    LOG_MODEL_DEBUG("CCameraQtCaptureVideoFrame", "format:%d;size:%s;handleType:%d", imageFormat, size, format.handleType());
    return imageFormat != QImage::Format_Invalid
            && !size.isEmpty()
            && format.handleType() == QAbstractVideoBuffer::NoHandle;
}
*/
bool CCameraQtCaptureVideoFrame::present(const QVideoFrame &frame)
{
#ifdef DEBUG_VIDEO_TIME
    static QTime preTime = QTime::currentTime();
    QTime curTime = QTime::currentTime();
    LOG_MODEL_DEBUG("Video", "CCameraQtCaptureVideoFrame::present:threadid:0x%X, preTime:%s, currTime:%s, space:%d",
           QThread::currentThreadId(),
           qPrintable(preTime.toString("hh:mm:ss.zzz")),
           qPrintable(curTime.toString("hh:mm:ss.zzz")),
           preTime.msecsTo(curTime));
    preTime = curTime;
#endif
    if(m_pCamera)
        m_pCamera->Present(frame);
    emit sigCaptureFrame(frame);
    return true;
}

//根据不同的平台，设置捕获方式  
bool CCameraQtCaptureVideoFrame::setSource(CCameraQt *pCamera)
{
    bool ret = true;

    m_pCamera = pCamera;

#ifdef ANDROID
    //android下,目前只能用probe捕获视频  
    ret = m_Probe.setSource(pCamera->m_Camera);
    if(ret)
    {
        ret = connect(&m_Probe, 
                SIGNAL(videoFrameProbed(const QVideoFrame&)),
                SLOT(present(const QVideoFrame&)));
        Q_ASSERT(ret);
    }
    else
        LOG_MODEL_ERROR("CaptureVideo", "m_Probe.setSource fail");
#else
    //windows下,只能用下面方式捕获视频  
    if(m_pCamera)
        m_pCamera->m_Camera->setViewfinder(this);
#endif
    return ret;
}
