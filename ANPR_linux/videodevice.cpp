#include "videodevice.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <unistd.h>


#define FILE_VIDEO 	"/dev/video0"


VideoDevice::VideoDevice(QString dev_name)//VideoDevice的构造函数进行初始化
{
    this->dev_name = dev_name;
    this->fd = -1;
    this->buffers = NULL;
    this->n_buffers = 0;
    this->index = -1;

    if(open_device() == false)
    {
        close_device();
    }

    if(init_device() == false)
    {
        close_device();
    }

    if(start_capturing() == false)
    {
        stop_capturing();
        close_device();
    }
}

VideoDevice::~VideoDevice()//VideoDevice的析构函数
{
    if(stop_capturing() == false)
    {
    }
    if(uninit_device() == false)
    {
    }
    if(close_device() == false)
    {
    }
}
int VideoDevice::open_device()
{
    fd = open(FILE_VIDEO,O_RDWR);
    if(fd == -1)
    {
        printf("Error opening V4L interface\n");
        return false;
    }
    return true;
}

int VideoDevice::close_device()
{
    if( close(fd) == false)
    {
        printf("Error closing V4L interface\n");
        return false;
    }
    return true;
}

int VideoDevice::init_device()//设备初始化
{
    v4l2_capability cap;//设备能力结构体
    v4l2_format fmt;//设置视频像素
    v4l2_streamparm setfps;//设置采样率

    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
    {
        printf("Error opening device %s: unable to query device.\n",FILE_VIDEO);
        return false;
    }
    else
    {
        printf("driver:\t\t%s\n",cap.driver);//驱动名
        printf("card:\t\t%s\n",cap.card);//摄像头信息
        printf("bus_info:\t%s\n",cap.bus_info);//PCI总线信息
        printf("version:\t%d\n",cap.version);//内核版本
        printf("capabili ties:\t%x\n",cap.capabilities);
    //以上打印信息详见设备能力结构体(struct v4l2_capability)
        if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE)
        {
            printf("Device %s: supports capture.\n",FILE_VIDEO);
        }

        if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING)
        {
            printf("Device %s: supports streaming.\n",FILE_VIDEO);
        }
    }

    //set fmt
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//恒为此项
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//视频数据存储类型
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;//隔行扫描

    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
            printf("Unable to set format\n");
            return false;
    }
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
    {
            printf("Unable to get format\n");
            return false;
    }

    //set fps 具体参考结构体v4l2_captureparm
    //set fps
    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*timeperframe字段用于指定想要使用的帧频率，它是一个结构体：
        numerator 和denominator所描述的系数给出的是成功的帧之间的时间间隔。
        numerator分子，denominator分母。主要表达每次帧之间时间间隔。numerator/denominator秒一帧。*/

    setfps.parm.capture.timeperframe.numerator = 1;
    setfps.parm.capture.timeperframe.denominator = 10;///这里就是假设 我的设置的是10帧

    if(ioctl(fd, VIDIOC_S_PARM, &setfps) == -1)
    {
        printf("Unable to set frame rate\n");
        return false;
    }
    else
    {
        printf("set fps OK!\n");
    }

    if(ioctl(fd, VIDIOC_G_PARM, &setfps) == -1)
    {
        printf("Unable to get frame rate\n");
        return false;
    }
    else
    {
        printf("get fps OK!\n");
        printf("timeperframe.numerator:\t%d\n",setfps.parm.capture.timeperframe.numerator);
        printf("timeperframe.denominator:\t%d\n",setfps.parm.capture.timeperframe.denominator);

    }

    //mmap
    if(init_mmap() == false )
    {
        printf("cannot mmap!\n");
        return false;

    }

    return true;
}

