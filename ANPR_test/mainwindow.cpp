#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"convert_mat_qimage.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <opencv/ml.h>

#include <iostream>
#include <vector>

#include "DetectRegions.h"
#include "OCR.h"

//我主要是本身的函数也看不懂 移植就困难了
//那就现在vs上彻底搞懂函数再移植

#include<QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTextCodec>
#include<QLineEdit>
//#include <QDebug>

using namespace std;
using namespace cv;



MainWindow::MainWindow(QWidget *parent) ://算是类的定义
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam     = NULL;
    timer   = new QTimer(this);

    ui->runButton->setEnabled(false);


   /*信号和槽*/
   connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));
  // 时间到，读取当前摄像头信息

}

MainWindow::~MainWindow()//析构函数
{
    delete ui;
}

void MainWindow::on_openButton_clicked()
{
    //打开一副图片
           QString filename = QFileDialog::getOpenFileName(this,tr("Open Image"),"",tr("Image File(*.bmp *.jpg *.jpeg *.png)"));
           QTextCodec *code = QTextCodec::codecForName("gb18030");
            name = code->fromUnicode(filename).data();
           input_image = cv::imread(name);
           if(!input_image.data)
           {
               QMessageBox msgBox;
               msgBox.setText(tr("image data is null"));
               msgBox.exec();
           }
           else
           {
               cv::cvtColor(input_image,input_image,CV_BGR2RGB);
               cv::imshow("b",input_image);//改变颜色编码确实有变化
               img = QImage((const unsigned char*)(input_image.data),
                            input_image.cols,input_image.rows,
                            //image.cols*image.channels(),
                            QImage::Format_RGB888);
               ui->pic_label->clear();
               ui->pic_label->setPixmap(QPixmap::fromImage(img));

               ui->pic_label->resize(ui->pic_label->pixmap()->size());

               ui->runButton->setEnabled(true);

           }//put the pic on  lable
}

void MainWindow::on_runButton_clicked()
{//能运行到这！
    DetectRegions detectRegions;
    detectRegions.saveRegions=false;
        detectRegions.showSteps = false;//是否   把中间的处理的得到的图像显示出来
        ///
        /// input_image是全局变量的  他的值已经变成了 从摄像头得到的图像的内容
        vector<Plate> posible_regions= detectRegions.run(input_image);//去到函数里面进行图像分割
//容器类型是定义好的 不是随便起名的
        //SVM for each plate region to get valid car plates
           //Read file storage.放好这个xml文件

        //qDebug()<<"Hello,Qt wo cao!";


           FileStorage fs;
           fs.open("SVM.xml", FileStorage::READ);
           Mat SVM_TrainingData;
           Mat SVM_Classes;
           fs["TrainingData"] >> SVM_TrainingData;//这个是什么意思？？？？
           fs["classes"] >> SVM_Classes;
           //Set SVM params
           CvSVMParams SVM_params;
           SVM_params.svm_type = CvSVM::C_SVC;
           SVM_params.kernel_type = CvSVM::LINEAR; //CvSVM::LINEAR;
           SVM_params.degree = 0;
           SVM_params.gamma = 1;
           SVM_params.coef0 = 0;
           SVM_params.C = 1;
           SVM_params.nu = 0;
           SVM_params.p = 0;
           SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);
           //Train SVM
           CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);

               //For each possible plate, classify with svm if it's a plate or no 让svm分类车牌 非车牌
               vector<Plate> plates;
               for(int i=0; i< posible_regions.size(); i++)
               {
                   Mat img=posible_regions[i].plateImg;
                   Mat p= img.reshape(1, 1);
                   p.convertTo(p, CV_32FC1);

                   int response = (int)svmClassifier.predict( p );
                   if(response==1)//1就 代表车牌
                       plates.push_back(posible_regions[i]);
               }


         OCR ocr("OCR.xml");  //类似的变量后面直接加括号是什么语法？？ mat也有这种
         ocr.saveSegments=false;
         ocr.DEBUG = false;
         for(int i=0; i< plates.size(); i++){// 对某个车牌 进行分割
                Plate plate=plates[i];
                string plateNumber=ocr.run(&plate);//这个变量没用吗？
                string licensePlate=plate.str();
                QString qstr2 = QString::fromStdString(licensePlate);
                ui->resultLineEdit->setText(qstr2);
          //      cout << "License plate number: "<< licensePlate << "\n";
            //           while (1);//得到车牌后不会闪退
         }

}

