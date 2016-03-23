#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include<QTimer>// 设置采集数据的间隔时间


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/highgui.h>  //包含opencv库头文件
#include <opencv/cv.h>

#include"common.h"
#include"videodevice.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void paintEvent(QPaintEvent *);//窗口刷新函数

    void on_openButton_clicked();
    void on_runButton_clicked();
    void on_open_cam_clicked();
    void on_take_pic_clicked();
    void on_close_cam_clicked();

private:
    Ui::MainWindow *ui;
    cv::Mat input_image;



////一下部分是v4l2的部分
           VideoDevice *vd;
           unsigned char rgb_buffer[640*480*3];
           unsigned int len;
           unsigned char * yuv_buffer_pointer;

          QImage *myPic_Qime;



};

#endif // MAINWINDOW_H