int VideoDevice::init_mmap()
{
    v4l2_requestbuffers req;

    req.count = 2;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
    {
        return false;
    }

    if(req.count < 2)
    {
        return false;
    }

    buffers = (buffer*)calloc(req.count, sizeof(*buffers));

    if(!buffers)
    {
        return false;
    }

    for(n_buffers = 0; n_buffers < req.count; ++n_buffers)
    {
        v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if(ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            return false;
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
                mmap(NULL, // start anywhere// allocate RAM*4
                     buf.length,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fd, buf.m.offset);

        if(MAP_FAILED == buffers[n_buffers].start)
        {

            return false;
        }
    }
    return true;

}

int VideoDevice::start_capturing()
{
    unsigned int i;
    for(i = 0; i < n_buffers; ++i)
    {
        v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory =V4L2_MEMORY_MMAP;
        buf.index = i;
        if(-1 == ioctl(fd, VIDIOC_QBUF, &buf))
        {
            return false;
        }
    }

    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(-1 == ioctl(fd, VIDIOC_STREAMON, &type))
    {
        return false;
    }
    return true;
}

int VideoDevice::stop_capturing()
{
    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        return false;
    }
    return true;
}

int VideoDevice::uninit_device()
{
    unsigned int i;

    for(i = 0; i < n_buffers; ++i)
    {
        if(-1 == munmap(buffers[i].start, buffers[i].length))
        {
            printf("munmap error\n");
            return false;
        }

    }

    delete buffers;
    return true;
}

int VideoDevice::get_frame(unsigned char ** yuv_buffer_pointer, size_t* len)
{
    v4l2_buffer queue_buf;

    queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queue_buf.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_DQBUF, &queue_buf) == -1)
    {
        return false;
    }

    *yuv_buffer_pointer = (unsigned char *)buffers[queue_buf.index].start;
    *len = buffers[queue_buf.index].length;
    index = queue_buf.index;

    return true;

}

int VideoDevice::unget_frame()
{
    if(index != -1)
    {
        v4l2_buffer queue_buf;
        queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        queue_buf.memory = V4L2_MEMORY_MMAP;
        queue_buf.index = index;

        if(ioctl(fd, VIDIOC_QBUF, &queue_buf) == -1)
        {
            return false;
        }
        return true;
    }
    return false;
}




/*yuv格式转换为rgb格式*/
int VideoDevice::convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
 unsigned int in, out = 0;
 unsigned int pixel_16;
 unsigned char pixel_24[3];
 unsigned int pixel32;
 int y0, u, y1, v;
 for(in = 0; in < width * height * 2; in += 4) {
  pixel_16 =
   yuv[in + 3] << 24 |
   yuv[in + 2] << 16 |
   yuv[in + 1] <<  8 |
   yuv[in + 0];
  y0 = (pixel_16 & 0x000000ff);
  u  = (pixel_16 & 0x0000ff00) >>  8;
  y1 = (pixel_16 & 0x00ff0000) >> 16;
  v  = (pixel_16 & 0xff000000) >> 24;
  pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
  pixel_24[0] = (pixel32 & 0x000000ff);
  pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
  pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
  rgb[out++] = pixel_24[0];
  rgb[out++] = pixel_24[1];
  rgb[out++] = pixel_24[2];
  pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
  pixel_24[0] = (pixel32 & 0x000000ff);
  pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
  pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
  rgb[out++] = pixel_24[0];
  rgb[out++] = pixel_24[1];
  rgb[out++] = pixel_24[2];
 }
 return 0;
}

int VideoDevice::convert_yuv_to_rgb_pixel(int y, int u, int v)
{
 unsigned int pixel32 = 0;
 unsigned char *pixel = (unsigned char *)&pixel32;
 int r, g, b;
 r = y + (1.370705 * (v-128));
 g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
 b = y + (1.732446 * (u-128));
 if(r > 255) r = 255;
 if(g > 255) g = 255;
 if(b > 255) b = 255;
 if(r < 0) r = 0;
 if(g < 0) g = 0;
 if(b < 0) b = 0;
 pixel[0] = r * 220 / 256;
 pixel[1] = g * 220 / 256;
 pixel[2] = b * 220 / 256;
 return pixel32;
}
/*yuv格式转换为rgb格式*/




