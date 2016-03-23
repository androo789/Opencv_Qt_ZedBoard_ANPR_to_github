/*****************************************************************************
*   Number Plate Recognition using SVM and Neural Networks
******************************************************************************
*   by David Mill? Escriv? 5th Dec 2012
*   http://blog.damiles.com
******************************************************************************
*   Ch5 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

#include "DetectRegions.h"
#include<QDebug>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

void DetectRegions::setFilename(string s) {
        filename=s;
}

DetectRegions::DetectRegions(){//C++??????????????  ??????????
    showSteps=false;
    saveRegions=false;
}

bool DetectRegions::verifySizes(RotatedRect mr){//??????????  ????

    float error=0.4;
    //Spain car plate size: 52x11 aspect 4,7272 ???????????
    float aspect=4.7272;
    //Set a min and max area. All other patchs are discarded
    int min= 15*aspect*15; // minimum area??????   ????????????
    int max= 125*aspect*125; // maximum area
    //Get only patchs that match to a respect ratio.
    float rmin= aspect-aspect*error;
    float rmax= aspect+aspect*error;

    int area= mr.size.height * mr.size.width;//?????
    float r= (float)mr.size.width / (float)mr.size.height;//??????
    if(r<1)
        r= (float)mr.size.height / (float)mr.size.width;//???????1

    if(( area < min || area > max ) || ( r < rmin || r > rmax )){
        return false;
    }else{
        return true;
    }

}

Mat DetectRegions::histeq(Mat in)
{
    Mat out(in.size(), in.type());
    if(in.channels()==3){
        Mat hsv;
        vector<Mat> hsvSplit;
        cvtColor(in, hsv, CV_BGR2HSV);
        split(hsv, hsvSplit);
        equalizeHist(hsvSplit[2], hsvSplit[2]);
        merge(hsvSplit, hsv);
        cvtColor(hsv, out, CV_HSV2BGR);
    }else if(in.channels()==1){
        equalizeHist(in, out);
    }

    return out;

}

vector<Plate> DetectRegions::segment(Mat input){//??????
    vector<Plate> output;//????????  ??

    //convert image to gray? ??
    Mat img_gray;
    cvtColor(input, img_gray, CV_BGR2GRAY);
    blur(img_gray, img_gray, Size(5,5));

    //Finde vertical lines??. x???? ?? Car plates have high density of vertical lines
    Mat img_sobel;
    Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);
    if(showSteps)
        imshow("Sobel", img_sobel);

     imwrite("/mnt/DDD0.bmp",img_sobel);

    //threshold image ???
    Mat img_threshold;
    threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU+CV_THRESH_BINARY);//??sobel???????
    if(showSteps)
        imshow("Threshold", img_threshold);

     imwrite("/mnt/DDD1.bmp",img_threshold);

    //Morphplogic operation close
    //Returns a structuring element of the specified size and shape for morphological operations.
    //?????????????????? ????????  ??? ???
    Mat element = getStructuringElement(MORPH_RECT, Size(17, 3) );
    morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element);
    if(showSteps)
        imshow("Close", img_threshold);

      imwrite("/mnt/DDD2.bmp",img_threshold);

    //Find contours of possibles plates??????? vactor?????????
    vector< vector< Point> > contours;//?????  ?????????? ??
    findContours(img_threshold,
            contours, // a vector of contours
            CV_RETR_EXTERNAL, // retrieve the external contours
            CV_CHAIN_APPROX_NONE); // all pixels of each contours

    //Start to iterate to each contour founded
    vector<vector<Point> >::iterator itc= contours.begin();
    vector<RotatedRect> rects;

    //Remove patch that are no inside limits of aspect ratio and area.
    while (itc!=contours.end()) {
        //Create bounding rect of object
        RotatedRect mr= minAreaRect(Mat(*itc));//??????
        if( !verifySizes(mr)){
            itc= contours.erase(itc);
        }else{
            ++itc;
            rects.push_back(mr);
        }//?????????
    }

    // Draw blue???? contours on a white image ????  scalar? ? ? ? ??
    cv::Mat result;
    input.copyTo(result);
    cv::drawContours(result,contours,
            -1, // draw all contours
            cv::Scalar(255,0,0), // in blue
            1); // with a thickness of 1

   int  DQ_flag =0;

   for(int i=0; i< (int)rects.size(); i++){//?????????

        //For better rect cropping for each posible box
        //Make floodfill algorithm because the plate has white background
        //And then we can retrieve?? more clearly the contour box???????????
        circle(result, rects[i].center, 3, Scalar(0,255,0), -1);
        //get the min size between width and height
        float minSize=(rects[i].size.width < rects[i].size.height)?rects[i].size.width:rects[i].size.height;
        minSize=minSize-minSize*0.5;
        //initialize rand and get 5 points around center for floodfill algorithm???????5?? ???
        srand ( time(NULL) );
        //Initialize floodfill parameters and variables
        Mat mask;
        mask.create(input.rows + 2, input.cols + 2, CV_8UC1);
        mask= Scalar::all(0);
        int loDiff = 30;
        int upDiff = 30;
        int connectivity = 4;
        int newMaskVal = 255;
        int NumSeeds = 10;
        Rect ccomp;
        int flags = connectivity + (newMaskVal << 8 ) + CV_FLOODFILL_FIXED_RANGE + CV_FLOODFILL_MASK_ONLY;
        for(int j=0; j<NumSeeds; j++){
            Point seed;
            seed.x=rects[i].center.x+rand()%(int)minSize-(minSize/2);
            seed.y=rects[i].center.y+rand()%(int)minSize-(minSize/2);
            circle(result, seed, 1, Scalar(0,255,255), -1);//?????
            int area = floodFill(input, mask, seed, Scalar(255,0,0), &ccomp, Scalar(loDiff, loDiff, loDiff), Scalar(upDiff, upDiff, upDiff), flags);
        }												//?????
        if(showSteps)
            imshow("MASK", mask);
        //cvWaitKey(0);
        if(DQ_flag == 0){
        imwrite("/mnt/DDD3.bmp",mask);
        DQ_flag=1;
        }
         imwrite("/mnt/DDD4.bmp",mask);


        //Check new floodfill mask match for a correct patch.
        //Get all points detected for get Minimal rotated Rect  255????
        vector<Point> pointsInterest;
        Mat_<uchar>::iterator itMask= mask.begin<uchar>();
        Mat_<uchar>::iterator end= mask.end<uchar>();
        for( ; itMask!=end; ++itMask)
            if(*itMask==255)
                pointsInterest.push_back(itMask.pos());

        RotatedRect minRect = minAreaRect(pointsInterest);

        if(verifySizes(minRect)){
            // rotated rectangle drawing
            Point2f rect_points[4]; minRect.points( rect_points );
            for( int j = 0; j < 4; j++ )
                line( result, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255), 1, 8 );
                    //????????? ?????? ???												//????红色
             imwrite("/mnt/DDD5.bmp",result);

            //Get rotation matrix
            float r= (float)minRect.size.width / (float)minRect.size.height;
            float angle=minRect.angle;
            qDebug()<<"minRect 's  feature "
                   <<minRect.size.width<<minRect.size.height<<minRect.angle
                     <<minRect.center.x<<minRect.center.y;

            if(r<1)
                angle=90+angle;
            Mat rotmat= getRotationMatrix2D(minRect.center, angle,1);

              imwrite("/mnt/DDD6.bmp",rotmat);

                imwrite("/mnt/DDD7.bmp",input);//

            //Create and rotate image
            Mat img_rotated;

            img_rotated=input;
//仿射变换
            //最后在zed上不用这次仿射
            //在pc上加上这次仿射
            //warpAffine(input, img_rotated, rotmat, input.size());


               imwrite("/mnt/DDD8.bmp",img_rotated);//这里图片就不一样了


            //Crop image
            Size rect_size=minRect.size;
            if(r < 1)
                swap(rect_size.width, rect_size.height);
            Mat img_crop;
            getRectSubPix(img_rotated, rect_size, minRect.center, img_crop);

            Mat resultResized;
            resultResized.create(33,144, CV_8UC3);
            resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);

              imwrite("/mnt/DDD9.bmp",resultResized);//这上面出了问题！！


            //Equalize croped image
            Mat grayResult;
            cvtColor(resultResized, grayResult, CV_BGR2GRAY);
            blur(grayResult, grayResult, Size(3,3));
            grayResult=histeq(grayResult);

            imwrite("/mnt/DDD10.bmp",grayResult);

            //if(saveRegions){ //????????
            //    stringstream ss(stringstream::in | stringstream::out);
            //    ss << "tmp/" << filename << "_" << i << ".jpg";
            //    imwrite(ss.str(), grayResult);
            //}
            output.push_back(Plate(grayResult,minRect.boundingRect()));//????? ????????? ????
        }
    }    //?????????
    if(showSteps)
        imshow("Contours", result);

       imwrite("/mnt/DDD11.bmp",result);
       //here is ok  on zed!

    return output;//???????
}

vector<Plate> DetectRegions::run(Mat input){//run?????? ???

    //Segment image by white
    vector<Plate> tmp=segment(input);

    //return detected and posibles regions
    qDebug()<<"tmp's number is"<<tmp.size();


 qDebug()<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";
    return tmp;
}

