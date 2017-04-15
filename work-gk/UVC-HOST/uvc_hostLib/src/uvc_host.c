#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <linux/usb/ch9.h>
#include <linux/usb/video.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include "uvc_host.h"

#define DBG_LOG
#ifdef DBG_LOG
#define UVC_HOST_DBG(msg...)\
  printf("[uvc_host]"msg)
#else
#define UVC_HOST_DBG(...)
#endif

#define MAX_DEVICE_NUM	10

static int cap_type = -1;
static int cap_h    = 0;
static int cap_w    = 0;

static ST_CAMERA *p_st_cam=NULL;
int uvc_host_open_dev(ST_CAMERA *);
static int uvc_host_init(void);
void uvc_host_close_dev(void);
void uvc_host_stream_off(ST_CAMERA *p_st_cam);
int uvc_host_munmap(ST_CAMERA *p_st_cam);
int uvc_host_chk_dev_capability(ST_CAMERA *);
int uvc_host_set_cap_fmt(ST_CAMERA *);
void uvc_host_capture_start(void);

static int GK_SetPthreadStackSize(pthread_attr_t * attr, int stacksize)
{
    int pret = -1;

    pret = pthread_attr_init(attr);
    if (pret != 0)
    {
        return -1;
    }

    pret = pthread_attr_setstacksize(attr, stacksize);
    if (pret != 0)
    {
        return -1;
    }

    return 0;
}

int uvc_host_init(void)
{
    int ret = 0;
	UVC_HOST_DBG("libuvc_host.a version:%s\n", UVC_HOST_VERSION);
	UVC_HOST_DBG("compile time:%s-%s\n", __DATE__, __TIME__);
	p_st_cam = (ST_CAMERA *) malloc(sizeof(ST_CAMERA));
	if (p_st_cam == NULL)
	{
		UVC_HOST_DBG("malloc p_st_cam fail, uvc_host_init fail!\n");
		return -1;
	}
	memset(p_st_cam, 0, sizeof(ST_CAMERA));

    return ret;
}

void uvc_host_deinit(void)
{
    if(p_st_cam){
        uvc_host_close_dev();
        free(p_st_cam);
    }

    p_st_cam = NULL;
}

int uvc_host_open_dev(ST_CAMERA * p)
{
	int i;
	int fd = 0;
	char filename[UVC_DEV_NAME_MAX_LEN];

	for (i = 0; i < MAX_DEVICE_NUM; i++)
	{
		sprintf(filename, "/dev/video%d", i);
		fd = open(filename, O_RDWR);
		//UVC_HOST_DBG("UVC device name:%s, fd=%d \n", filename, fd);

		if (fd > 0)
			break;
	}
	
	if (fd <= 0)
		return -1;
		
	strncpy(p->device_name, filename, UVC_DEV_NAME_MAX_LEN);
	p->fd = fd;
	
	return 0;
}

void uvc_host_close_dev(void)
{   
    capture_data_callback_t p;

    if (p_st_cam->fd > 0){
        UVC_HOST_DBG("%s\n", __func__);
        
    	uvc_host_stream_off(p_st_cam);
    	uvc_host_munmap(p_st_cam);
    	close(p_st_cam->fd);

        p = p_st_cam->fun;
	    //clear all struct data except the callback function.
        memset(p_st_cam, 0, sizeof(ST_CAMERA));
        p_st_cam->fun = p;
    }
}

int uvc_host_get_dev_fd(void)
{
	if(p_st_cam != NULL){
		return p_st_cam->fd;
    } else {
        return -1;
    }
}

int uvc_host_chk_dev_capability(ST_CAMERA * p)
{
	int ret;
	struct v4l2_capability cap;

    if (!p){
        UVC_HOST_DBG("p is NULL !\n");
        return -1;
    }
    
	ret = ioctl(p->fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0)
	{
		 UVC_HOST_DBG("unable to query device: %s (%d)\n", strerror(errno), errno);
		return -1;
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		UVC_HOST_DBG("device not support V4L2_CAP_VIDEO_CAPTURE !\n");
		return -1;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		UVC_HOST_DBG("device not support V4L2_CAP_STREAMING !\n");
		return -1;
	}
	
	UVC_HOST_DBG("device is [%s] on bus [%s]\n", cap.card, cap.bus_info);

	return 0;
}