void MainWindow::on_open_cam_clicked()
{
    cam = cvCreateCameraCapture(-1);//打开摄像头，从摄像头中获取视频

       timer->start(33);              // 开始计时，超时则发出timeout()信号
}


/*********************************
********* 读取摄像头信息 ***********
**********************************/
void MainWindow::readFarme()
{
    frame = cvQueryFrame(cam);// 从摄像头中抓取并返回每一帧
    // 将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
    QImage image =QImage((const uchar*)frame->imageData, frame->width,
                 frame->height, QImage::Format_RGB888).rgbSwapped();

    ////QImage image((const uchar*)frame->imageData,
    /// frame->width, frame->height, QImage::Format_RGB888);
    /// 改为了 QImage image = QImage((const uchar*)frame->imageData,
    ///  frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
    /// 真的是有巨大的优化！！！！！！

    /***一下部分是图片调整大小 适应label大小！！！***/
     QImage* imgScaled = new QImage();
     *imgScaled=image.scaled(100,
                         100,
                       Qt::IgnoreAspectRatio);

    ui->cam_label->setPixmap(QPixmap::fromImage(*imgScaled));  // 将图片显示到cam_label上
}

/*************************
********* 拍照 ***********
**************************/
void MainWindow::on_take_pic_clicked()
{
    frame = cvQueryFrame(cam);// 从摄像头中抓取并返回每一帧

    // 将抓取到的帧，转换为QImage格式。QImage::Format_RGB888不同的摄像头用不同的格式。
    QImage image =QImage((const uchar*)frame->imageData, frame->width,
                 frame->height, QImage::Format_RGB888).rgbSwapped();

    ui->pic_label->setPixmap(QPixmap::fromImage(image));  // 将图片显示到pic_label上
    ui->runButton->setEnabled(true);


//数据类型要转化input_image=image;//传值给要处理掉对象

//take_pic_mat=cv::Mat(image.height(), image.width(),
  //                   CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());
 //cv::cvtColor(take_pic_mat, take_pic_mat, CV_RGB2BGR);
    ///要进行可是转化！！
   //首先进行//QImage->IplImage *
    //再进行Mat mtx(img); // convert IplImage* -> Mat
//    int nChannel=0;
//        if(image.format()==QImage::Format_RGB888)nChannel=3;
//        if(image.format()==QImage::Format_ARGB32)nChannel=4;
//        //if( nChannel==0 )return false;

//        IplImage *iplImg=cvCreateImageHeader( cvSize(image.width(),
//                                                     image.height()),8,nChannel );
//        iplImg->imageData=(char*)image.bits();

//        if(nChannel==3)
//            cvConvertImage(iplImg,iplImg,CV_CVTIMG_SWAP_RB);

//        cv::Mat take_pic_mat(iplImg);//最后得到要处理的图像take_pic_mat
      //  cv::Mat input_image = new cv::Mat(iplImg);
//input_image=cv::c   take_pic_mat;//将局部变量 赋值给 全局变量  因为上面要用它
//cv::cvtColor(input_image,input_image,CV_BGR2RGB);
    convert_Mat_Qimage *my_con =new convert_Mat_Qimage();

    input_image=my_con->QImage2Mat(image);
 //调试语句cv::imshow("a",input_image);//弹窗显示从摄像头得到的  要处理的  图片


}

void MainWindow::on_close_cam_clicked()
{
    timer->stop();         // 停止读取数据。

    cvReleaseCapture(&cam);//释放内存；
}
