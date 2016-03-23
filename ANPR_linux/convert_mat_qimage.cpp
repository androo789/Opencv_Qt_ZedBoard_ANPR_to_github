#include "convert_mat_qimage.h"

convert_Mat_Qimage::convert_Mat_Qimage()
{
}


QImage convert_Mat_Qimage::Mat2QImage(cv::Mat const& src)
{
     cv::Mat temp; // make the same cv::Mat
     cvtColor(src, temp,CV_BGR2RGB); // cvtColor Makes a copt, that what i need
     QImage dest((const uchar *) temp.data,
                 temp.cols, temp.rows, temp.step,
                 QImage::Format_RGB888);
     dest.bits(); ///!! enforce deep copy, see documentation
     // of QImage::QImage ( const uchar * data, int width, int height, Format format )
     return dest;
}


cv::Mat convert_Mat_Qimage::QImage2Mat(QImage const& src)
{
     cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
     cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
     cvtColor(tmp, result,CV_BGR2RGB);
     return result;
}
