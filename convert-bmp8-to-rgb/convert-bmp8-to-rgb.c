#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define OUTPUT_RGB_565

#define BMP_HEAD		0x4d42	
#define MAX_COLOR_NUM	256

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

typedef struct tagRGB565 {
	WORD	r : 5;
	WORD	g : 6;
	WORD	b : 5;
}__attribute__((packed)) RGB565QUAD;

typedef unsigned char		UINT8;

RGBQUAD gst_color_board[MAX_COLOR_NUM];

void putchars(char *addr, int cnt)
{
	int i = 0;
	for(; i<cnt; i++){
		putchar(addr[i]);
	}
	printf("\n");
	
}

static int convert_bmp8_to_rgb(char *name)
{
	
	FILE *fp;
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER bih;
	int i, j, tmp;
	int width, height, append;
	UINT8 *p_bmp_data, *prgb, *p;
	RGBQUAD rgb;
	char output_name[256] = {0}, rgb888_name[256] = {0};
	int out_fd, rgb888_fd; 
	char * pSuffix;
	int ret;
	
	fp = fopen(name, "r");
	if (fp){
		ret = fread(&bfh, sizeof(bfh), 1, fp);
		if (bfh.bfType != BMP_HEAD){
			printf("name:%s, bfh.bfType:%x != BMP_HEAD:%x !\n", name, bfh.bfType, BMP_HEAD);
			fclose(fp);
			return -1;
		}
		
		fread(&bih, sizeof(bih), 1, fp);
		
		width = bih.biWidth;
		if (bih.biHeight > 0)
			height = bih.biHeight;
		else
			height = -bih.biHeight;
		
		if (width > 720 || height > 480){
			printf("name:%s, display over range! width:%x, height:%x !\n", name, width, height);
			fclose(fp);
			return -1;
		}
		
		//biBitCount must be 8 => (256 color)
        if (bih.biBitCount != 8)
        {
            printf("name:%s, bih.biBitCount:%d != 8 !\n", name, bih.biBitCount);
            fclose(fp);
            return -1;
        }
		
		//load color table from BMP file to gcolor_board
        fread(&gst_color_board, sizeof(gst_color_board), 1, fp);
		
		//store one 256 color data per 4 Bytes
        ///<  4字节对齐下计算每行冗余
        append = 4 - (width & 0x03);
        if (append == 4)
            append = 0;
		
		p_bmp_data = malloc((width + append) * height);
		ret = fread(p_bmp_data, (width + append) * height, 1, fp);
		fclose(fp);

		prgb = malloc((width + append) * height * 2); //RGB565, 16bit size per pixel
		p = prgb + (width + append) * (height - 1) * 2;
		
		//replace suffix to .rgb565
		strncpy(rgb888_name, name, sizeof(rgb888_name));
		pSuffix = rindex(rgb888_name, '.');
		sprintf(pSuffix, ".rgb888");
		rgb888_fd = open(rgb888_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
		
        //convert BMP bitmap data to RGB565 reversely
        for (i = 0; i < height; i++)
        {
			for (j = 0; j < width; j++){
				tmp = p_bmp_data[i * width + j];
				rgb = gst_color_board[tmp];
				write(rgb888_fd, &rgb, sizeof(rgb) - 1);
			#ifdef OUTPUT_RGB_565
				//RGB565
				p[j*2] = (rgb.rgbGreen << 3 & (0b11100000)) | (rgb.rgbBlue >> 3);
				p[j*2 + 1] = (rgb.rgbRed & (0b11111000)) | (rgb.rgbGreen >> 5);
				
			#else
				//ARGB 4444
				p[j*2] = (rgb.rgbGreen & 0xf0) | (rgb.rgbBlue >> 4);
				p[j*2 + 1] = (rgb.rgbRed >> 4) | 0xf0;	//Alpha 4bit set to 100% not transparent
			#endif
				
			}
			if (p - width * 2 >= prgb)
				p -= width * 2;

        }
		
		free(p_bmp_data);
		close(rgb888_fd);
		
		strncpy(output_name, name, sizeof(output_name));
		pSuffix = rindex(output_name, '.');
	#ifdef OUTPUT_RGB_565
		//RGB565
		sprintf(pSuffix, ".rgb565");
	#else
		//ARGB444
		sprintf(pSuffix, ".rgb444");
	#endif
		out_fd = open(output_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
		if (out_fd < 0){
			printf("open out_fd fail !\n");
			return -1;
		}
		
		ret = write(out_fd, prgb, (width + append) * height * 2);
		free(prgb);
		close(out_fd);
	}
	
	return 0;
}


int main(int argc, char *argv[])
{
    if (argc < 2){
		printf("Usage: %s [FILE]\n", argv[0]);
		return -1;
	}
	    
	char file_name[256] = {0};
	
	strcpy(file_name, argv[1]);

	convert_bmp8_to_rgb(file_name);

	return 0;
}