int uvc_host_mmap(ST_CAMERA *p_st_cam)
{
	struct v4l2_requestbuffers req;
	int ret;
	int i;

	memset(&req,0,sizeof(req));

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(p_st_cam->fd, VIDIOC_REQBUFS, &req);
	if (ret)
	{
		UVC_HOST_DBG("get memory faile\n");
		return -1;
	}

	if (req.count < 2)
	{
		UVC_HOST_DBG("memory is not enough\n");
		return -1;
	}
	
	p_st_cam->buf_num=req.count;
	p_st_cam->buffers = calloc(req.count, sizeof(*(p_st_cam->buffers)));

	if (!p_st_cam->buffers)
	{
		UVC_HOST_DBG("get memory fail\n");
		return -1;
	}

	for (i = 0; i < req.count; ++i)
	{
		struct v4l2_buffer buf;

		memset(&buf,0,sizeof(buf));

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		//将VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
		if (-1 == ioctl(p_st_cam->fd, VIDIOC_QUERYBUF, &buf))
		{
			UVC_HOST_DBG("VIDIOC_QUERYBUF fail\n");
			return -1;
		}

		p_st_cam->buffers[i].length = buf.length;
		p_st_cam->buffers[i].start = mmap(NULL /* start anywhere */,
				buf.length, PROT_READ | PROT_WRITE /* required */,
				MAP_SHARED /* recommended */, p_st_cam->fd, buf.m.offset);

		if (MAP_FAILED == p_st_cam->buffers[i].start)
		{
			UVC_HOST_DBG("MAP_FAILED\n");
			return  -1;
		}
	}

	return 0;
}

int uvc_host_munmap(ST_CAMERA *p_st_cam)
{
	int i,buf_num;

	buf_num=p_st_cam->buf_num;
	for(i=0;i<buf_num;i++)
		munmap(p_st_cam->buffers[i].start, p_st_cam->buffers[i].length);
	free(p_st_cam->buffers);

	return 0;
}

