/**	 Edited By haishao_deng 2016-05, According to adidemo audio.c	**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "audio.h"

#define DEFAULT_GAIN_LEVEL             8
#define DEFAULT_VOL_LEVEL             10

#define GPIO_10     10
#define LED_SET_LOW     0
#define LED_SET_HIGH    1


#define FORMAT_PCM  1
#define FORMAT_IEEE_FLOAT  3
#define FORMAT_A_LAW_8BIT  6
#define FORMAT_MU_LAW_8BIT 7

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

void audio_record(char *file_name)
{
    
    const char *name = file_name;
    int fd = -1;
    GADI_AUDIO_AioFrameT audio_frame;
    GADI_AUDIO_AioAttrT aio_attr;
    GADI_BOOL flag = GADI_FALSE;
    int retVal;

    /* config aio dev attr */
    aio_attr.bitWidth = GADI_AUDIO_BIT_WIDTH_16;
    aio_attr.soundMode = GADI_AUDIO_SOUND_MODE_MONO;
    aio_attr.sampleRate = GADI_AUDIO_SAMPLE_RATE_32000; //GADI_AUDIO_SAMPLE_RATE_8000;


    /* open a new file to write */
    remove(name);
    if ((fd = open(name, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO)) == -1)
    {
        printf("open %s failed.\n", name);
        return;
    }

    gadi_audio_set_sample_format(GADI_AUDIO_SAMPLE_FORMAT_RAW_PCM);
    GADI_INFO("gadi_audio_set_sample_format ok\n");

    /* enable AI*/
    retVal = gadi_audio_ai_set_attr(&aio_attr);
    if (retVal){
        printf("Set ai attribute failed \n");
        goto exit1;
    }
    GADI_INFO("gadi_audio_ai_set_attr ok\n");
    if (gadi_audio_ai_enable()){
        printf("Enable ai device failed \n");
        goto exit1;
    }
    GADI_INFO("gadi_audio_ai_enable ok\n");

    audio_frame.len = gadi_audio_get_frame_size();
    printf("audio_record: period byte=%d\n", audio_frame.len);
    GADI_INFO("gadi_audio_get_frame_size ok\n");

    /* capture */
    while (1)
    {
        if(gadi_audio_ai_get_frame(&audio_frame, flag) == 0)
        {
            if (write(fd, audio_frame.virAddr, audio_frame.len) != audio_frame.len)
            {
                printf("audio_record: write file error, len=%d\n",audio_frame.len);
                goto exit2;
            }
        }
        else{
            //printf("get frame failed\n");
        }
        //gadi_sys_thread_sleep(1);
    }

exit2:
    gadi_audio_ai_disable();
    GADI_INFO("gadi_audio_ai_disable ok\n");
exit1:
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("Usage: audioRecorder [FILE]\n");
        return -1;
    }
    
    int ret = -1;
    char file_name[256] = {0};

    strcpy(file_name, argv[1]);
    ret = audio_init();

    if(ret == GADI_OK){
        audio_record(file_name);
    }
    gadi_audio_exit();
    return 0;
}

