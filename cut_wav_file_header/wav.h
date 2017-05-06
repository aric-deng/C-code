#include "basetypes.h"

typedef struct WaveHeader 
{ 
    u8 riff[4];             //资源交换文件标志 
    u32 size;               //从下个地址开始到文件结尾的字节数 
    u8 wave_flag[4];        //wave文件标识 
    u8 fmt[4];              //波形格式标识 
    u32 fmt_len;            //过滤字节(一般为00000010H) 
    u16 tag;                //格式种类，值为1时，表示PCM线性编码 ; 6: A-LAW; 7: MU-LAW
    u16 channels;           //通道数，单声道为1，双声道为2 
    u32 samp_freq;          //采样频率 
    u32 byte_rate;          //数据传输率 (每秒字节＝采样频率×每个样本字节数) 
    u16 block_align;        //块对齐字节数 = (Sample Rate * BitsPerSample * Channels) / 8.
    u16 bit_samp;           //bits per sample (又称量化位数) 
} wave_header_t; 

typedef struct WaveStruct 
{ 
    FILE *fp;                  //file pointer 
    wave_header_t header;      //header 
    u8 data_flag[4];        //数据标识符 
    u32 length;             //采样数据总数 
    u32 *pData;             //data 
} wave_t; 

typedef struct WaveDataStruct
{
	u8 dat_str[4];
	u32 dat_len;	
} wav_data_t;
