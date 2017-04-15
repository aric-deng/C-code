/*
 * uvc_host.h
 *
 *  Created on: 2016年12月30日
 *      Author: jia
 */

#ifndef SRC_UVC_HOST_H_
#define SRC_UVC_HOST_H_
#define UVC_HOST_VERSION  "1.0.0.1"
#define UVC_DEV_NAME_MAX_LEN    32

struct st_uvc_buf
{
	void *start;
	size_t length;
};

typedef struct st_uvc_reso{
    int h;
    int w;
} ST_UVC_RESO;

typedef void (*capture_data_callback_t)(unsigned char *buf, int len);

typedef struct st_camera
{
	char device_name[UVC_DEV_NAME_MAX_LEN];
	int fd;
	int format;
	int width;
	int height;
	int display_depth;
	int image_size;
	int frame_number;
	struct st_uvc_buf *buffers;
	int buf_num;
	capture_data_callback_t fun;
} ST_CAMERA;

/*start up uvc thread*/
int uvc_host_start(void);

/*Be used to register a callback function.Uvc capture thread will call
 * the registered function when get a new frame data
 * RETURN VALUE:return 0;or -1 if an error occurred.
 * */
int uvc_host_register_capture_cb_func(capture_data_callback_t p);

#endif /* SRC_UVC_HOST_H_ */
