#include "DetectFaces.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <QDebug>

CDetectFaces::CDetectFaces(QObject *parent) : QObject(parent)
{  
    cv::String szCascades = "D:\\source\\RabbitIm\\ThirdLibrary\\windows_msvc\\etc\\haarcascades\\";
    //cv::String szCascades = "D:\\source\\opencv\\data\\haarcascades_cuda\\";
    cv::String szFaceCascade = szCascades + "haarcascade_frontalface_alt2.xml";
    m_FaceCascade.load(szFaceCascade);
    //cv::String eyes_cascade_name = szCascades + "haarcascade_eye_tree_eyeglasses.xml";
    cv::String eyes_cascade_name = szCascades + "haarcascade_eye.xml";
    m_EyesCascade.load(eyes_cascade_name);

    m_Model = cv::face::EigenFaceRecognizer::create();
    
    m_nWidth = m_nHeight = 0;
}

bool CDetectFaces::DetectFaces(cv::Mat frame, cv::Mat &frame_gray)
{
    bool bFind = false;
    double t = 0;
    std::vector<cv::Rect> faces;
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(frame_gray, frame_gray);
    t = (double)cvGetTickCount();
    m_FaceCascade.detectMultiScale(frame_gray,
                                   faces,
                                   1.1,
                                   3,
                                   CV_HAAR_DO_ROUGH_SEARCH,
                                   cv::Size(1, 1),
                                   cv::Size(50,50));
    t = (double)cvGetTickCount() - t;
    qDebug() << "Detection face time = " << t / ((double)cvGetTickFrequency() * 1000) << "ms";
    for (size_t i = 0; i < faces.size(); i++)
    {
        cv::rectangle(frame, faces[i], cv::Scalar(0, 255, 0), 2, 8, 0);
        
        cv::Mat faceROI = frame_gray(faces[i]);
        std::vector<cv::Rect> eyes;
        
        t = (double)cvGetTickCount() - t;
        //-- In each face, detect eyes
        m_EyesCascade.detectMultiScale(faceROI, eyes, 1.1, 3,
                                       CV_HAAR_DO_ROUGH_SEARCH,
                                       cv::Size(1, 1), cv::Size(50, 50));
        t = (double)cvGetTickCount() - t;
        qDebug() << "Detection eye time = " << t / ((double)cvGetTickFrequency() * 1000) << "ms";        
        for (size_t j = 0; j < eyes.size(); j++)
        {
            cv::Rect rect(faces[i].x + eyes[j].x,
                          faces[i].y + eyes[j].y,
                          eyes[j].width,
                          eyes[j].height);
            cv::rectangle(frame, rect, cv::Scalar(0, 255, 255), 2, 8, 0);
            bFind = true;
        }
    }
    return bFind;
}

bool CDetectFaces::DetectFaces(cv::Mat frame,
                               cv::Mat &frame_gray,
                               CV_OUT std::vector<cv::Rect>& faces,
                               double scale)
{
    bool bFind = false;
    double t = 0;
    std::vector<cv::Rect> f;
    cv::Mat smallImg(cvRound(frame.rows / scale), cvRound(frame.cols / scale), CV_8UC1);
    cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
    cv::resize(frame_gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);

    cv::equalizeHist(smallImg, smallImg);
    t = (double)cvGetTickCount();
    m_FaceCascade.detectMultiScale(smallImg,
                                   f,
                                   1.1,
                                   3,
                                   CV_HAAR_DO_CANNY_PRUNING
                                   |CV_HAAR_FIND_BIGGEST_OBJECT
                                   |CV_HAAR_DO_ROUGH_SEARCH
                                   |CV_HAAR_SCALE_IMAGE);
    t = (double)cvGetTickCount() - t;
    //qDebug() << "Detection face time = " << t / ((double)cvGetTickFrequency() * 1000) << "ms";
    for (size_t i = 0; i < f.size(); i++)
    {
        cv::Rect r;
        r.x = cvRound(f[i].x * scale);
        r.y = cvRound(f[i].y * scale);
        r.width = cvRound(f[i].width * scale);
        r.height = cvRound(f[i].height * scale);
        faces.push_back(r);
        cv::rectangle(frame, r, cv::Scalar(0, 255, 0), 2, 8, 0);
        
        cv::Mat faceROI = smallImg(f[i]);
        std::vector<cv::Rect> eyes;
        
        //-- In each face, detect eyes
        //t = (double)cvGetTickCount();
        m_EyesCascade.detectMultiScale(faceROI,
                                       eyes,
                                       1.1,
                                       3,
                                       CV_HAAR_DO_CANNY_PRUNING
                                       |CV_HAAR_FIND_BIGGEST_OBJECT
                                       |CV_HAAR_DO_ROUGH_SEARCH
                                       |CV_HAAR_SCALE_IMAGE);
        //t = (double)cvGetTickCount() - t;
        //qDebug() << "Detection eye time = " << t / ((double)cvGetTickFrequency() * 1000) << "ms";
        for (size_t j = 0; j < eyes.size(); j++)
        {
            cv::Rect rect(cvRound((f[i].x + eyes[j].x) * scale),
                          cvRound((f[i].y + eyes[j].y) * scale),
                          cvRound(eyes[j].width * scale),
                          cvRound(eyes[j].height * scale));
            cv::rectangle(frame, rect, cv::Scalar(0, 255, 255), 2, 8, 0);
            bFind = true;
        }
    }
    
    return bFind;
}

int CDetectFaces::AddImage(cv::Mat image, int lable)
{
    if(image.empty())
        return -1;

    if(0 == m_nHeight || 0 == m_nWidth)
    {
        m_nHeight = image.rows;
        m_nWidth = image.cols;
    }

    cv::Mat img(m_nHeight, m_nWidth,  CV_8UC1);
    qDebug() << "AddImage: " << "width:" << img.cols << " height:" << img.rows;
    cv::resize(image, img, img.size(), 0, 0, cv::INTER_LINEAR);

    m_Images.push_back(img);
    m_Lables.push_back(lable);
    return 0;
}

int CDetectFaces::Train()
{
    int nRet = 0;
    double t = 0;
    t = (double)cvGetTickCount();
    m_Model->train(m_Images, m_Lables);
    t = (double)cvGetTickCount() - t;
    qDebug() << "Detection eye time = " << t / ((double)cvGetTickFrequency() * 1000) << "ms";    
    m_Model->save("MyFaceLBPHModel.xml");
    return nRet;
}

int CDetectFaces::Recognizer(cv::Mat image, CV_OUT int &label, CV_OUT double &confidence)
{
    if(image.empty())
        return -1;

    if(0 == m_nHeight || 0 == m_nWidth)
    {
        m_nHeight = image.rows;
        m_nWidth = image.cols;
    }

    cv::Mat img(m_nHeight, m_nWidth,  CV_8UC1);
    cv::resize(image, img, img.size(), 0, 0, cv::INTER_LINEAR);
    
    m_Model->predict(img, label, confidence);
    //label = m_Model->predict(img);
    qDebug() << "label:" << label << " confidence:" << confidence;
    return 0;
}
