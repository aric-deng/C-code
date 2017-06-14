#if 0
extern "C"  
{  
    #include <stdio.h>  
    #include <stdlib.h>  
    #include <jpeglib.h>  
}  
  
typedef int LONG;  
typedef unsigned long DWORD;  
typedef unsigned short WORD;  
  
typedef struct {  
        WORD    bfType;  
        DWORD   bfSize;  
        WORD    bfReserved1;  
        WORD    bfReserved2;  
        DWORD   bfOffBits;  
} BMPFILEHEADER_T;  
  
typedef struct{  
        DWORD      biSize;  
        LONG       biWidth;  
        LONG       biHeight;  
        WORD       biPlanes;  
        WORD       biBitCount;  
        DWORD      biCompression;  
        DWORD      biSizeImage;  
        LONG       biXPelsPerMeter;  
        LONG       biYPelsPerMeter;  
        DWORD      biClrUsed;  
        DWORD      biClrImportant;  
} BMPINFOHEADER_T;  
  
void savebmp(uchar * pdata, char * bmp_file, int width, int height )  
{      //分别为rgb数据，要保存的bmp文件名，图片长宽  
       int size = width*height*3*sizeof(char); // 每个像素点3个字节  
       // 位图第一部分，文件信息  
       BMPFILEHEADER_T bfh;  
       bfh.bfType = (WORD)0x4d42;  //bm  
       bfh.bfSize = size  // data size  
              + sizeof( BMPFILEHEADER_T ) // first section size  
              + sizeof( BMPINFOHEADER_T ) // second section size  
              ;  
       bfh.bfReserved1 = 0; // reserved  
       bfh.bfReserved2 = 0; // reserved  
       bfh.bfOffBits = sizeof( BMPFILEHEADER_T )+ sizeof( BMPINFOHEADER_T );//真正的数据的位置  
  
       // 位图第二部分，数据信息  
       BMPINFOHEADER_T bih;  
       bih.biSize = sizeof(BMPINFOHEADER_T);  
       bih.biWidth = width;  
       bih.biHeight = -height;//BMP图片从最后一个点开始扫描，显示时图片是倒着的，所以用-height，这样图片就正了  
       bih.biPlanes = 1;//为1，不用改  
       bih.biBitCount = 24;  
       bih.biCompression = 0;//不压缩  
       bih.biSizeImage = size;  
       bih.biXPelsPerMeter = 2835 ;//像素每米  
       bih.biYPelsPerMeter = 2835 ;  
       bih.biClrUsed = 0;//已用过的颜色，24位的为0  
       bih.biClrImportant = 0;//每个像素都重要  
       FILE * fp = fopen( bmp_file,"wb" );  
       if( !fp ) return;  
  
       fwrite( &bfh, 8, 1,  fp );//由于linux上4字节对齐，而信息头大小为54字节，第一部分14字节，第二部分40字节，所以会将第一部分补齐为16自己，直接用sizeof，打开图片时就会遇到premature end-of-file encountered错误  
       fwrite(&bfh.bfReserved2, sizeof(bfh.bfReserved2), 1, fp);  
       fwrite(&bfh.bfOffBits, sizeof(bfh.bfOffBits), 1, fp);  
       fwrite( &bih, sizeof(BMPINFOHEADER_T),1,fp );  
       fwrite(pdata,size,1,fp);  
       fclose( fp );  
}  
#elif 1
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>

typedef unsigned short u16;
enum YUV_FMT {
  YUV_FMT_NV16 = 1,
  YUV_FMT_UYV565,
  YUV_FMT_AYUV4444,
  YUV_FMT_NUM = YUV_FMT_AYUV4444
};
/*
y = 0.257*red + 0.504*green + 0.098*blue + 16; 
u = -0.148*red - 0.291*green + 0.439*blue + 128;
v  =  0.439*red - 0.368*green - 0.071*blue + 128;
*/
#define u8 	unsigned char
#define USAGE()		"bmp2yuv pic.bmp yuv_raw.bin\n"
#define BM 19778 // The ASCII code for BM

