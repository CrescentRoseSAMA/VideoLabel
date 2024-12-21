#include <QApplication>
#include "threadLabel.hpp"
int main(int argc, char *argv[])
{
    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat*");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    QApplication app(argc, argv);
    MyWidget w;
    std::string path = "../Armor.mp4";
    w.videoPlay(path, 20);
    w.show();

    return app.exec();
}