// GKMKVRepairTool.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <string.h>

typedef unsigned char uint8;
typedef unsigned int  uint32;

uint8 EBML_HEAD_ID[4]= {0x1A,0x45,0xDF,0xA3};
uint8 EBML_VERSION_ID[4]= {0X42,0X86,0X81,0X01};
uint8 EBML_READ_VERSION_ID[4]= {0X42,0XF7,0X81,0X01};
uint8 EBML_MAXIDLENGTH_ID[4]= {0X42,0XF2,0X81,0X04};
uint8 EBML_MAXSIZELENGTH_ID[4]= {0X42,0XF3,0X81,0X08};
uint8 DOCTYPES_ID[11]= {0X42,0X82,0X88,'m','a','t','r','o','s','k','a'};
uint8 DOCTYPEVERSION_ID[4]= {0X42,0X87,0X81,0X04};
uint8 DOCTYPEREADVERSION_ID[4]= {0X42,0X85,0X81,0X02};
uint8 SEGMENT_ID[4]= {0X18,0X53,0X80,0X67};
uint8 SEGMENT_SIZE[8]= {0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8 TOTAL_DURATION_ID[7]= {0X44,0X89,0X84,0X00,0X00,0X00,0X00};
uint8 CLUSTER_ID[4]= {0X1F,0X43,0XB6,0X75};
uint8 VIDEO_CODECID_ID[17] = { 0X86, 0X8F, 'V', '_', 'M', 'P', 'E', 'G', '4', '/', 'I', 'S', 'O', '/', 'A', 'V', 'C' };
uint8 MJPEG_VIDEO_CODECID_ID[9] = { 0X86, 0X87, 'V', '_', 'M', 'J', 'P', 'E', 'G'};
uint8 CLUSTER_TIMECODE_ID[1] = { 0XE7 };

const int HEADER_SIZE = 40;
const int H264_TRACK_SIZE = 371;
const int MJPEG_TRACK_SIZE = 906;
const int CLUSTER_HEAD_SIZE = 14;
unsigned char HEADER_BUF[HEADER_SIZE] = {0};
unsigned char TRACK_BUF[H264_TRACK_SIZE] = {0};
long long g_lFileLen = 0;
long long g_lCurIndex = 0;
unsigned int g_nReadFileTime = 0;
unsigned int g_nResultFileTime = 0;
int g_nVideoType = 0;
int g_nTrackSize = 371;
FILE *g_fp = NULL;

#define CLUSTER_VINT_DATA_LEN (4)


int openFile(char *pFileName, long long &lFileLen)
{
	if(pFileName == NULL)
	{
		return -1;
	}
	g_fp = fopen(pFileName,"rb+");
	if(g_fp == NULL)
	{
		printf("open file filed.\n");
		return -1;
	}

	fseek(g_fp,0L,SEEK_END);
	lFileLen = ftell(g_fp);
	printf("file len is %lld\n",lFileLen);
	fseek(g_fp,0L,SEEK_SET);
	return 0;
}

int buildHeaderBuf(void)
{
    unsigned char *p;
    p = (unsigned char*)HEADER_BUF;
    memcpy(p,EBML_HEAD_ID,sizeof(EBML_HEAD_ID));
    p = p + sizeof(EBML_HEAD_ID);

	p[0] = 0xA3;
	p = p + 1;
    //memcpy(p,head_element_size.c,head_element_size.size);
    //p = p + head_element_size.size;

    memcpy(p,EBML_VERSION_ID,sizeof(EBML_VERSION_ID));
    p = p + sizeof(EBML_VERSION_ID);

    memcpy(p,EBML_READ_VERSION_ID,sizeof(EBML_READ_VERSION_ID));
    p = p + sizeof(EBML_READ_VERSION_ID);

    memcpy(p,EBML_MAXIDLENGTH_ID,sizeof(EBML_MAXIDLENGTH_ID));
    p = p + sizeof(EBML_MAXIDLENGTH_ID);

    memcpy(p,EBML_MAXSIZELENGTH_ID,sizeof(EBML_MAXSIZELENGTH_ID));
    p = p + sizeof(EBML_MAXSIZELENGTH_ID);

    memcpy(p,DOCTYPES_ID,sizeof(DOCTYPES_ID));
    p = p + sizeof(DOCTYPES_ID);

    memcpy(p,DOCTYPEVERSION_ID,sizeof(DOCTYPEVERSION_ID));
    p = p + sizeof(DOCTYPEVERSION_ID);


    memcpy(p,DOCTYPEREADVERSION_ID,sizeof(DOCTYPEREADVERSION_ID));
    p = p + sizeof(DOCTYPEREADVERSION_ID);

    return 0;
}


int checkMKVHeader()
{
	int ret = 0;
	unsigned char szHeaderBuf[HEADER_SIZE];
	memset(szHeaderBuf,0,sizeof(szHeaderBuf));
	ret = fread(szHeaderBuf,1,HEADER_SIZE,g_fp);
	if(ret != HEADER_SIZE)
	{
		printf("read mkv header wrong.ret=%d.\n",ret);
		return -1;
	}
	buildHeaderBuf();
	ret = memcmp(szHeaderBuf, HEADER_BUF, HEADER_SIZE);
	if(ret != 0)
	{
		printf("mkv header is wrong.\n");
		return -1;
	}
	printf("mkv header is ok.\n");
	return 0;
}

void set_segment_size(long long t)
{
    SEGMENT_SIZE[1]=(unsigned char)((t>>48) & 0xff);
    SEGMENT_SIZE[2]=(unsigned char)((t>>40) & 0xff);
    SEGMENT_SIZE[3]=(unsigned char)((t>>32) & 0xff);
    SEGMENT_SIZE[4]=(unsigned char)((t>>24) & 0xff);
    SEGMENT_SIZE[5]=(unsigned char)((t>>16) & 0xff);
    SEGMENT_SIZE[6]=(unsigned char)((t>>8) & 0xff);
    SEGMENT_SIZE[7]=(unsigned char)((t>>0) & 0xff);
}

int checkFileSize()
{
	int ret = 0;
	ret = fread(TRACK_BUF,1,H264_TRACK_SIZE,g_fp);
	if(ret != H264_TRACK_SIZE)
	{
		printf("read mkv header wrong.ret=%d.\n",ret);
		return -1;
	}
	unsigned char *p = (unsigned char *)TRACK_BUF;
    if (memcmp(SEGMENT_ID, p, sizeof(SEGMENT_ID))   !=  0 )
    {
        printf("head of track buffer is wrong.\n");
        return -1;
    }

    p += sizeof(SEGMENT_ID);
	long long  s=0,t=0;
    unsigned char  *q=(unsigned char*)&t;
    memcpy(&s, p+1, sizeof(SEGMENT_SIZE) - 1); // skip idsize

    q[0] = (  unsigned char)((s>>48) & 0xff);
    q[1] = (  unsigned char)((s>>40) & 0xff);
    q[2] = (  unsigned char)((s>>32) & 0xff);
    q[3] = (  unsigned char)((s>>24) & 0xff);
    q[4] = (  unsigned char)((s>>16) & 0xff);
    q[5] = (  unsigned char)((s>>8) & 0xff);
    q[6] = (  unsigned char)((s>>0) & 0xff);

	long long total_len = t;
	printf("read the total len is %lld.\n",total_len);
	if(g_lFileLen - total_len > 1024)
	{
		set_segment_size(g_lFileLen);

		fseek(g_fp,HEADER_SIZE+sizeof(SEGMENT_ID),SEEK_SET);
		fwrite(SEGMENT_SIZE,1,sizeof(SEGMENT_SIZE),g_fp);

		printf("modify the total len.\n");
	}

	return 0;
}

void set_total_druation(float ms)
{
    uint8 *p = TOTAL_DURATION_ID+3;
    uint32 val= *(uint32*)(&ms);
    p[0]=(uint8)(val>>24);
    p[1]=(uint8)(val>>16);
    p[2]=(uint8)(val>>8);
    p[3]=(uint8)(val);

}

int checkFileTime()
{
	unsigned char *p = (unsigned char *)TRACK_BUF;
	p += 128;
	if (memcmp(TOTAL_DURATION_ID, p, sizeof(TOTAL_DURATION_ID) -4 )   !=  0 )
    {
        printf("get file time ,id error!\n");
        return -1;
    }

	int  s = 0;
    float  t = 0;
    unsigned   char  *q = NULL;
	memcpy(&s, p+3, sizeof(TOTAL_DURATION_ID)-4 );
    q = (unsigned char *)&t;
	q[0] =(  unsigned char)((s>>24) & 0xff);
    q[1] = (  unsigned char)((s>>16) & 0xff);
    q[2] = (  unsigned char)((s>>8) & 0xff);
    q[3] = (  unsigned char)((s>>0) & 0xff);

	g_nReadFileTime = t;
	printf("read the total time is %u.\n",g_nReadFileTime);
	if(g_nReadFileTime <= 0)
	{
		set_total_druation(10000); //临时改为10s
		fseek(g_fp,HEADER_SIZE+128,SEEK_SET);
		fwrite(TOTAL_DURATION_ID,1,sizeof(TOTAL_DURATION_ID),g_fp);
		printf("modify the total time.\n");
	}

	return 0;
}
//-----------------------------
int GetVideoEncodeType(int &nVideoType)//0:H264;1:MJPEG
{
	unsigned char *p = (unsigned char *)TRACK_BUF;
	p += 198;

	uint8 video_codecid[20] = { 0 };
	memcpy(video_codecid, p, sizeof(VIDEO_CODECID_ID));

	if(memcmp(video_codecid, VIDEO_CODECID_ID, sizeof(VIDEO_CODECID_ID)) == 0)
	{
		printf("video type is H264.\n");
		nVideoType = 0;
		g_nTrackSize = H264_TRACK_SIZE;
		return 0;
	}
	else
	{
		memset(video_codecid,0,sizeof(VIDEO_CODECID_ID));
		memcpy(video_codecid,p,sizeof(MJPEG_VIDEO_CODECID_ID));
		if(memcmp(video_codecid, MJPEG_VIDEO_CODECID_ID, sizeof(MJPEG_VIDEO_CODECID_ID)) == 0)
		{
			printf("video type is MJPEG.\n");
		    nVideoType = 1;
			g_nTrackSize = MJPEG_TRACK_SIZE;
		    return 0;
		}
	}
	printf("get video type error\n");
	return -1;
}

int SearchCluster(long long &lClusterIndex)
{
	int ret = 0;
	int i = 0;
	unsigned char *p = NULL;
	const int nOffset = 3;
	unsigned char szClusterHead[CLUSTER_HEAD_SIZE+nOffset];
	memset(szClusterHead,0,sizeof(szClusterHead));

	while((g_lCurIndex + CLUSTER_HEAD_SIZE + nOffset) < g_lFileLen)
	{
		fseek(g_fp,g_lCurIndex,SEEK_SET);
		ret = fread(szClusterHead,1,CLUSTER_HEAD_SIZE+nOffset,g_fp);
		g_lCurIndex += CLUSTER_HEAD_SIZE;
		if(ret != (CLUSTER_HEAD_SIZE+nOffset))
		{
			printf("search cluser head size wrong.ret=%d.\n",ret);
			return -1;
		}
		p = (unsigned char*)szClusterHead;
		for(i = 0; i < CLUSTER_HEAD_SIZE; i++)
		{
			if (memcmp(CLUSTER_ID, p+i, sizeof(CLUSTER_ID)) == 0)
			{
				printf("find cluser head.index = %lld\n",g_lCurIndex+i-CLUSTER_HEAD_SIZE);
				lClusterIndex = g_lCurIndex+i-CLUSTER_HEAD_SIZE;
				return 0;
			}
		}
	}
	return -1;
}

int CheckClusterInfo()
{
	int ret = 0;
	unsigned int s = 0, t = 0;
	unsigned char szClusterHead[CLUSTER_HEAD_SIZE];
	memset(szClusterHead,0,sizeof(szClusterHead));
	long long lClusterInde = 0;
	unsigned char *p = NULL;
	unsigned char *q = NULL;
	unsigned int nClusterLen = 0;
	unsigned long lTimes = 0;
	while(1)
	{
		memset(szClusterHead,0,sizeof(szClusterHead));
		fseek(g_fp,g_lCurIndex,SEEK_SET);
		ret = fread(szClusterHead,1,CLUSTER_HEAD_SIZE,g_fp);
		if(ret != CLUSTER_HEAD_SIZE)
		{
			printf("read cluser head size wrong.ret=%d.\n",ret);
			return -1;
		}
		p = (unsigned char*)szClusterHead;

		if (memcmp(CLUSTER_ID, p, sizeof(CLUSTER_ID)) != 0)
		{
			printf("cluster ID err, %02x %02x %02x %02x\n",p[0],p[1],p[2],p[3]);
			ret = SearchCluster(lClusterInde);
			if(ret == -1)
			{
				return -1;
			} 
			fseek(g_fp,lClusterInde,SEEK_SET);
			g_lCurIndex = lClusterInde;
			continue;
		}
		printf("cluster id index is %lld.\n",g_lCurIndex);
		memcpy(&s, p + 4, CLUSTER_VINT_DATA_LEN);
		q = (unsigned char *) &t;
		q[0] = (unsigned char) ((s >> 24) & 0xff);
		q[1] = (unsigned char) ((s >> 16) & 0xff);
		q[2] = (unsigned char) ((s >> 8) & 0xff);
		q[3] = (unsigned char) ((s >> 0) & 0xff);
		t &= 0x0fffffff;
		nClusterLen = (t - 5 - sizeof(CLUSTER_TIMECODE_ID));
		printf("cluster len is %d\n",nClusterLen);
		g_lCurIndex = g_lCurIndex + CLUSTER_HEAD_SIZE + nClusterLen;

		s = 0;
		memcpy(&s, p + 10, CLUSTER_VINT_DATA_LEN);
		q = (unsigned char *) &t;
		q[0] = (unsigned char) ((s >> 24) & 0xff);
		q[1] = (unsigned char) ((s >> 16) & 0xff);
		q[2] = (unsigned char) ((s >> 8) & 0xff);
		q[3] = (unsigned char) ((s >> 0) & 0xff);
		g_nResultFileTime = t;
		printf("cluster time is %d, times = %ld\n",g_nResultFileTime,lTimes);
		g_nResultFileTime += 1000;
		lTimes++;
	}

	return 0;
}

int main(int argc, char **argv)
{
	int nRet = 0;
	if(argc <= 1)
	{
		return -1;
	}
	char *pFileName = argv[1];
	//char *pFileName = "D:\\linux_2018\\异常MKV\\MKVRepair\\0.mkv";
	nRet = openFile(pFileName,g_lFileLen);
	if(nRet != 0)
	{
		return -1;
	}

	nRet = checkMKVHeader();
	if(nRet != 0)
	{
		return -1;
	}

	nRet = checkFileSize();
	if(nRet != 0)
	{
		return -1;
	}

	nRet = checkFileTime();
	if(nRet != 0)
	{
		return -1;
	}
	//-------------------------
	nRet = GetVideoEncodeType(g_nVideoType);
	if(nRet != 0)
	{
		return -1;
	}
	g_lCurIndex = HEADER_SIZE+g_nTrackSize;
	fseek(g_fp,g_lCurIndex,SEEK_SET);
	CheckClusterInfo();
	if(g_nResultFileTime - g_nReadFileTime >= 1000 )
	{
		set_total_druation(g_nResultFileTime); //临时改为10s
		fseek(g_fp,HEADER_SIZE+128,SEEK_SET);
		fwrite(TOTAL_DURATION_ID,1,sizeof(TOTAL_DURATION_ID),g_fp);
		printf("modify the total time %d.\n", g_nResultFileTime);
	}
	//long long lClusterIndex = 0;
	//SearchCluster(lClusterIndex);

	fclose(g_fp);
	return 0;
}

