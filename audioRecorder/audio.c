/******************************************************************************
** \file        adi/test/src/audio.c
**
** \brief       ADI layer audio(record/play) test.
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2013-2014 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

#include <getopt.h>

#include "audio.h"
#include "shell.h"


//*****************************************************************************
//*****************************************************************************
//** Local Defines
//*****************************************************************************
//*****************************************************************************
#define DEFAULT_CHANNEL_NUMBER    1
#define DEFAULT_STREAM_FILE       "record_pcm.dat"

#define AUDIO_PLAYBACK_THREAD_PRIORITY  3
#define AUDIO_PLAYBACK_THREAD_STACKSIZE 2048
#define AUDIO_PLAYBACK_THREAD_NAME      "audio_playback"

#define AUDIO_RECORD_THREAD_PRIORITY    4
#define AUDIO_RECORD_THREAD_STACKSIZE   2048
#define AUDIO_RECORD_THREAD_NAME        "audio_record"

#define AUDIO_LOOPBACK_THREAD_PRIORITY    4
#define AUDIO_LOOPBACK_THREAD_STACKSIZE   2048
#define AUDIO_LOOPBACK_THREAD_NAME        "audio_loopback"

//*****************************************************************************
//*****************************************************************************
//** Local structures
//*****************************************************************************
//*****************************************************************************



//*****************************************************************************
//*****************************************************************************
//** Global Data
//*****************************************************************************
//*****************************************************************************


//*****************************************************************************
//*****************************************************************************
//** Local Data
//*****************************************************************************
//*****************************************************************************
static const char *shortOptions = "hIEPRf:LS:l:g:v:";
static struct option longOptions[] =
{
    {"help",     0, 0, 'h'},
    {"init",     0, 0, 'I'},
    {"exit",     0, 0, 'E'},
    {"playback", 0, 0, 'P'},
    {"record",   0, 0, 'R'},
    {"format",   1, 0, 'f'},
    {"loopback", 0, 0, 'L'},
    {"stop",     1, 0, 'S'},
    {"look",     1, 0, 'l'},
    {"gain",     1, 0, 'g'},
    {"volume",   1, 0, 'v'},
    {0,          0, 0, 0}
};

char file_name[256];


static GADI_SYS_ThreadHandleT    audioPlaybackThreadHandle = 0;
static GADI_SYS_ThreadHandleT    audioRecordThreadHandle   = 0;
static GADI_SYS_ThreadHandleT    audioLoopbackThreadHandle   = 0;


static int audioPlaybackRunning = 0;
static int audioRecordRunning   = 0;
static int audioLoopbackRunning  = 0;


//*****************************************************************************
//*****************************************************************************
//** Local Functions Declaration
//*****************************************************************************
//*****************************************************************************
static void usage(void);
static GADI_ERR handle_audio_command(int argc, char* argv[]);
static void audio_playback_thread(void *optDataPtr);
static void audio_record_thread(void *optDataPtr);
static void audio_loopback_thread(void *optDataPtr);


//*****************************************************************************
//*****************************************************************************
//** API Functions
//*****************************************************************************
//*****************************************************************************
int audio_register_testcase(void)
{
    int   retVal = 0;
    (void)shell_registercommand (
        "audio",
        handle_audio_command,
        "audio command",
        "---------------------------------------------------------------------\n"
        "audio -R file_name\n"
        "   brief : start record audio stream.\n"
        "\n"
        "---------------------------------------------------------------------\n"
        "audio -L\n"
        "   brief : Loopback(ai to ao)  audio stream.\n"
        "\n"
        "---------------------------------------------------------------------\n"
        "audio -P file_name\n"
        "   brief : start play audio stream.\n"
        "\n"
        "---------------------------------------------------------------------\n"
        "audio -S\n"
        "   brief : stop record audio stream.\n"
        "\n"
    );

    return retVal;
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

int audio_exit(void)
{
	int retVal;

	retVal = gadi_audio_exit();
     if(retVal != GADI_OK){
        GADI_ERROR("gadi_audio_exit fail\n");
    } else {
        GADI_INFO("gadi_audio_exit ok\n");
    }

	return retVal;
}

static void audio_print_ai(void)
{
    GADI_AUDIO_AioAttrT  ptrAttr;
    GADI_AUDIO_GainLevelEnumT gain;
    GADI_ERR retVal;
    retVal = gadi_audio_ai_get_attr(&ptrAttr);
    if(retVal != GADI_OK){
        GADI_ERROR("gadi_audio_ai_get_attr fail\n");
    }else{
        GADI_INFO("gadi_audio_ai_get_attr ok\n");
        GADI_INFO("sampleRate:%d bitWidth:%d\n"
            "\tsoundMode:%d frameSize:%d\n",
            ptrAttr.sampleRate, ptrAttr.bitWidth, ptrAttr.soundMode, ptrAttr.frameSize);
    }
    retVal = gadi_audio_ai_get_fd();
    GADI_INFO("gadi_audio_ai_get_fd ok\n");
    GADI_INFO("audio input fd:%d \n", retVal);

    retVal = gadi_audio_ai_get_gain(&gain);
    if(retVal != GADI_OK){
        GADI_ERROR("gadi_audio_ai_get_gain fail\n");
    }else{
        GADI_INFO("gadi_audio_ai_get_gain ok\n");
        GADI_INFO("gain level:%d Lv\n", gain - 0x2f);
    }
}

static void audio_print_ao(void)
{
    GADI_AUDIO_AioAttrT  ptrAttr;
    GADI_AUDIO_VolumeLevelEnumT volume;
    GADI_ERR retVal;
    retVal = gadi_audio_ao_get_attr(&ptrAttr);
    if(retVal != GADI_OK){
        GADI_ERROR("gadi_audio_ao_get_attr fail\n");
    }else{
        GADI_INFO("gadi_audio_ao_get_attr ok\n");
        GADI_INFO("sampleRate:%d bitWidth:%d\n"
            "\tsoundMode:%d frameSize:%d\n",
            ptrAttr.sampleRate, ptrAttr.bitWidth, ptrAttr.soundMode, ptrAttr.frameSize);
    }
    retVal = gadi_audio_ao_get_fd();
    GADI_INFO("gadi_audio_ao_get_fd ok\n");
    GADI_INFO("audio output fd:%d \n", retVal);

    retVal = gadi_audio_ao_get_volume(&volume);
    if(retVal != GADI_OK){
        GADI_ERROR("gadi_audio_ao_get_volume fail\n");
    }else{
        GADI_INFO("gadi_audio_ao_get_volume ok\n");
        GADI_INFO("volume level:%d Lv\n", volume - 0x2f);
    }
}




//*****************************************************************************
//*****************************************************************************
//** Local Functions
//*****************************************************************************
//*****************************************************************************
static void usage(void)
{
    printf("\nusage: audio [OPTION]...[FILE]...\n");
    printf("\t-h, --help            help.\n"
           "\t-I, --init            init audio device.\n"
           "\t-E, --exit            exit audio device.\n"
           "\t-P, --playback        playback file.\n"
           "\t-L, --loopback        loopback(ai to ao).\n"
           "\t-R, --record          record file.\n"
           "\t-l, --look=#          print attribute(ai/ao)\n"
           "\t-v, --volume=#        set ao volume level(0~12)\n"
           "\t-g, --gain=#          set ai gain level(0~12)\n");
    printf("\n");
}

static GADI_ERR handle_audio_command(int argc, char* argv[])
{
    int option_index, ch;
    GADI_AUDIO_StreamDirectEnumT stream_direct = GADI_AUDIO_STREAM_LAST;
    int retVal;


    strcpy(file_name, DEFAULT_STREAM_FILE);

    /*change parameters when giving input options.*/
    while ((ch = getopt_long(argc, argv, shortOptions, longOptions, &option_index)) != -1)
    {
        switch (ch)
        {
            case 'h':
                usage();
                goto command_exit;
            case 'I':
                audio_init();
                break;
            case 'E':
                audio_exit();
                break;

            case 'R':
                stream_direct = GADI_AUDIO_STREAM_CAPTURE;
                break;

            case 'P':
                stream_direct = GADI_AUDIO_STREAM_PLAYBACK;
                break;
            case 'l':
                if(strcmp(optarg, "ai") == 0){
                    audio_print_ai();
                } else if(strcmp(optarg, "ao") == 0){
                    audio_print_ao();
                } else {
                    GADI_ERROR("bad params\n");
                }

                break;
            case 'g':
                {
                    GADI_AUDIO_GainLevelEnumT gainLevel;
                    gainLevel = 0x2f + atoi(optarg);
                    retVal = gadi_audio_ai_set_gain(&gainLevel);
                    if(retVal != GADI_OK){
                        GADI_ERROR("gadi_audio_ai_set_gain fail\n");
                    }else{
                        GADI_INFO("gadi_audio_ai_set_gain ok\n");
                        GADI_INFO("gain level:%d Lv\n", gainLevel - 0x2f);
                    }
                }
                break;

            case 'v':
                {
                    GADI_AUDIO_VolumeLevelEnumT volumeLevel;
                    volumeLevel = 0x2f + atoi(optarg);
                    retVal = gadi_audio_ao_set_volume(&volumeLevel);
                    if(retVal != GADI_OK){
                        GADI_ERROR("gadi_audio_ao_set_volume fail\n");
                    }else{
                        GADI_INFO("gadi_audio_ao_set_volume ok\n");
                        GADI_INFO("volume level:%d Lv\n", volumeLevel - 0x2f);
                    }
                }
                break;

            case 'f':
                if(strcmp(optarg, "mu_law") == 0)
                {
                    gadi_audio_set_sample_format(GADI_AUDIO_SAMPLE_FORMAT_MU_LAW);
                }
                else if(strcmp(optarg, "a_law") == 0)
                {
                    gadi_audio_set_sample_format(GADI_AUDIO_SAMPLE_FORMAT_A_LAW);
                }
                else
                {
                    gadi_audio_set_sample_format(GADI_AUDIO_SAMPLE_FORMAT_RAW_PCM);
                }
				GADI_INFO("gadi_audio_set_sample_format ok\n");
                break;

            case 'L':
                stream_direct = GADI_AUDIO_STREAM_LOOPBACK;
                break;

            case 'S':
                if(strcmp(optarg, "play") == 0)
                {
                    if(audioPlaybackThreadHandle != 0)
                    {
                        audioPlaybackRunning = 0;
                        retVal = gadi_sys_wait_end_thread(audioPlaybackThreadHandle);
                        if (retVal != GADI_OK)
                        {
                            printf("audio: destroy playback thread failed %d\n", retVal);
                            goto command_exit;
                        }
                        printf("audio: stop playback successfully!!!\n");
                        audioPlaybackThreadHandle = 0;
                    }
                    goto command_exit;
                }
                else if(strcmp(optarg, "record") == 0)
                {
                    if(audioRecordThreadHandle != 0)
                    {
                        audioRecordRunning = 0;
                        retVal = gadi_sys_wait_end_thread(audioRecordThreadHandle);
                        if (retVal != GADI_OK)
                        {
                            printf("audio: destroy record thread failed %d\n", retVal);
                            goto command_exit;
                        }
                        printf("audio: stop record successfully!!!\n");
                        audioRecordThreadHandle = 0;
                    }
                    goto command_exit;
                }
                else if(strcmp(optarg, "loopback") == 0)
                {
                    if(audioLoopbackThreadHandle != 0)
                    {
                        audioLoopbackRunning = 0;
                        retVal = gadi_sys_wait_end_thread(audioLoopbackThreadHandle);
                        if (retVal != GADI_OK)
                        {
                            printf("audio: destroy record thread failed %d\n", retVal);
                            goto command_exit;
                        }
                        printf("audio: stop loop successfully!!!\n");
                        audioLoopbackThreadHandle = 0;
                    }
                    goto command_exit;
                }
                else
                {
                    printf("please intput what to stop -S (record/play/loopback)?\n");
                    goto command_exit;
                }
                break;

            default:
                printf("type '--help' for more usage.\n");
                goto command_exit;
        }
    }

    if(optind <= argc -1)
        strcpy(file_name, argv[optind]);

    printf("audio test: file name=%s\n",file_name);

    if(stream_direct == GADI_AUDIO_STREAM_PLAYBACK)
    {
        if (audioPlaybackRunning == 0)
        {
            audioPlaybackRunning = 1;
            gadi_sys_thread_create(audio_playback_thread,
                                   NULL,
                                   AUDIO_PLAYBACK_THREAD_PRIORITY,
                                   AUDIO_PLAYBACK_THREAD_STACKSIZE,
                                   AUDIO_PLAYBACK_THREAD_NAME,
                                   &audioPlaybackThreadHandle);
        }
    }
    else if(stream_direct == GADI_AUDIO_STREAM_CAPTURE)
    {
        if(audioRecordRunning == 0)
        {
            audioRecordRunning = 1;
            gadi_sys_thread_create(audio_record_thread,
                                   NULL,
                                   AUDIO_RECORD_THREAD_PRIORITY,
                                   AUDIO_RECORD_THREAD_STACKSIZE,
                                   AUDIO_RECORD_THREAD_NAME,
                                   &audioRecordThreadHandle);
        }

    }
    else if(stream_direct == GADI_AUDIO_STREAM_LOOPBACK)
    {
        if(audioLoopbackRunning == 0)
        {
            audioLoopbackRunning = 1;
            gadi_sys_thread_create(audio_loopback_thread,
                                   NULL,
                                   AUDIO_LOOPBACK_THREAD_PRIORITY,
                                   AUDIO_LOOPBACK_THREAD_STACKSIZE,
                                   AUDIO_LOOPBACK_THREAD_NAME,
                                   &audioLoopbackThreadHandle);
        }
    }
