#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include<QTimer>// ���òɼ����ݵļ��ʱ��


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/highgui.h>  //����opencv��ͷ�ļ�
#include <opencv/cv.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);//����������� ���캯��
    ~MainWindow();//��������

private slots:

    void readFarme();       // ��ȡ��ǰ֡��Ϣ


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
        CvCapture *cam;// ��Ƶ��ȡ�ṹ�� ������Ϊ��Ƶ��ȡ������һ������
        IplImage  *frame;//����IplImage����ָ�룬���������ڴ�ռ������ÿһ֡ͼ��



};

#endif // MAINWINDOW_H
