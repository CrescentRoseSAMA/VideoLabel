#ifndef THREADLABEL_HPP
#include <QLabel>
#include <QThread>
#include <QPushButton>
#include <QDialog>
#include <QFileDialog>
#include <opencv4/opencv2/opencv.hpp>

class modelPart : public QObject
{
    Q_OBJECT
private:
    std::string path;
    cv::VideoCapture cap;
    cv::Mat frame;

public:
    bool siGnal;
    bool videoEnd;
    enum State
    {
        Video = 1,
        Cam,
        Stop
    };
    modelPart(QObject *parent = nullptr) : QObject(parent)
    {
        siGnal = false;
        videoEnd = true;
    }
    bool setModel(std::string &path, State st)
    {
        bool ret = true;
        if (path.empty())
            ret = false;
        this->path = path;
        if (st == Video)
        {
            if (cap.isOpened())
                cap.release();
            cap.open(path);
            if (!cap.isOpened())
                ret = false;
            else
                videoEnd = false;
        }
        else if (st == Cam)
        {
            if (path.size() != 1)
                ret = false;
            int videoId = path[0] - '0';
            if (videoId < 0 || videoId > 9)
                ret = false;
            cap.open(videoId);
            if (!cap.isOpened())
                ret = false;
            else
                videoEnd = false;
        }
        else if (st == Stop)
        {
            if (cap.isOpened())
            {
                cap.release();
                videoEnd = true;
            }
        }
        return ret;
    }
public slots:
    void sendFrame(cv::Mat *dst)
    {
        if (cap.read(*(dst)))
        {
            siGnal = true;
            videoEnd = false;
        }
        else
        {
            siGnal = true;
            videoEnd = true;
        }
    }
};

class controlPart : public QThread
{
    Q_OBJECT
private:
    cv::Mat frame;
    float msPerFrame;

signals:
    void getFrame(cv::Mat *dst);
    void sendFrame(cv::Mat &src); // 链接外部

public:
    bool stop;
    modelPart model;
    controlPart(QObject *parent = nullptr) : QThread(parent)
    {
        connect(this, &controlPart::getFrame, &model, &modelPart::sendFrame);
        stop = false;
    }
    void setMsPerFrame(float msPerFrame) { this->msPerFrame = msPerFrame; }
    void setModel(std::string &path, modelPart::State st) { model.setModel(path, st); }

    void run() override
    {
        while (!model.videoEnd && !stop)
        {
            emit getFrame(&frame);
            while (!model.siGnal)
            {
            };
            model.siGnal = false;
            if (!model.videoEnd)
                emit sendFrame(frame);
            msleep(msPerFrame);
        }
        stop = false;
    }
};

class videoLabel : public QLabel
{
    Q_OBJECT
private:
    controlPart control;
    cv::Mat showImg;
    bool isVideoPlaying;
private slots:
    void updateFrame(cv::Mat &src)
    {
        if (src.empty())
        {
            std::cout << "empty frame" << std::endl;
            return;
        }
        showImg = src.clone();
        cv::cvtColor(showImg, showImg, cv::COLOR_BGR2RGB);
        QImage img((const unsigned char *)showImg.data, showImg.cols, showImg.rows, showImg.step, QImage::Format_RGB888);
        QPixmap pixmap = QPixmap::fromImage(img);
        setPixmap(pixmap);
    }
    void endThread()
    {
        control.quit();
    }

public:
    videoLabel(QWidget *parent = nullptr) : QLabel(parent)
    {
        isVideoPlaying = false;
        connect(&control, &controlPart::sendFrame, this, &videoLabel::updateFrame);
    }
    void videoPlay(std::string &path, float msPerFrame)
    {
        control.setMsPerFrame(msPerFrame);
        control.setModel(path, modelPart::Video);
        isVideoPlaying = true;
        control.start();
    }
    void stopVideo()
    {
        if (isVideoPlaying)
        {
            control.stop = true;
            isVideoPlaying = false;
            while (!control.isFinished() && !control.stop)
            {
            }
        }
    }
};

class MyWidget : public QWidget
{
    Q_OBJECT
private:
    videoLabel *video;
    QPushButton *btn;

public:
    MyWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        video = new videoLabel(this);
        video->resize(640, 480);
        btn = new QPushButton(this);
        btn->setText("Open Img");
        btn->move(50, 50);
        connect(btn, &QPushButton::clicked, this, &MyWidget::showPic);
    }
    void videoPlay(std::string &path, float msPerFrame)
    {
        video->videoPlay(path, msPerFrame);
    }
private slots:
    void showPic()
    {
        video->stopVideo();
        std::string ph = QFileDialog::getOpenFileName(this, "Open Img", "./", "Img (*.png *.jpg)").toStdString();
        if (!ph.empty())
        {
            auto img = cv::imread(ph);
            if (!img.empty())
            {
                cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
                QImage qImg((const unsigned char *)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
                QPixmap pixmap = QPixmap::fromImage(qImg);
                video->setPixmap(pixmap);
            }
        }
    }
};
#define THREADLABEL_HPP
#endif // THREADLABEL_HPP