int uvc_host_set_cap_fmt(ST_CAMERA *p_st_cam)
{
	int ret, i, select_reso;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format fmt;
	struct v4l2_frmsizeenum fsize;
	int priority, best_priority = 0;
	int max_width = 0, max_height = 0;
	const unsigned int fmt_type[3] =
	{ V4L2_PIX_FMT_YUYV, V4L2_PIX_FMT_H264, V4L2_PIX_FMT_MJPEG };

	memset(&fmtdesc, 0, sizeof(fmtdesc));
	memset(&fmt, 0, sizeof(fmt));

	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmtdesc.index = 0;
	while ((ret = ioctl(p_st_cam->fd, VIDIOC_ENUM_FMT, &fmtdesc)) == 0)
	{
	    //supports how many types of fmtdesc
		fmtdesc.index++;
		UVC_HOST_DBG("======== device can support:format fourcc=%s =======\n",
				(char * ) (&fmtdesc.pixelformat));

		if (V4L2_PIX_FMT_YUYV == fmtdesc.pixelformat)
			priority = 1;
		else if (V4L2_PIX_FMT_H264 == fmtdesc.pixelformat)
			priority = 2;
		else if (V4L2_PIX_FMT_MJPEG == fmtdesc.pixelformat)
			priority = 3;
		else
			priority = 0;

		if (priority > best_priority)
			best_priority = priority;

        if ( priority > 0 && priority < 4 ) {
    		memset(&fsize, 0, sizeof(fsize));
        	fsize.index = 0;
        	fsize.pixel_format = fmt_type[priority - 1];
        	UVC_HOST_DBG("enum the %dth format supported framesize.\n", priority);
        	while ((ret = ioctl(p_st_cam->fd, VIDIOC_ENUM_FRAMESIZES, &fsize)) == 0)
        	{
        	    //supports how many types of fsize.
        		fsize.index++;
        		UVC_HOST_DBG("fsize.index=%d\n", fsize.index);
        		//UVC_HOST_DBG("fsize.type=%d\n", fsize.type);
        		//UVC_HOST_DBG("fsize.pixel_format=0x%x\n", fsize.pixel_format);
        		if (V4L2_FRMSIZE_TYPE_DISCRETE == fsize.type)
        		{
        			UVC_HOST_DBG("fsize.discrete.width=%d\n", fsize.discrete.width);
        			UVC_HOST_DBG("fsize.discrete.height=%d\n", fsize.discrete.height);
        			if ((fsize.discrete.width * fsize.discrete.height> max_width * max_height) && 
        			        (fsize.discrete.width * fsize.discrete.height <= 1280*720))
        			{
        				max_width = fsize.discrete.width;
        				max_height = fsize.discrete.height;
        			}
        		}

        	}	
    	}
	}
	
	if (best_priority <= 0 || best_priority > 3)
	{
		UVC_HOST_DBG("get format error\n");
		return -1;
	}
	
#if 0	
    printf("please input captype:[1:YUYV, 2:H264, 3:MJPG]");
	scanf("%d", &cap_type);

    printf("please select input resolution:\n");
    printf("1. 320x240\n2. 640x480\n3. 1280x720\n4. 1920x1080\n5. 2592x1944\n6. 3264x2448\ninput:");
	scanf("%d", &select_reso);

    switch (select_reso)
    {
        case 1:
            cap_w = 320;
            cap_h = 240;
            break;
        case 2:
            cap_w = 640;
            cap_h = 480;
            break;
        case 3:
            cap_w = 1280;
            cap_h = 720;
            break;
        case 4:
            cap_w = 1920;
            cap_h = 1080;
            break;
        case 5:
            cap_w = 2952;
            cap_h = 1944;
            break;
        case 6:
            cap_w = 3264;
            cap_h = 2448;
            break;
        default:
            break;
    }
#endif

	p_st_cam->format= cap_type > 0? cap_type : best_priority;
    if (cap_type < 0){
        cap_type = p_st_cam->format;
    }
    
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = cap_w > 0? cap_w : max_width;
	fmt.fmt.pix.height = cap_h > 0? cap_h : max_height;
	fmt.fmt.pix.pixelformat = fmt_type[p_st_cam->format - 1];
	fmt.fmt.pix.field = V4L2_FIELD_ANY;
	
	UVC_HOST_DBG("==== capture fmt:0x%x, reso: w:%d, h:%d \n", fmt.fmt.pix.pixelformat, fmt.fmt.pix.width, fmt.fmt.pix.height);
	for (i = 0; i < 5; i++)
	{
		ret = ioctl(p_st_cam->fd, VIDIOC_S_FMT, &fmt);
		if (ret)
		{
			UVC_HOST_DBG("%d time to try format fail ret=%d\n", i, ret);
			sleep(1);
		}
		else
		{
			break;
		}
	}
	if (i == 5)
		return -1;
//	ret = ioctl(p_st_cam->fd, VIDIOC_G_FMT, &fmt);
//	if (ret)
//	{
//		UVC_HOST_DBG("VIDIOC_G_FMT fail ret=%d\n", ret);
//	}
//	else
//	{
//		UVC_HOST_DBG("VIDIOC_G_FMT success width=%d,height=%d format=%x\n",
//				fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.pixelformat);
//	}
	p_st_cam->width = fmt.fmt.pix.width;
	p_st_cam->height = fmt.fmt.pix.height;
	return 0;
}

int uvc_host_set_stream_on(ST_CAMERA *p_st_cam)
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < p_st_cam->buf_num; ++i)
	{
		struct v4l2_buffer buf;

		memset(&buf,0,sizeof(buf));

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == ioctl(p_st_cam->fd, VIDIOC_QBUF, &buf)){
			UVC_HOST_DBG("VIDIOC_QBUF command failed\n");
			return -1;
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == ioctl(p_st_cam->fd, VIDIOC_STREAMON, &type)){
		UVC_HOST_DBG("VIDIOC_STREAMON command failed\n");
		return -1;
	}
	
    return 0;
}

