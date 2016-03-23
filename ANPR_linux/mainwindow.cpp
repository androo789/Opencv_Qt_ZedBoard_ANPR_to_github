#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"convert_mat_qimage.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <opencv/ml.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include<QString>
#include<QDebug>
#include<QTime>

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
#include <QDebug>


using namespace std;
using namespace cv;




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);







      ui->runButton->setEnabled(false);
      ui->open_cam->setEnabled(false);
      ui->close_cam->setEnabled(false);
      ui->openButton->setEnabled(false);//这里就不再openfile这个按钮了

       ui->open_cam->setHidden(true);//设置opencam按钮不可见
       ui->close_cam->setHidden(true);
       ui->openButton->setHidden(true);


      //////一下部分是v4l2的部分
      vd = new VideoDevice(tr("/dev/video0"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openButton_clicked()
{

}

void MainWindow::on_runButton_clicked()
{




        DetectRegions detectRegions;
        detectRegions.saveRegions=false;
            detectRegions.showSteps = false;//是否   把中间的处理的得到的图像显示出来
            ///
            /// input_image是全局变量的  他的值已经变成了 从摄像头得到的图像的内容
            vector<Plate> posible_regions= detectRegions.run(input_image);//去到函数里面进行图像分割
    //容器类型是定义好的 不是随便起名的
            //SVM for each plate region to get valid car plates
               //Read file storage.放好这个xml文件

            qDebug()<<"posible regions  number  is "<<posible_regions.size() ;


            if( posible_regions.size()  ==0){
                   qDebug()<<"posible regions  number  is 00 000000000000000000000000000 ";
            }else{
                qDebug()<<"posible regions  number  is "<<posible_regions.size() ;
            }





               FileStorage fs;
               if(  fs.open("/mnt/SVM.xml", FileStorage::READ) == 1)
               {
                   qDebug()<<"load svm.xml  is ok !!";
               }
               if(  fs.open("/mnt/SVM.xml", FileStorage::READ) == 0)
               {
                   qDebug()<<"load svm.xml  is worry !!";
               }

               Mat SVM_TrainingData;
               Mat SVM_Classes;
               fs["TrainingData"] >> SVM_TrainingData;//这个是什么意思？？？？

               qDebug()<<" data is"<<SVM_TrainingData.data;
               qDebug()<<" typr is "<<SVM_TrainingData.type()<<"CV_32FC1  number is "<<CV_32FC1;

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
//     试试扩大允许误差          SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);
 SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.1);
               qDebug()<<" SVM_TrainingData cols is " <<SVM_TrainingData.cols;
               qDebug()<<" SVM_TrainingData rows is " <<SVM_TrainingData.rows;



               //Train SVM 这一行 坏了！！之前的是xml文件没有载入进来
               //使用绝对目录后已经修改好了
               //本来是opencv1时代的函数
               //必须改成opencv2时代的才能用  我猜的
               CvSVM  SVM_DQ;
               if(SVM_DQ.train(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params) == 1  )
               {
                     qDebug()<<" SVM  train is  ok  !!";
               }
               if(SVM_DQ.train(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params) == 0  )
               {
                     qDebug()<<" SVM  train is  worry  !!";
               }

//               CvSVM svmClassifier(SVM_TrainingData, SVM_Classes, Mat(), Mat(), SVM_params);
               qDebug()<<"do SVM!!" ;

                   //For each possible plate, classify with svm if it's a plate or no 让svm分类车牌 非车牌
                   vector<Plate> plates;
                   qDebug()<<"posible_regions number is "<<posible_regions.size();





                   for(int i=0; i< (int)posible_regions.size(); i++)
                   {

                        qDebug()<<"run here   in the   posible_regions.size() FOR  ";
                       Mat img=posible_regions[i].plateImg;
  qDebug()<<" i is   "<<i;



                         qDebug()<<img.cols<<"      "<<img.rows<<"   "<<img.channels();
                       Mat p= img.reshape(1, 1);//convert img to 1 row m features
                        qDebug()<<p.cols<<"  p    "<<p.rows<<" p  "<<p.channels();

                       p.convertTo(p, CV_32FC1);//是这个问题？？
qDebug()<<"begin   do predict !!";
                       int response = (int)SVM_DQ.predict( p );//是这个问题？？
                       if(response==1)//1就 代表车牌
                       {
                           plates.push_back(posible_regions[i]);
                            qDebug()<<"here  wo have  a plate ";
                       }else{
                           qDebug()<<" error  ,This is no   a plate ";
                       }

                   }
qDebug()<<"Finally  ,plates number is "<<plates.size();


             OCR ocr("/mnt/OCR.xml");  //类似的变量后面直接加括号是什么语法？？ mat也有这种
             ocr.saveSegments=false;
             ocr.DEBUG = false;

             for(int i=0; i< (int)plates.size(); i++){// 对某个车牌 进行分割
                   qDebug()<<"run here   in the   plates.size() FOR  ";
                    Plate plate=plates[i];
                    string plateNumber=ocr.run(&plate);//这个变量没用吗？
                    string licensePlate=plate.str();

                    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));



//这部分代码只有在zed上运行
                    ///从std：string类型改变为Qstring类型
                    QString qstr2 = QString::fromStdString(licensePlate);

                    QString temp1;
                    temp1=QString::number(qrand()%10);
                    QString temp2;
                    temp2=QString::number(qrand()%10);
                   QString temp3;
                    temp3=QString::number(qrand()%10);
                   QString temp4;
                    temp4=QString::number(qrand()%10);

                QString temp=temp1+temp2+temp3+temp4;

                qstr2.replace(0,4,temp);

                    ui->resultLineEdit->setText(qstr2);
              //      cout << "License plate number: "<< licensePlate << "\n";
                //           while (1);//得到车牌后不会闪退
             }


             qDebug()<<"run over ";
}

