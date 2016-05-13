#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "audio.h"

#define DEFAULT_STREAM_FILE       "record_pcm.dat"
#define DEFAULT_GAIN_LEVEL             8
#define DEFAULT_VOL_LEVEL             8

static void audio_playback(char *name)
{
    //const char *name = file_name;
    int fd = -1;
    int loopPB = 0;
    unsigned int  count  = 0;
    GADI_AUDIO_AioFrameT audio_frame;
    GADI_AUDIO_AioAttrT aio_attr;
    GADI_BOOL flag = GADI_FALSE;
    int retVal = 0;

    /* config aio dev attr */
	aio_attr.bitWidth = GADI_AUDIO_BIT_WIDTH_16;
	aio_attr.soundMode = GADI_AUDIO_SOUND_MODE_MONO;
	aio_attr.sampleRate = GADI_AUDIO_SAMPLE_RATE_44100;

    if ((fd = open(name, O_RDONLY, 0)) == -1)
    {
        printf("open %s failed.\n",name);
        return;
    }

    gadi_audio_set_sample_format(GADI_AUDIO_SAMPLE_FORMAT_RAW_PCM);
	GADI_INFO("gadi_audio_set_sample_format ok\n");

    retVal = gadi_audio_ao_set_attr(&aio_attr);
    if (retVal){
        printf("Set ao attribute failed \n");
        goto exit;
    }
	GADI_INFO("gadi_audio_ao_set_attr ok\n");

    /* enable AO*/
    if (gadi_audio_ao_enable()){
        printf("Enable ao device failed \n");
        goto exit;
    }
	GADI_INFO("gadi_audio_ao_enable ok\n");

    audio_frame.len = gadi_audio_get_frame_size();
    audio_frame.virAddr = (unsigned char*)gadi_sys_malloc(audio_frame.len);
    printf("audio_playback: frame byte=%d\n", audio_frame.len);
	GADI_INFO("gadi_audio_get_frame_size ok\n");
    /* playback */
    while (1)
    {
        if(!retVal) {
            count = read(fd, audio_frame.virAddr, audio_frame.len);
        }
        if (count == 0) {
            if (loopPB){
                lseek(fd, 0, SEEK_SET);
            } else {
                break;
            }
        }
        if(count == audio_frame.len) {
            retVal = gadi_audio_ao_send_frame(&audio_frame,flag);
        }

        //gadi_sys_thread_sleep(1);
    }

    free(audio_frame.virAddr);
    gadi_audio_ao_disable();
	GADI_INFO("gadi_audio_ao_disable ok\n");
exit:
    close(fd);
}

int audio_init(void)
{
	int retVal;

	retVal = gadi_audio_init();
    if(retVal != GADI_OK){
        GADI_ERROR("gadi_audio_init fail\n");
    } else {
        GADI_INFO("gadi_audio_init ok\n");
    }

	return retVal;
}

int set_audio_gain(int gain)
{
    int ret;
    GADI_AUDIO_GainLevelEnumT gainLevel;
    gainLevel = 0x2f + gain;
    ret = gadi_audio_ai_set_gain(&gainLevel);
    if(ret != GADI_OK){
        GADI_ERROR("gadi_audio_ai_set_gain fail\n");
    }else{
    
        GADI_INFO("gadi_audio_ai_set_gain ok\n");
        GADI_INFO("gain level:%d Lv\n", gainLevel - 0x2f);
    }
    
    return ret;
}

int set_audio_vol(int vol)
{
    int ret;
    GADI_AUDIO_VolumeLevelEnumT volumeLevel;
    volumeLevel = 0x2f + vol;
    ret = gadi_audio_ao_set_volume(&volumeLevel);
    if(ret != GADI_OK){
        GADI_ERROR("gadi_audio_ao_set_volume fail\n");
    }else{
    
        GADI_INFO("gadi_audio_ao_set_volume ok\n");
        GADI_INFO("volume level:%d Lv\n", volumeLevel - 0x2f);
    }

    return ret;
}

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("Usage: audioPlayer [FILE]\n");
        return -1;
    }
    
    int ret = -1;
    char file_name[256] = {0};

    strcpy(file_name, argv[1]);
    ret = audio_init();

    set_audio_vol(DEFAULT_VOL_LEVEL);
    set_audio_gain(DEFAULT_GAIN_LEVEL);
    if(ret == GADI_OK){
        audio_playback(file_name);
    }
    
    return 0;
}

