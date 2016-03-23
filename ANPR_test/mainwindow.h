#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include<QTimer>// 设置采集数据的间隔时间


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/highgui.h>  //包含opencv库头文件
#include <opencv/cv.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);//算是类的声明 构造函数
    ~MainWindow();//析构函数

private slots:

    void readFarme();       // 读取当前帧信息


    void on_openButton_clicked();

    void on_runButton_clicked();

    void on_open_cam_clicked();

    void on_take_pic_clicked();

    void on_close_cam_clicked();

private:
    Ui::MainWindow *ui;
    cv::Mat input_image;
        QImage img;

        std::string name;

        QTimer    *timer;
        CvCapture *cam;// 视频获取结构， 用来作为视频获取函数的一个参数
        IplImage  *frame;//申请IplImage类型指针，就是申请内存空间来存放每一帧图像



};

#endif // MAINWINDOW_H
