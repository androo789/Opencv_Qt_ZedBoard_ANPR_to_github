#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H


//#include <errno.h>


#include <QString>
#include <QObject>
#include <QTextCodec>

struct buffer
{
    void * start;//视频缓冲区的起始地址
    size_t length;//缓冲区的长度
};


#define CLEAR(x) memset(&(x), 0, sizeof(x))

class VideoDevice : public QObject
{
    Q_OBJECT //有了这条语句才能使用QT中的signal和slot机制
public:
    VideoDevice(QString dev_name);//构造函数定义，用于初始化
    ~VideoDevice();//析构函数用于释放内存
    int get_frame(unsigned char ** yuv_buffer_pointer, size_t* len);//获取视频帧
    int unget_frame();//释放视频帧，让出缓存空间准备新的视频帧数据

    int stop_capturing();
    int uninit_device();//注销设备
    int close_device();//关闭设备

    int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
   int convert_yuv_to_rgb_pixel(int y, int u, int v);

private:
    int open_device();
    int init_device();
    int start_capturing();//启动视频采集
    int init_mmap();//内存映射初始化

    QString dev_name;
    int fd;//video0 file
    buffer* buffers;
    unsigned int n_buffers;
    int index;


signals:
    //void display_error(QString);

};

#endif // VIDEODEVICE_H
