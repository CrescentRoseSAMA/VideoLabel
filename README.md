# 使用Qt多线程以及opencv实现的离线播放Label控件

## 简介
刚开始学Qt，使用Label的setPixmap()加上opencv实现视频播放，但是发现如果直接在主线程中连续
播放视频，会导致主线程的Widget或者MainWindow无法接受外部的事件，导致在播放视频的时候无法进行其他操作。

## 解决方案
尝试使用多线程嵌入了一个操作，但是写的太耦合了，又难看读取一些量也不方便。因此便想着抽时间写一个
即插即用，可以直接使用的离线播放Label控件。

## 实现

主要有三个类：

- `class modelPart`， 负责读取视频帧并等待信号，将读取到的帧发送给播放线程。
- `class controlPart` 仅有这部分在子线程中，负责每隔一段时间发送信号给播放线程，从而从modelPart中读取视频帧。
- `class videoLabel` 继承自QLabel，其包含一个controlPart对象，负责获取视频帧并显示

另外头文件中还有一个测试主界面的类，用于测试播放效果。
```c++
class MyWidget : public QWidget
```

## 主要api

```c++
void videoPlay(std::string &path, float msPerFrame);
// @param path 视频路径
// @param msPerFrame 每一帧的播放时间间隔，单位为毫秒

void stopVideo();
// 停止播放,会发送停止信号给播放线程
```

## 使用示例

```c++
    void videoPlay(std::string &path, float msPerFrame)
    {
        video->videoPlay(path, msPerFrame);
    }
// 调用videoPlay函数，传入视频路径和每一帧的播放时间间隔

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
// 调用showPic函数，弹出文件选择框，选择图片，将图片显示在videoLabel控件上,同时检测是否有视频正在播放，如果有，则停止播放。
```