command_exit:
    optind = 1;
    return 1;
}


static void audio_playback_thread(void *optDataPtr)
{
    const char *name = file_name;
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
    while (audioPlaybackRunning)
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

static void audio_record_thread(void *optDataPtr)
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
	aio_attr.sampleRate = GADI_AUDIO_SAMPLE_RATE_8000;


    /* open a new file to write */
    remove(name);
    if ((fd = open(name, O_WRONLY | O_CREAT, 0644)) == -1)
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
    while (audioRecordRunning)
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

static void audio_loopback_thread (void *optDataPtr)
{
    GADI_ERR retVal = GADI_OK;
    GADI_AUDIO_AioAttrT aio_attr;

    /* config aio dev attr */
	aio_attr.bitWidth = GADI_AUDIO_BIT_WIDTH_16;
	aio_attr.soundMode = GADI_AUDIO_SOUND_MODE_MONO;
	aio_attr.sampleRate = GADI_AUDIO_SAMPLE_RATE_8000;

    retVal = gadi_audio_ai_set_attr(&aio_attr);
    if (retVal){
        printf("Set ai attribute failed \n");
        return;
    }

    retVal = gadi_audio_ao_set_attr(&aio_attr);
    if (retVal){
        printf("Set ao attribute failed \n");
        return;
    }

    gadi_audio_ao_bind_ai();
	GADI_INFO("gadi_audio_ao_bind_ai ok\n");
    while (audioLoopbackRunning)
    {
        gadi_sys_thread_sleep(1000);
    }

    gadi_audio_ao_unbind_ai();
	GADI_INFO("gadi_audio_ao_unbind_ai ok\n");

    return;
}