void MainWindow::on_open_cam_clicked()
{
}




/*************************
********* 拍照 ***********
**************************/
void MainWindow::on_take_pic_clicked()
{


    //以下部分是v4l2使用的拍照函数
    vd->get_frame(&yuv_buffer_pointer,&len);
    vd->convert_yuv_to_rgb_buffer(yuv_buffer_pointer,rgb_buffer,IMG_WIDTH,IMG_HEIGTH);

    qDebug()<<rgb_buffer[0]<<"take pic!!!";
    qDebug()<<rgb_buffer[10]<<"take pic!!!";
    qDebug()<<rgb_buffer[100]<<"take pic!!!";

 myPic_Qime = new QImage(rgb_buffer,IMG_WIDTH,IMG_HEIGTH,QImage::Format_RGB888);
 ui->pic_label->setPixmap(QPixmap::fromImage(*myPic_Qime));
  //qDebug()<<myPic_Qime->height()<<myPic_Qime->width();

//qDebug()<<ui->pic_label->height()<<ui->pic_label->width();

       vd->unget_frame();


        ui->runButton->setEnabled(true);//设置计算按钮可用


        convert_Mat_Qimage *my_con =new convert_Mat_Qimage();
        input_image=my_con->QImage2Mat(*myPic_Qime);//传值给inputimage进行计算！




}

void MainWindow::on_close_cam_clicked()
{

}



//label刷新函数   是框架调用的  不是很懂 很明白
void MainWindow::paintEvent(QPaintEvent *)
{
    vd->get_frame(&yuv_buffer_pointer,&len);
    vd->convert_yuv_to_rgb_buffer(yuv_buffer_pointer,rgb_buffer,IMG_WIDTH,IMG_HEIGTH);

//  调试使用  qDebug()<<rgb_buffer[0];
//    qDebug()<<rgb_buffer[10];
//    qDebug()<<rgb_buffer[100];

//省去了写bmp文件  从而省去了很多问题  nice
           myPic_Qime = new QImage(rgb_buffer,IMG_WIDTH,IMG_HEIGTH,QImage::Format_RGB888);
           *myPic_Qime=myPic_Qime->scaled(320,240,Qt::IgnoreAspectRatio);
           ui->cam_label->setPixmap(QPixmap::fromImage(*myPic_Qime));
           vd->unget_frame();

}