void uvc_host_stream_off(ST_CAMERA *p_st_cam)
{
	int type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

	ioctl(p_st_cam->fd, VIDIOC_STREAMOFF, &type);
}

int uvc_host_read_one_frame_data(ST_CAMERA *p_st_cam)
{
	struct v4l2_buffer buf;

//	UVC_HOST_DBG("in read_frame\n");

	memset(&buf,0,sizeof(buf));

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	//this operator below will change buf.index and (0 <= buf.index <= 3)
	if (-1 == ioctl(p_st_cam->fd, VIDIOC_DQBUF, &buf))
	{
		//UVC_HOST_DBG("VIDIOC_DQBUF error\n");
		return -1;
	}
	
//	UVC_HOST_DBG("buf.bytesused=%d\n", buf.bytesused);
	if(p_st_cam->fun)
		p_st_cam->fun(p_st_cam->buffers[buf.index].start, buf.bytesused);
	//	fwrite(p_st_cam->buffers[buf.index].start, buf.bytesused, 1, yuv_fp);
	
	if (-1 == ioctl(p_st_cam->fd, VIDIOC_QBUF, &buf)){
		UVC_HOST_DBG("VIDIOC_QBUF");
		return -1;
	}

	return 0;
}

/*if p_st_cam->device_name exist,return 0;otherwidth  return -1*/
int uvc_host_check_device_exist(ST_CAMERA *p_st_cam)
{
	struct stat buf;
	int ret;
	
	ret = stat(p_st_cam->device_name, &buf);
	if(ret != 0)
		UVC_HOST_DBG("%s isn't exist\n",p_st_cam->device_name);
		
	return ret;
}

int uvc_host_register_capture_cb_func(capture_data_callback_t p)
{
	if ((p == NULL) ||  (p_st_cam == NULL))
		return -1;
	p_st_cam->fun = p;
	return 0;
}

int uvc_host_get_dev_vendor_name(char *vendor_buf,int len)
{
	int ret;
	struct v4l2_capability cap;

	if(p_st_cam == NULL)
		return -1;

	if( (vendor_buf  == NULL) || (len <=0))
		return -1;
	memset(&cap,0,sizeof(cap));

	ret = ioctl(p_st_cam->fd, VIDIOC_QUERYCAP, &cap);
	if (ret < 0)
	{
		 UVC_HOST_DBG("unable to query device: %s (%d)\n", strerror(errno), errno);
		return -1;
	}
	strncpy(vendor_buf,cap.card,len);
		
		return 0;
}

int uvc_host_get_cur_fmt(void)
{
	if(p_st_cam == NULL)
		return -1;
	return p_st_cam->format;
}

#if 0
int uvc_host_get_cur_reso(int * width, int * height)
{
	if(p_st_cam == NULL || width == NULL || height == NULL)
		return -1;
	*width=p_st_cam->width;
	*height=p_st_cam->height;
	return 0;
}
#endif

int uvc_host_get_cur_reso(int fd, ST_UVC_RESO* reso)
{
    struct v4l2_format fmt;

    if (fd < 0 || reso == NULL){
        return -1;
    }
    
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) < 0){
        return -1;
    }

    reso->h = fmt.fmt.pix.height;
    reso->w = fmt.fmt.pix.width;
    return 0;
}

int uvc_host_set_cur_reso(int fd, ST_UVC_RESO* reso)
{
    struct v4l2_format fmt;

    if (fd < 0 || reso == NULL){
        return -1;
    }
    
    if(ioctl(fd, VIDIOC_G_FMT, &fmt) < 0){
        return -1;
    }

    fmt.fmt.pix.height = reso->h;
    fmt.fmt.pix.width = reso->w;

    if(ioctl(fd, VIDIOC_S_FMT, &fmt) < 0){
        return -1;
    }
    
    return 0;
}

