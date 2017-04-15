/*用uvc_host库，写个测试ｕｖｃ　host 的单独运行进程*/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "uvc_host.h"

#define TEST_CODE
//#define CAP_YUV_DATA
//#define CAP_H264_DATA

#if 0
int make_dir(void)
{
  struct stat  buf;
  int ret;
  char *path="/temp";
  if(stat(path,&buf) == 0){
    ret=rmdir(path);
    if(ret != 0){
      printf("rmdir fail\n");
      return -1 ;
    }
  }

  ret=mkdir(path,0x777);
  if(ret != 0){
    printf("mkdir fail ret=%d\n",ret);
    return -1;
  }
  return ret;
}

void save_frame_as_pictures(unsigned char *buf,int len)
{
  static int count=0;
  char file_name[32];
  int fd;
  if(count == 0)
    make_dir();
  count ++;
  memset(file_name,0,32);
  sprintf(file_name,"/temp/%dx%d_%d.jpg",cam->width,cam->height,count);
  fd=open(file_name,O_CREAT|O_RDWR);
  if(fd < 0){
    printf("open file:%s failed\n",file_name);
    return ;
  }

  write(fd,buf,len);
  close(fd);
}
#endif
#ifdef  TEST_CODE

int make_dir(void)
{
	struct stat  buf;
	int ret;
	char *path="./mjpg";
	
#if 0	
	if(stat(path,&buf) == 0){
		ret=rmdir(path);
		if(ret != 0){
			printf("rmdir fail\n");
			return -1 ;
		}
	}
#endif

	ret=mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
	if(ret != 0){
		printf("mkdir fail ret=%d\n",ret);
		return -1;
	}
	return ret;
}

void save_mjpg_data_to_file(unsigned char *buf,int len)
{
	static int count=0, size_cnt=0;
	time_t tt;
	char sTime[32] = {0};
	static char sLastTime[32] = {0};
	char file_name[64];
	int fd;

    count++;
    size_cnt += len;
	time(&tt);
	strftime(sTime, sizeof(sTime), "%M:%S", localtime(&tt));
    if(strncmp(sLastTime, sTime, sizeof(sTime))){
        printf("=== dhs, time change, last cnt:%d\n", count - 1);
        count = 1;
        size_cnt = len;
    }
	strncpy(sLastTime, sTime, sizeof(sTime));
	printf("%s, data len:%d, cnt:%d, total size_cnt in 1s:%dKB, cur_time:%s \n", __func__, len, count, size_cnt/1024, sTime);
	return;
	
	printf("%s, data len:%d \n", __func__, len);

	if(count == 0)
		make_dir();
		
	count ++;
	memset(file_name,0,64);
	time(&tt);
	strftime(sTime, sizeof(sTime), "%Y-%m-%d_%H_%M_%S", localtime(&tt));
	//sprintf(file_name,"./mjpg/%s_%dx%d_%d.jpg", sTime, p_st_cam->width, p_st_cam->height, count);
	sprintf(file_name,"./mjpg/%s_%d.jpg", sTime, count);
	fd=open(file_name, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if(fd < 0){
		printf("open file:%s failed\n",file_name);
		return ;
	}

	write(fd,buf,len);
	printf("save the %d files\n", count);
	close(fd);
}

#define SAVE_YUV_FRAME_NUM  10000
void save_yuv_data_to_file(unsigned char *buf,int len)
{
    static int fd = 0, saveFlag = 1, cnt=0;
    time_t tt;
	char sTime[32] = {0};
	char file_name[64] = {0};
    int ret;

    if(cnt >= SAVE_YUV_FRAME_NUM)
        return;
        
	printf("%s, data len:%d \n", __func__, len);

	if (fd > 0){
    	ret = write(fd,buf,len);
    	if (ret < 0){
    		printf("write file:%s failed\n", file_name);
    		return;
    	}
    	cnt ++;
    	if (cnt >= SAVE_YUV_FRAME_NUM){
            printf("close file.\n");
            close(fd);
            sync();
    	}
	} else if (saveFlag == 1){
	    
        time(&tt);
        strftime(sTime, sizeof(sTime), "%Y-%m-%d_%H_%M_%S", localtime(&tt));
        //sprintf(file_name,"./%s_%dx%d.yuv", sTime, p_st_cam->width,p_st_cam->height);
        sprintf(file_name,"./%s.yuv", sTime);

        fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    	if(fd < 0){
    		printf("open file:%s failed\n", file_name);
    		return ;
		}
    	ret = write(fd,buf,len);
    	if (ret < 0){
    		printf("write file:%s failed\n", file_name);
    		return;
    	}
    	cnt ++;
	}
}

#define SAVE_H264_FRAME_NUM  10000
void save_h264_data_to_file(unsigned char *buf,int len)
{
    static int fd = 0, saveFlag = 1, cnt=0;
    time_t tt;
	char sTime[32] = {0};
	char file_name[64] = {0};
    int ret;

    if(cnt >= SAVE_H264_FRAME_NUM)
        return;

	printf("%s, data len:%d \n", __func__, len);
        
	if (fd > 0){
    	ret = write(fd,buf,len);
    	if (ret < 0){
    		printf("write file:%s failed\n", file_name);
    		return;
    	}
    	cnt ++;
    	if (cnt >= SAVE_H264_FRAME_NUM){
            printf("close file.\n");
            close(fd);
            sync();
    	}
	} else if (saveFlag == 1){
	    
        time(&tt);
        strftime(sTime, sizeof(sTime), "%Y-%m-%d_%H_%M_%S", localtime(&tt));
        //sprintf(file_name,"./%s_%dx%d.h264", sTime, p_st_cam->width,p_st_cam->height);
        sprintf(file_name,"./%s.h264", sTime);

        fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    	if(fd < 0){
    		printf("open file:%s failed\n", file_name);
    		return ;
		}
    	ret = write(fd,buf,len);
    	if (ret < 0){
    		printf("write file:%s failed\n", file_name);
    		return;
    	}
    	cnt ++;
	}
}

#endif

void send_data_to_stream_lib(unsigned char *buf,int len)
{
    printf("send_data_to_stream_lib, len:%d \n", len);
}

extern void save_mjpg_data_to_file(unsigned char *buf,int len);

int main(int argc,char *argv[])
{
  
  uvc_host_start();
  uvc_host_register_capture_cb_func(&save_mjpg_data_to_file);
  //uvc_host_register_capture_cb_func(&send_data_to_stream_lib);
  while(1){
    sleep(5);
    //printf("in sleeping\n");
  }
  return 0;
}
