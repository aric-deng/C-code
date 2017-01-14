#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "wav.h"

void putchars(char *addr, int cnt)
{
	int i = 0;
	for(; i<cnt; i++){
		putchar(addr[i]);
	}
	printf("\n");
	
}

static int parse_wav_file(char *name)
{
	int fd = -1, i = 0;
	wave_header_t wh = {0};
//	wave_header_t * pwh;
	u8 buf[64] = {0};

	fd = open(name, O_RDONLY, 0);
	if(fd < 0){
		printf("open file %s error! \n", name);
		return -1;
	}
	printf("open file %s success ! \n", name);
	read(fd, buf, sizeof(wave_header_t));
//	read(fd, pwh, sizeof(wave_header_t));
	//pwh = buf;
//	strncpy(&wh, buf, sizeof(wh));
#if 0
	strncpy(&wh.riff, buf, sizeof(wh.riff));
	i += sizeof(wh.riff);
	
//	strncpy(&wh.size, buf+4, sizeof(wh.size));
	wh.size = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;
	
	i += 4;
	strncpy(&wh.wave_flag, &buf[i], sizeof(wh.wave_flag));
	
	i += sizeof(wh.wave_flag);
	strncpy(&wh.fmt, &buf[i], sizeof(wh.fmt));
	
	i += sizeof(wh.fmt);
	wh.fmt_len = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;
	
	i += 4;
	strncpy(&wh.tag, &buf[i], sizeof(wh.tag));
	
	i += sizeof(wh.tag);
	wh.channels = buf[i] | buf[i+1]<<8;
	
	i += 2;
	wh.samp_freq = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;
	i += 4;
	wh.byte_rate = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;
	i += 4;
	wh.block_align = buf[i] | buf[i+1]<<8 ;
	i += 2;
	wh.bit_samp = buf[i] | buf[i+1]<<8 ;
#endif

    strncpy(&wh.riff, buf, sizeof(wh.riff));
    i += sizeof(wh.riff);

    wh.size = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;

    i += sizeof(wh.size);	
    strncpy(&wh.wave_flag, &buf[i], sizeof(wh.wave_flag));

    i += sizeof(wh.wave_flag);
    strncpy(&wh.fmt, &buf[i], sizeof(wh.fmt));

    i += sizeof(wh.fmt);
    wh.fmt_len = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;

    i += sizeof(wh.fmt_len);
    strncpy(&wh.tag, &buf[i], sizeof(wh.tag));

    i += sizeof(wh.tag);
    wh.channels = buf[i] | buf[i+1]<<8;

    i += sizeof(wh.channels);
    wh.samp_freq = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;

    i += sizeof(wh.samp_freq);
    wh.byte_rate = buf[i] | buf[i+1]<<8 | buf[i+2]<<16 | buf[i+3]<<24;

    i += sizeof(wh.byte_rate);
    wh.block_align = buf[i] | buf[i+1]<<8 ;

    i += sizeof(wh.block_align);
    wh.bit_samp = buf[i] | buf[i+1]<<8 ;
	
	//print parse info
	printf("riff string: ");
	putchars(&wh.riff, sizeof(wh.riff));
	printf("size:%d \n", wh.size);
	printf("wave_flag: ");
	putchars(&wh.wave_flag, sizeof(wh.wave_flag));
	printf("wave fmt: ");
	putchars(&wh.fmt, sizeof(wh.fmt));
	printf("fmt_len:%d \n", wh.fmt_len);
	printf("wave tag:%d \n", wh.tag);
	//putchars(&wh.tag, sizeof(wh.tag));
	printf("channels:%d \n", wh.channels);
	printf("samp_freq:%d \n", wh.samp_freq);
	printf("byte_rate:%d, bit_rate:%d \n", wh.byte_rate, wh.byte_rate*8);
	printf("block_align:%d \n", wh.block_align);
	printf("bit_samp:%d \n", wh.bit_samp);
	
	return 0;
}


int main(int argc, char *argv[])
{
    if (argc < 2){
		printf("Usage: audioPlayer [FILE]\n");
		return -1;
	}
	    
	char file_name[256] = {0};
	
	strcpy(file_name, argv[1]);

	parse_wav_file(file_name);

	return 0;
}