int uvc_host_get_fps(int fd, struct v4l2_fract * timeperframe)
{
    struct v4l2_streamparm parm;
    
    if (fd < 0 || timeperframe == NULL){
        return -1;
    }

    if(ioctl(fd, VIDIOC_G_PARM, &parm) < 0){
        return -1;
    }

    *timeperframe = parm.parm.capture.timeperframe;

    return 0;
}

int uvc_host_set_fps(int fd, struct v4l2_fract * timeperframe)
{
    struct v4l2_streamparm parm;
    
    if (fd < 0 || timeperframe == NULL){
        return -1;
    }

    if(ioctl(fd, VIDIOC_G_PARM, &parm) < 0){
        return -1;
    }

    parm.parm.capture.timeperframe = *timeperframe;

    if(ioctl(fd, VIDIOC_S_PARM, &parm) < 0){
        return -1;
    }
    
    return 0;
}

int uvc_host_query_ctrl(int fd, struct v4l2_queryctrl *qCtrl)
{
    if (fd < 0 || qCtrl == NULL){
        return -1;
    }
    
    if (ioctl(fd, VIDIOC_QUERYCTRL, qCtrl) == 0)
    {
        UVC_HOST_DBG("qCtrl name:%s, min:%d, max:%d, step:%d, def:%d \n", qCtrl->name, qCtrl->minimum, qCtrl->maximum, qCtrl->step, qCtrl->default_value);
    } else {
        return -1;
    }

    return 0;
}

int uvc_host_get_dev_ctrl(int fd, __u32 id, int* pVal)
{
    struct v4l2_queryctrl qCtrl;
    struct v4l2_control ctrl;
    
    if (fd < 0){
        return -1;
    }
    
    memset(&qCtrl, 0, sizeof(qCtrl));
    qCtrl.id = id;

    if (uvc_host_query_ctrl(fd, &qCtrl) == -1){
        return -1;
    } else {
        memset (&ctrl, 0, sizeof(ctrl));
        ctrl.id = id;
        if(ioctl(fd, VIDIOC_G_CTRL, &ctrl) == -1){
            return -1;
        }
    }

    *pVal = ctrl.value;
    return 0;
}

int uvc_host_set_dev_ctrl(int fd, __u32 id, __s32 val)
{
    struct v4l2_queryctrl qCtrl;
    struct v4l2_control ctrl;
    
    if (fd < 0){
        return -1;
    }
    
    memset(&qCtrl, 0, sizeof(qCtrl));
    qCtrl.id = id;

    if (uvc_host_query_ctrl(fd, &qCtrl) == -1){
        return -1;
    } else {
        memset (&ctrl, 0, sizeof (ctrl));
        ctrl.id = id;
        ctrl.value = val;
        if(ioctl(fd, VIDIOC_S_CTRL, &ctrl) == -1){
            return -1;
        }
    }

    return 0;
}


//int uvc_host_get_dev_brightness(int fd, int* pVal)
int uvc_host_get_dev_brightness(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_BRIGHTNESS, pVal);
}

//int uvc_host_set_dev_brightness(int fd, int val)
int uvc_host_set_dev_brightness(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_BRIGHTNESS, val);
}

int uvc_host_get_dev_contrast(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_CONTRAST, pVal);
}

int uvc_host_set_dev_contrast(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_CONTRAST, val);
}

int uvc_host_get_dev_saturation(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_SATURATION, pVal);
}

int uvc_host_set_dev_saturation(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_SATURATION, val);
}

int uvc_host_get_dev_hue(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_HUE, pVal);
}

int uvc_host_set_dev_hue(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_HUE, val);
}

int uvc_host_get_dev_gamma(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_GAMMA, pVal);
}

int uvc_host_set_dev_gamma(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_GAMMA, val);
}

int uvc_host_get_dev_gain(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_GAIN, pVal);
}

int uvc_host_set_dev_gain(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_GAIN, val);
}

int uvc_host_get_dev_sharpness(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_SHARPNESS, pVal);
}

