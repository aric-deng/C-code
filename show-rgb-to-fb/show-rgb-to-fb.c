#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;


typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
}__attribute__((packed)) BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
}__attribute__((packed)) BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
}__attribute__((packed)) RGBQUAD;


#define FRAME_BUFFER_PATH                "/dev/fb0"

static unsigned short  * pfbBuf = NULL;

int main(int argc, char *argv[])
{  
	int i, ret;
	int img_fd, fb_fd;
	int data_size;
	char *img_buf;
	struct fb_var_screeninfo var_info;
	struct fb_fix_screeninfo fixInfo;
	BITMAPFILEHEADER file_head;
	BITMAPINFOHEADER info_head;
	FILE *fp;
	//RGBQUAD rgb_quad;
	
	if (argc != 2)
	{
			printf("usage %s bmp_file\n", argv[0]);
			return -1;
	}        
	
	/*open files*/
	fb_fd = open(FRAME_BUFFER_PATH, O_RDWR);
	if (fb_fd < 0)
	{
			perror("open framebuff");
			return -1;
	}
	if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &var_info) < 0)
	{
			perror("ioctl FBIOGET_VSCREENINFO");
			close(fb_fd);
			return 0;
	}
	
	if(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fixInfo) < 0)
    {
        printf("Cannot get fixed screen info\n.");
        close (fb_fd);
        return -1;
    }
	
	printf("xres %d, yres %d, mem_len:%d\n", var_info.xres, var_info.yres, fixInfo.smem_len);
	pfbBuf = (unsigned short *)mmap(NULL, fixInfo.smem_len,
                                      PROT_WRITE,  MAP_SHARED, fb_fd, 0);
	
	fp = fopen(argv[1], "r");	
	if (fp != NULL){
		fseek(fp, 0, SEEK_END);
		data_size = ftell(fp);
		fclose(fp);
	}
	
	if (data_size <= 0){
		data_size = fixInfo.smem_len;		
	}
	
	img_fd = open(argv[1], O_RDONLY);
	if (img_fd < 0)
	{
		perror("open image");
		return -1;
	}
	
	/*read img data and */
	ret = read(img_fd, pfbBuf, data_size);

	printf("read data_size:%d \n", ret);
	//write(img_fd, &file_head, sizeof(file_head));
	//write(img_fd, &info_head, sizeof(info_head));
	//printf("file_head size:%d, info_head size:%d, data_size:%d \n", sizeof(file_head), sizeof(info_head), data_size);
	
#if 0	
	/*reverse img and write to file*/
	for (i = 0; i < var_info.yres; i++)
	{
	  write(img_fd, img_buf + var_info.xres * i * (var_info.bits_per_pixel/8), 
			  var_info.xres * (var_info.bits_per_pixel/8));
	}
#endif

	//flush fb show
    var_info.yoffset= 0;
    if(ioctl(fb_fd, FBIOPAN_DISPLAY, &var_info) < 0)
    {
        printf("Cannot display Pan\n.");
        close (fb_fd);
        return -1;
    }
	close(fb_fd);
	close(img_fd);
    return 0;
}