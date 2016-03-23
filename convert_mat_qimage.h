#ifndef CONVERT_MAT_QIMAGE_H
#define CONVERT_MAT_QIMAGE_H

#include<opencv2/opencv.hpp>
#include <QImage>
class convert_Mat_Qimage
{
public:
    convert_Mat_Qimage();

    QImage Mat2QImage(cv::Mat const& src);
    cv::Mat QImage2Mat(QImage const& src);


};

#endif // CONVERT_MAT_QIMAGE_H