int uvc_host_set_dev_sharpness(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_SHARPNESS, val);
}

//backlight_compensation
int uvc_host_get_dev_bl_comp(int* pVal)
{
    int fd = uvc_host_get_dev_fd();
    
    if (fd < 0){
        return -1;
    }
    
    return uvc_host_get_dev_ctrl(fd, V4L2_CID_BACKLIGHT_COMPENSATION, pVal);
}

int uvc_host_set_dev_bl_comp(int val)
{
    int fd = uvc_host_get_dev_fd();

    if (fd < 0){
        return -1;
    }
    
    return uvc_host_set_dev_ctrl(fd, V4L2_CID_BACKLIGHT_COMPENSATION, val);
}

int uvc_host_enum_ctrl(int fd)
{
    struct v4l2_queryctrl qCtrl;

    if (fd < 0){
        return -1;
    }
    
    memset(&qCtrl, 0, sizeof(qCtrl));
    qCtrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while(ioctl(fd, VIDIOC_QUERYCTRL, &qCtrl) == 0)
    {
        UVC_HOST_DBG("qCtrl name:%s, min:%d, max:%d, step:%d, def:%d \n", qCtrl.name, qCtrl.minimum, qCtrl.maximum, qCtrl.step, qCtrl.default_value);
        qCtrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }

    return 0;
}

int uvc_host_cap_data(void)
{
    fd_set fds;
    struct timeval tv;
    int ret, is_ng;

    while(1){
    
        is_ng = 1;
        FD_ZERO(&fds);
        FD_SET(p_st_cam->fd, &fds);

        /* set timeout period. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;
        
        ret = select(p_st_cam->fd + 1, &fds, NULL, NULL, &tv);

        //readfds ready
        if (ret == 1){
            if(uvc_host_read_one_frame_data(p_st_cam) == 0){
                is_ng = 0;
            }
        } else {
            if (ret == 0)
            {
                UVC_HOST_DBG("time out, ret:%d\n", ret);
            } else {
                UVC_HOST_DBG("select ret abnormal, ret:%d\n", ret);
            }
        }

        if(is_ng){
            if(uvc_host_check_device_exist(p_st_cam) != 0){
                break;
            }
        }
    }

    return -1;
}

void uvc_host_main_thread(void)
{
    if (pthread_detach(pthread_self()) != 0)
    {
        UVC_HOST_DBG("%s, pthread_detach err !\n", __func__);
    }

    while(1)
    {
        if (uvc_host_open_dev(p_st_cam) == -1){
            UVC_HOST_DBG("uvc_host_open_dev fail\n");
            sleep(2);
            continue;
        } else {
            
            if (uvc_host_chk_dev_capability(p_st_cam) < 0){
                UVC_HOST_DBG("uvc_host_chk_dev_capability fail\n");
                goto close_dev;
            }
            
            if (uvc_host_set_cap_fmt(p_st_cam) < 0){
                UVC_HOST_DBG("uvc_host_set_cap_fmt fail\n");
                goto close_dev;
            }

            if(uvc_host_mmap(p_st_cam) < 0){
                UVC_HOST_DBG("uvc_host_set_cap_fmt fail\n");
                goto close_dev;
            }
            
            if(uvc_host_set_stream_on(p_st_cam) < 0){
                UVC_HOST_DBG("uvc_host_set_stream_on fail\n");
                goto close_dev;
            }

            if (uvc_host_cap_data() < 0){
                UVC_HOST_DBG("uvc_host_cap_data fail\n");
                goto close_dev;
            }
        }

close_dev:
        uvc_host_close_dev();
        sleep(2);
    }
}

int uvc_host_start(void)
{
    if (uvc_host_init() < 0){
        UVC_HOST_DBG("uvc_host_init fail\n");
        return -1;
    }

	pthread_t uvcthread;
	pthread_attr_t uvc_thread_attr;
	GK_SetPthreadStackSize(&uvc_thread_attr, 512 * 1024);
	pthread_create(&uvcthread, &uvc_thread_attr, uvc_host_main_thread, NULL);

	return 0;
}

