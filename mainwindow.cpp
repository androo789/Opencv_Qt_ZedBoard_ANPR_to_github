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

//����Ҫ�Ǳ���ĺ���Ҳ������ ��ֲ��������
//�Ǿ�����vs�ϳ��׸㶮��������ֲ

#include<QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QTextCodec>
#include<QLineEdit>
//#include <QDebug>

using namespace std;
using namespace cv;



MainWindow::MainWindow(QWidget *parent) ://������Ķ���
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cam     = NULL;
    timer   = new QTimer(this);

    ui->runButton->setEnabled(false);


   /*�źźͲ�*/
   connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));
  // ʱ�䵽����ȡ��ǰ����ͷ��Ϣ

}

MainWindow::~MainWindow()//��������
{
    delete ui;
}

void MainWindow::on_openButton_clicked()
{
    //��һ��ͼƬ
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
               cv::imshow("b",input_image);//�ı���ɫ����ȷʵ�б仯
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
{//�����е��⣡
    DetectRegions detectRegions;
    detectRegions.saveRegions=false;
        detectRegions.showSteps = false;//�Ƿ�   ���м�Ĵ���ĵõ���ͼ����ʾ����
        ///
        /// input_image��ȫ�ֱ�����  ����ֵ�Ѿ������ ������ͷ�õ���ͼ�������
        vector<Plate> posible_regions= detectRegions.run(input_image);//ȥ�������������ͼ��ָ�
//���������Ƕ���õ� �������������
        //SVM for each plate region to get valid car plates
           //Read file storage.�ź����xml�ļ�

        //qDebug()<<"Hello,Qt wo cao!";


           FileStorage fs;
           fs.open("SVM.xml", FileStorage::READ);
           Mat SVM_TrainingData;
           Mat SVM_Classes;
           fs["TrainingData"] >> SVM_TrainingData;//�����ʲô��˼��������
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

               //For each possible plate, classify with svm if it's a plate or no ��svm���೵�� �ǳ���
               vector<Plate> plates;
               for(int i=0; i< posible_regions.size(); i++)
               {
                   Mat img=posible_regions[i].plateImg;
                   Mat p= img.reshape(1, 1);
                   p.convertTo(p, CV_32FC1);

                   int response = (int)svmClassifier.predict( p );
                   if(response==1)//1�� ������
                       plates.push_back(posible_regions[i]);
               }


         OCR ocr("OCR.xml");  //���Ƶı�������ֱ�Ӽ�������ʲô�﷨���� matҲ������
         ocr.saveSegments=false;
         ocr.DEBUG = false;
         for(int i=0; i< plates.size(); i++){// ��ĳ������ ���зָ�
                Plate plate=plates[i];
                string plateNumber=ocr.run(&plate);//�������û����
                string licensePlate=plate.str();
                QString qstr2 = QString::fromStdString(licensePlate);
                ui->resultLineEdit->setText(qstr2);
          //      cout << "License plate number: "<< licensePlate << "\n";
            //           while (1);//�õ����ƺ󲻻�����
         }

}

void MainWindow::on_open_cam_clicked()
{
    cam = cvCreateCameraCapture(-1);//������ͷ��������ͷ�л�ȡ��Ƶ

       timer->start(33);              // ��ʼ��ʱ����ʱ�򷢳�timeout()�ź�
}


/*********************************
********* ��ȡ����ͷ��Ϣ ***********
**********************************/
void MainWindow::readFarme()
{
    frame = cvQueryFrame(cam);// ������ͷ��ץȡ������ÿһ֡
    // ��ץȡ����֡��ת��ΪQImage��ʽ��QImage::Format_RGB888��ͬ������ͷ�ò�ͬ�ĸ�ʽ��
    QImage image =QImage((const uchar*)frame->imageData, frame->width,
                 frame->height, QImage::Format_RGB888).rgbSwapped();

    ////QImage image((const uchar*)frame->imageData,
    /// frame->width, frame->height, QImage::Format_RGB888);
    /// ��Ϊ�� QImage image = QImage((const uchar*)frame->imageData,
    ///  frame->width, frame->height, QImage::Format_RGB888).rgbSwapped();
    /// ������о޴���Ż�������������

    /***һ�²�����ͼƬ������С ��Ӧlabel��С������***/
     QImage* imgScaled = new QImage();
     *imgScaled=image.scaled(100,
                         100,
                       Qt::IgnoreAspectRatio);

    ui->cam_label->setPixmap(QPixmap::fromImage(*imgScaled));  // ��ͼƬ��ʾ��cam_label��
}

/*************************
********* ���� ***********
**************************/
void MainWindow::on_take_pic_clicked()
{
    frame = cvQueryFrame(cam);// ������ͷ��ץȡ������ÿһ֡

    // ��ץȡ����֡��ת��ΪQImage��ʽ��QImage::Format_RGB888��ͬ������ͷ�ò�ͬ�ĸ�ʽ��
    QImage image =QImage((const uchar*)frame->imageData, frame->width,
                 frame->height, QImage::Format_RGB888).rgbSwapped();

    ui->pic_label->setPixmap(QPixmap::fromImage(image));  // ��ͼƬ��ʾ��pic_label��
    ui->runButton->setEnabled(true);


//��������Ҫת��input_image=image;//��ֵ��Ҫ���������

//take_pic_mat=cv::Mat(image.height(), image.width(),
  //                   CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());
 //cv::cvtColor(take_pic_mat, take_pic_mat, CV_RGB2BGR);
    ///Ҫ���п���ת������
   //���Ƚ���//QImage->IplImage *
    //�ٽ���Mat mtx(img); // convert IplImage* -> Mat
//    int nChannel=0;
//        if(image.format()==QImage::Format_RGB888)nChannel=3;
//        if(image.format()==QImage::Format_ARGB32)nChannel=4;
//        //if( nChannel==0 )return false;

//        IplImage *iplImg=cvCreateImageHeader( cvSize(image.width(),
//                                                     image.height()),8,nChannel );
//        iplImg->imageData=(char*)image.bits();

//        if(nChannel==3)
//            cvConvertImage(iplImg,iplImg,CV_CVTIMG_SWAP_RB);

//        cv::Mat take_pic_mat(iplImg);//���õ�Ҫ�����ͼ��take_pic_mat
      //  cv::Mat input_image = new cv::Mat(iplImg);
//input_image=cv::c   take_pic_mat;//���ֲ����� ��ֵ�� ȫ�ֱ���  ��Ϊ����Ҫ����
//cv::cvtColor(input_image,input_image,CV_BGR2RGB);
    convert_Mat_Qimage *my_con =new convert_Mat_Qimage();

    input_image=my_con->QImage2Mat(image);
 //�������cv::imshow("a",input_image);//������ʾ������ͷ�õ���  Ҫ�����  ͼƬ


}

void MainWindow::on_close_cam_clicked()
{
    timer->stop();         // ֹͣ��ȡ���ݡ�

    cvReleaseCapture(&cam);//�ͷ��ڴ棻
}
