#-------------------------------------------------
#
# Project created by QtCreator 2014-10-18T15:15:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QT_ANPR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    DetectRegions.cpp \
    OCR.cpp \
    Plate.cpp \
    convert_mat_qimage.cpp

HEADERS  += mainwindow.h \
    DetectRegions.h \
    OCR.h \
    Plate.h \
    convert_mat_qimage.h

FORMS    += mainwindow.ui

INCLUDEPATH += D:\opencv\include\

    LIBS += D:\opencv\lib\libopencv_calib3d248.dll.a\
            D:\opencv\lib\libopencv_contrib248.dll.a\
            D:\opencv\lib\libopencv_core248.dll.a\
            D:\opencv\lib\libopencv_features2d248.dll.a\
            D:\opencv\lib\libopencv_flann248.dll.a\
            D:\opencv\lib\libopencv_gpu248.dll.a\
            D:\opencv\lib\libopencv_highgui248.dll.a\
            D:\opencv\lib\libopencv_imgproc248.dll.a\
            D:\opencv\lib\libopencv_legacy248.dll.a\
            D:\opencv\lib\libopencv_ml248.dll.a\
            D:\opencv\lib\libopencv_objdetect248.dll.a\
            D:\opencv\lib\libopencv_video248.dll.a\
            D:\opencv\lib\libopencv_*

