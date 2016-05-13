#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include "audio.h"

#define DEFAULT_STREAM_FILE       "record_pcm.dat"

static void audio_playback(char *name)
{
    //const char *name = file_name;
    int fd = -1;
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
            lseek(fd, 0, SEEK_SET);
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

int main(int argc, char *argv[])
{
    if (argc < 2){
        printf("Usage: audioPlayer [FILE]\n");
        return -1;
    }
    
    int ret = -1;
    char file_name[256] = {0};

    //strncpy(file_name, argc[1], sizeof(file_name));
    strcpy(file_name, DEFAULT_STREAM_FILE);
    ret = audio_init();
    if(ret == GADI_OK){
        audio_playback(file_name);
    }
    
    return 0;
}