static void bmp_file_test(FILE* fpbmp)
{
	unsigned short bfType = 0;
	fseek(fpbmp, 0L, SEEK_SET);
	fread(&bfType, sizeof(char), 2, fpbmp);
	if (BM != bfType)
	{
		printf("This file is not bmp file.!!!\n");
		exit(1);
	}
}

static void rgb2yuv(enum YUV_FMT yuv_fmt, unsigned int width, unsigned int height, unsigned char *rgb, unsigned char *yuv)
{
	int i, j, index = 0, p = 0;
	u8 r, g, b;
	int y, u, v, nv_index = 0, nv_start = width*height;
	u8 utemp = 0, vtemp = 0;
	
	if(yuv_fmt == YUV_FMT_NV16) {
	  printf("YUV_FMT_NV16\n");
	} else if(yuv_fmt == YUV_FMT_UYV565) {
	  printf("YUV_FMT_UYV565\n");
	} else if(yuv_fmt == YUV_FMT_AYUV4444) {
	  printf("YUV_FMT_AYUV4444\n");
	} else {
  	printf("Error: unknown yuv format!\n");
  	return;
  }
  	  
	for(i = 0; i <  height; i++)
	{
		for(j = 0; j < width; j ++) {
			index = p%width + (height - i - 1) * width;
			b = rgb[index*3];
			g = rgb[index*3 + 1];
			r = rgb[index*3 + 2];
#define BYTE_TRUNCATE(a)		a = a >	255? 255 :	a <	0? 0 :	a
	
			y = (int)(( 66 * r + 129 * g +  25 * b + 128) >> 8) + 16; 		
      
      // u分量  
      u = (int)((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;  
      // v分量  
      v = (int)((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;  
      
      if(yuv_fmt == YUV_FMT_NV16) {        
        BYTE_TRUNCATE(y);
  			yuv[p] = y;
  			nv_index = nv_start + i*width + j;
  			if((j%2) == 0)  
  			{		    
  			  u = utemp?((int)utemp+u)/2:u;
  		    BYTE_TRUNCATE(u);
  		    yuv[nv_index] = u; 
  		    vtemp = v; 
  		  }
  	    else
  	    {  
  	        v = vtemp?((int)vtemp+v)/2:v;
  	        BYTE_TRUNCATE(v);
  	        yuv[nv_index] = v; 
  	        utemp = u; 
  	    }  
  	  } else if(yuv_fmt == YUV_FMT_UYV565) {
        union uyv565 {
          struct {
          u16 v :5;
          u16 y :6;
          u16 u :5;
          } bit;
          u16 all;
        } *puyv565 = (union uyv565*) &yuv[p*2]; 
        BYTE_TRUNCATE(y);
        BYTE_TRUNCATE(u);
        BYTE_TRUNCATE(v);   
        
        puyv565->bit.u = u>>3;
        puyv565->bit.y = y>>2;
        puyv565->bit.v = v>>3;  	    
  	  } else if(yuv_fmt == YUV_FMT_AYUV4444) {
        union auyv {
          struct {
          u16 v :4;
          u16 u :4;
          u16 y :4;
          u16 a :4;
          } bit;
          u16 all;
        } *pauyv = (union auyv*) &yuv[p*2]; 
        BYTE_TRUNCATE(y);
        BYTE_TRUNCATE(u);
        BYTE_TRUNCATE(v);   
        
        pauyv->bit.a = 1;
        pauyv->bit.u = u>>4;
        pauyv->bit.y = y>>4;
        pauyv->bit.v = v>>4;  	    
  	  } 
  	  
			p++;			
		}
	}
	printf("%s exit\n", __func__);   
}

#define SGKS_PRINTF if(0) printf
static inline void hexdump(void *_buf, int len)
{
	u8 *buf = _buf;
	int i;
	
	SGKS_PRINTF("%s: buf at %p len %d\n", __func__, buf, len);
	for (i = 0; i < len; i++) {
		if ((i % 16) == 0)
			SGKS_PRINTF("%s%s: ", i ? "\n" : "",
							__func__);
		SGKS_PRINTF("%02x ", buf[i]);
	}
	SGKS_PRINTF("\n");
}
struct resolution {
  int w;
  int h;
};
static int change2resolution(unsigned char* irgbbuf, unsigned char* orgbbuf, struct resolution iresol, 
  struct resolution oresol)
{
  int w, h, sw = 0, sh = 0, ew, eh, iw, ih, tmp, ch;
  unsigned char*dumpbuf = (unsigned char*) malloc(oresol.w*oresol.h*3); 
  int revert = 0;
  
  if(oresol.w < oresol.h) {
    int tmp;
    tmp = oresol.w;
    oresol.w = oresol.h;
    oresol.h = tmp;
    revert = 1;
  }
  
  ch = oresol.w*iresol.h/iresol.w;
#define SCALE(x)  (ch <= oresol.h?(x*oresol.w/iresol.w):(x*oresol.h/iresol.h))
#define RSCALE(x) (ch <= oresol.h?(x)*iresol.w/oresol.w:(x)*iresol.h/oresol.h)
  if(ch <= oresol.h) {
    tmp = SCALE(iresol.h);   
    sh = (oresol.h-tmp)/2;
    eh = sh+tmp;
    sw = 0;
    ew = oresol.w;   
  } else {      
    tmp = SCALE(iresol.w);   
    sw = (oresol.w-tmp)/2;
    ew = sw+tmp;      
    sh = 0;    
    eh = oresol.h;
  }
  printf("ch %d required height %d\n", ch, oresol.h);
  printf("Output effect resolution %dx%d, start w,h=%d,%d\n", ew-sw, eh-sh, sw, sh);

  for(h=sh;h<eh;h++)
  {    
    for(w=sw;w<ew;w++)
    {
      int iidx, oidx = h*oresol.w+w;
      
      iw = RSCALE(w-sw);
      
      iidx=iresol.w*RSCALE(h-sh)+iw;      
      orgbbuf[oidx*3] = irgbbuf[iidx*3];
      orgbbuf[oidx*3+1] = irgbbuf[iidx*3+1];
      orgbbuf[oidx*3+2] = irgbbuf[iidx*3+2];        
    }
    SGKS_PRINTF("ih %d iw %d\n", iresol.w*RSCALE(h-sh)/iresol.w, iw);
  } 
  
  if(ch > oresol.h || revert) {
    printf("need revert!\n");
    for(h=0;h<oresol.h;h++)
      for(w=0;w<oresol.w;w++)
      {      
        int oidx = h*oresol.w+w;      
        int _oidx = (oresol.w-w-1)*oresol.h+h;      
        dumpbuf[_oidx*3] = orgbbuf[oidx*3];
        dumpbuf[_oidx*3+1] = orgbbuf[oidx*3+1];
        dumpbuf[_oidx*3+2] = orgbbuf[oidx*3+2]; 
      }
  }
  
  if(revert)
    memcpy(orgbbuf, dumpbuf, oresol.w*oresol.h*3);
  if(ch > oresol.h)
    hexdump(dumpbuf, oresol.w*oresol.h);
  else
    hexdump(orgbbuf, oresol.w*oresol.h);
  free(dumpbuf);
  return 0;
}

int main(int argc, char *argv[])
{
	FILE *bmpfile = NULL, *yuvfile = NULL;
	unsigned char* _rgbbuf = NULL, *rgbbuf = NULL;
	unsigned char* yuvbuf = NULL;
	struct resolution iresol, oresol;
	unsigned int temp;
	enum YUV_FMT dst_fmt = YUV_FMT_NV16;
	int i;
	
	if(argc < 3)
		printf("Invalid argument!\n" "%s", USAGE());			
	if((bmpfile = fopen(argv[1], "rb+")) == NULL)
	{
		printf("cannot find bmp file\n");
		goto fail_openbmp;
	}
	
	if(argc == 5) {
  	printf("want format %s\n", argv[4]);
  	dst_fmt = !strcmp(argv[4], "nv16")?YUV_FMT_NV16:(!strcmp(argv[4], "uyv565")?YUV_FMT_UYV565 \
  	  :(!strcmp(argv[4], "ayuv4444")?YUV_FMT_AYUV4444:0));
  }
	fseek(bmpfile, 18L, SEEK_SET);
	fread(&temp, sizeof(unsigned int), 1, bmpfile);
	iresol.w = temp;
	
	fseek(bmpfile, 22L, SEEK_SET);
	fread(&temp, sizeof(unsigned int), 1, bmpfile);
	iresol.h = temp;
	
	printf("width = %d , height = %d\n", iresol.w, iresol.h);
	
	temp = iresol.w*iresol.h*3;
	_rgbbuf = (unsigned char*) malloc(temp);printf("rgb size %d\n", temp);	
	if (_rgbbuf == NULL)
	{
		printf("no enought memory\n");
		goto fail_allocmem;
	}
	printf("%s %d\n", __func__, __LINE__);
	fseek(bmpfile, 54L, SEEK_SET);
	fread (_rgbbuf, 1, iresol.w * iresol.h * 3, bmpfile);	
	printf("%s %d\n", __func__, __LINE__);
	if(argc >= 4) {
	  char *hresol = strchr(argv[3], 'x');
	  if(hresol == NULL) {
	    printf("Invalid resolution format!Use like: 800x480\n");
	    return -1;
	  }
	  *hresol = 0;
	  oresol.w = atoi(argv[3]);
	  oresol.h = atoi(hresol+1);
	  printf("Input dedicated resolution: %dx%d\n", oresol.w, oresol.h);
	  temp = oresol.w*oresol.h*3;
	  rgbbuf = (unsigned char*) malloc(temp);printf("rgb size %d\n", temp);
	  if (rgbbuf == NULL)
  	{
  		printf("no enought memory\n");
  		free(_rgbbuf);
  		goto fail_allocmem2;
  	}
  	memset(rgbbuf, 0, temp);
	  change2resolution(_rgbbuf, rgbbuf, iresol, oresol);
	  free(_rgbbuf);
	} else {
	  printf("No change, use default resolution\n");
	  oresol = iresol;
	  rgbbuf = _rgbbuf;
	}
	temp = temp*2/3; printf("yuv size %d\n", temp);
	yuvbuf = (unsigned char*) malloc(temp);printf("%s %d\n", __func__, __LINE__);
	if (yuvbuf == NULL)
	{
		printf("no enought memory\n");
		goto fail_allocmem3;
	}
	rgb2yuv(dst_fmt, oresol.w, oresol.h, rgbbuf, yuvbuf);printf("%s %d\n", __func__, __LINE__);
	if((yuvfile = fopen(argv[2], "wb")) == NULL)
	{
		printf("cannot open yuv file\n");
		goto fail_openyuv;
	}	printf("%s %d\n", __func__, __LINE__);
	fwrite(yuvbuf, temp, 1, yuvfile);	
	fclose(yuvfile);
	fclose(bmpfile);printf("%s %d\n", __func__, __LINE__);
	
	free(yuvbuf);printf("%s %d\n", __func__, __LINE__);
	free(rgbbuf);	printf("%s %d\n", __func__, __LINE__);
	return 0;
	
fail_openyuv:
  free(yuvbuf);
fail_allocmem3:
  free(rgbbuf);
fail_allocmem2:		
fail_allocmem:
	fclose(bmpfile);	
fail_openbmp:
	exit(1);	
}
#else

#endif