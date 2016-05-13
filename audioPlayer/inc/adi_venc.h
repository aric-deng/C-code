/*!
*****************************************************************************
** \file        adi/include/adi_venc.h
**
** \brief       ADI video encode module header file
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2014 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ADI_VENC_H_
#define _ADI_VENC_H_

#include "stdio.h"
#include "adi_types.h"

//*****************************************************************************
//*****************************************************************************
//** Defines and Macros
//*****************************************************************************
//*****************************************************************************

/*! The num of chan .. */
#define MAX_STREAM_NUM  4 /*MAX STREAM*/

/*
*******************************************************************************
** Defines for general error codes of the module.
*******************************************************************************
*/
/*! Bad parameter passed. */
#define GADI_VENC_ERR_BAD_PARAMETER                                          \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_BAD_PARAMETER)
/*! Memory allocation failed. */
#define GADI_VENC_ERR_OUT_OF_MEMORY                                          \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_OUT_OF_MEMORY)
/*! Device already initialised. */
#define GADI_VENC_ERR_ALREADY_INITIALIZED                                    \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_ALREADY_INITIALIZED)
/*! Device not initialised. */
#define GADI_VENC_ERR_NOT_INITIALIZED                                        \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_NOT_INITIALIZED)
/*! Feature or function is not available. */
#define GADI_VENC_ERR_FEATURE_NOT_SUPPORTED                                  \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_FEATURE_NOT_SUPPORTED)
/*! Timeout occured. */
#define GADI_VENC_ERR_TIMEOUT                                                \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_TIMEOUT)
/*! The device is busy, try again later. */
#define GADI_VENC_ERR_DEVICE_BUSY                                            \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_DEVICE_BUSY)
/*! Invalid handle was passed. */
#define GADI_VENC_ERR_INVALID_HANDLE                                         \
                              (GADI_VENC_MODULE_BASE + GADI_ERR_INVALID_HANDLE)
/*! Semaphore could not be created. */
#define GADI_VENC_ERR_SEMAPHORE_CREATE                                       \
                               (GADI_VENC_MODULE_BASE + GADI_ERR_SEMAPHORE_CREATE)
/*! The driver's used version is not supported. */
#define GADI_VENC_ERR_UNSUPPORTED_VERSION                                    \
                               (GADI_VENC_MODULE_BASE + GADI_ERR_UNSUPPORTED_VERSION)
/*! The driver's used version is not supported. */
#define GADI_VENC_ERR_FROM_DRIVER                                            \
                               (GADI_VENC_MODULE_BASE + GADI_ERR_FROM_DRIVER)
/*! The device/handle is not open.. */
#define GADI_VENC_ERR_NOT_OPEN                                               \
                               (GADI_VENC_MODULE_BASE + GADI_ERR_NOT_OPEN)
/*! The device/map is err.. */
#define GADI_VENC_ERR_MAP                                               \
                               (GADI_VENC_MODULE_BASE + GADI_ERR_MAP)


//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************
/*!
*******************************************************************************
** \brief video encode output stream enum.
*******************************************************************************
*/
typedef enum {
    GADI_VENC_STREAM_FIRST = 0,
    GADI_VENC_STREAM_SECOND,
    GADI_VENC_STREAM_THIRD,
    GADI_VENC_STREAM_FORTH,
    GADI_VENC_STREAM_NUM,
} GADI_VENC_StreamEnumT;

/*!
*******************************************************************************
** \brief video encode stream state enum.
*******************************************************************************
*/
typedef enum {
    /*uninitialized or unconfigured*/
    GADI_VENC_STREAM_STATE_UNKNOWN = 0,
    /*configured ready, but not started encoding yet*/
    GADI_VENC_STREAM_STATE_READY_FOR_ENCODING = 1,
    /*encoding*/
    GADI_VENC_STREAM_STATE_ENCODING = 2,
    /*transition state: starting to encode*/
    GADI_VENC_STREAM_STATE_STARTING = 3,
    /*transition state: stopping encoding*/
    GADI_VENC_STREAM_STATE_STOPPING = 4,
    /*known error*/
    GADI_VENC_STREAM_STATE_ERROR    = 255,
} GADI_VENC_StreamStateEnumT;


/*!
*******************************************************************************
** \brief video encode channel enum.
*******************************************************************************
*/
typedef enum {
    GADI_VENC_CHANNEL_1 = 0,
    GADI_VENC_CHANNEL_2,
    GADI_VENC_CHANNEL_3,
    GADI_VENC_CHANNEL_4,
    GADI_VENC_CHANNEL_NUM,
} GADI_VENC_ChannelEnumT;

/*!
*******************************************************************************
** \brief video encode channel state enum.
*******************************************************************************
*/
typedef enum {
    /*uninitialized or unconfigured*/
    GADI_VENC_CHANNEL_STATE_UNKNOWN = 0,
    /*configured, but not used by any stream to encode*/
    GADI_VENC_CHANNEL_STATE_IDLE    = 1,
    /*configured and used by at least one stream to encode*/
    GADI_VENC_CHANNEL_STATE_BUSY    = 2,
    /*known error*/
    GADI_VENC_CHANNEL_STATE_ERROR   = 255,
}GADI_VENC_ChannelStateEnumT;


/*!
*******************************************************************************
** \brief video encode channel type for used enum.
*******************************************************************************
*/
typedef enum {
    /*channel disabled*/
	GADI_VENC_CHANNEL_TYPE_OFF = 0,
	/*channel for encoding*/
	GADI_VENC_CHANNEL_TYPE_ENCODE = 1,
	/*channel for preview(CVBS output)*/
	GADI_VENC_CHANNEL_TYPE_PREVIEW = 2,
} GADI_VENC_ChannelTypeEnumT;

/*!
*******************************************************************************
** \brief video encode type enum.
*******************************************************************************
*/
typedef enum {
    GADI_VENC_TYPE_OFF = 0,
    GADI_VENC_TYPE_H264,
    GADI_VENC_TYPE_MJPEG,
} GADI_VENC_TypeEnumT;

/*!
*******************************************************************************
** \brief video encode bit rate control mode enum.
*******************************************************************************
*/
typedef enum {
    GADI_VENC_CBR_MODE = 0,
    GADI_VENC_VBR_MODE,
    GADI_VENC_CBR_QUALITY_MODE,
    GADI_VENC_VBR_QUALITY_MODE,
    GADI_VENC_BRC_MODE_NUM,
}GADI_VENC_BrcModeEnumT;

/*!
*******************************************************************************
** \brief video encode support frame rate.
*******************************************************************************
*/
typedef enum {
    /*use default sensor frame rate.*/
    GADI_VENC_FPS_AUTO   = 0,
    /*fps:1.*/
    GADI_VENC_FPS_1      = 1,
    GADI_VENC_FPS_2      = 2,
    GADI_VENC_FPS_3      = 3,
    GADI_VENC_FPS_4      = 4,
    GADI_VENC_FPS_5      = 5,
    GADI_VENC_FPS_6      = 6,
    GADI_VENC_FPS_10     = 10,
    GADI_VENC_FPS_12     = 12,
    GADI_VENC_FPS_13     = 13,
    GADI_VENC_FPS_14     = 14,
    GADI_VENC_FPS_15     = 15,
    GADI_VENC_FPS_20     = 20,
    GADI_VENC_FPS_24     = 24,
    GADI_VENC_FPS_25     = 25,
    GADI_VENC_FPS_30     = 30,
    GADI_VENC_FPS_50     = 50,
    GADI_VENC_FPS_60     = 60,
    GADI_VENC_FPS_120    = 120,
    /*fps:3.125.*/
    GADI_VENC_FPS_3_125  = 3125000,
    /*fps:3.75.*/
    GADI_VENC_FPS_3_75   = 37500,
    /*fps:6.25.*/
    GADI_VENC_FPS_6_25   = 62500,
    /*fps:7.5.*/
    GADI_VENC_FPS_7_5    = 750,
    /*fps:12.5.*/
    GADI_VENC_FPS_12_5   = 1250,
    /*fps:23.976.*/
    GADI_VENC_FPS_23_976 = 23976000,
    /*fps:29.97.*/
    GADI_VENC_FPS_29_97  = 299700,
    /*fps:59.94.*/
    GADI_VENC_FPS_59_94  = 599400,
}GADI_VENC_FrameRateEnumT;


//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************
/*!
*******************************************************************************
** \brief one channel state struct.
*******************************************************************************
*/
typedef struct
{
    /*channel index.*/
    GADI_U32            channelId;
    /*channel state.*/
    GADI_U32            state;
} GADI_VENC_ChannelStateT;

/*!
*******************************************************************************
** \brief channel parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*first channel(capture buffer) type 0:disable, 1:for encoding, 2:for preview.*/
    GADI_U32            chan1Type;
    /*first channel buffer width.*/
    GADI_U16            chan1Width;
    /*first channel buffer height.*/
    GADI_U16            chan1Height;
    /*deintlc mode of first channel buffer.*/
    GADI_U16            chan1DeintlcForIntlcVin;
    /*second channel type.*/
    GADI_U32            chan2Type;
    /*second channel buffer width.*/
    GADI_U16            chan2Width;
    /*second channel buffer height.*/
    GADI_U16            chan2Height;
    /*deintlc mode of second channel buffer.*/
    GADI_U16            chan2DeintlcForIntlcVin;
    /*third channel type.*/
    GADI_U32            chan3Type;
    /*third channel buffer width.*/
    GADI_U16            chan3Width;
    /*third channel buffer height.*/
    GADI_U16            chan3Height;
    /*deintlc mode of third channel buffer.*/
    GADI_U16            chan3DeintlcForIntlcVin;
    /*fourth channel type.*/
    GADI_U32            chan4Type;
    /*fourth channel buffer width.*/
    GADI_U16            chan4Width;
    /*third channel buffer height.*/
    GADI_U16            chan4Height;
    /*deintlc mode of fourth channel buffer.*/
    GADI_U16            chan4DeintlcForIntlcVin;
    /*0: OFF  1: use progressive VIN to encode interlaced video*/
    GADI_U8             intlcScan;
}GADI_VENC_ChannelsParamsT;

/*!
*******************************************************************************
** \brief one stream state struct.
*******************************************************************************
*/
typedef struct
{
    /*stream index.*/
    GADI_U32            streamId;
    /*stream state.*/
    GADI_U32            state;
} GADI_VENC_StreamStateT;

/*!
*******************************************************************************
** \brief one stream format struct.
*******************************************************************************
*/
typedef struct
{
    /*stream index.*/
    GADI_U32                    streamId;
    /*0: none, 1: H.264, 2: MJPEG*/
    GADI_U8                     encodeType;
    /*channel index:GADI_VENC_ChannelEnumT.*/
    GADI_U8                     channelId;
    /*rotate: 0x01:means flip horizontal,0x02:means flip vertical, 0x04:means rotate 90.*/
    GADI_U8                     flipRotate;
    /*encode width.*/
    GADI_U16                    width;
    /*encode height.*/
    GADI_U16                    height;
    /*encode x offset.*/
    GADI_U16                    xOffset;
    /*encode y offset.*/
    GADI_U16                    yOffset;
    /*encode frame rate.*/
    GADI_VENC_FrameRateEnumT    fps;
    /*encode video keep aspect ratio.*/
    /*vi width scale to video width ratio not equal video height ratio.*/
    /*keep aspect ration means use the small aspect ratio.*/
    /* 1: means keep aspect ration, 0: means do not keep.*/
    GADI_U8                     keepAspRat;
}GADI_VENC_StreamFormatT;

/*!
*******************************************************************************
** \brief one h264 stream configuration struct.
*******************************************************************************
*/
typedef struct
{
    /*stream index.*/
    GADI_U32            streamId;
    /*gop M value.*/
    GADI_U8             gopM;
    /*gop N value.*/
    GADI_U8             gopN;
    /*IDR interval .*/
    GADI_U8             idrInterval;
    /*gop model.*/
    GADI_U8             gopModel;
    /*encode profile.*/
    GADI_U8             profile;
    /*0: CBR; 1: VBR; 2: CBR keep quality; 3: VBR keep quality: GADI_VENC_BrcModeEnumT*/
    GADI_U8             brcMode;
    /*cbr mode, bit rate.*/
    GADI_U32            cbrAvgBps;
    /*vbr mode, min bit rate.*/
    GADI_U32            vbrMinbps;
    /*vbr mode, max bit rate.*/
    GADI_U32            vbrMaxbps;
    /*picure quality 0,3,0: poor, 3: best*/
    GADI_U8             quality;
    /*picure quality consistency ,0: poor ,3 best*/
    GADI_U8             qcon;
}GADI_VENC_H264ConfigT;

/*!
*******************************************************************************
** \brief one MJPEG stream configuration struct.
*******************************************************************************
*/
typedef struct
{
    /*stream index.*/
    GADI_U32            streamId;
    /*0: YUV 422, 1: YUV 420.*/
    GADI_U8             chromaFormat;
    /*1 ~ 100, 100 is best quality.*/
    GADI_U8             quality;
} GADI_VENC_MjpegConfigT;

/*!
*******************************************************************************
** \brief video encode module open parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*video input module handle.*/
    GADI_SYS_HandleT            viHandle;
    /*video output module handle.*/
    GADI_SYS_HandleT            voutHandle;
}GADI_VENC_OpenParamsT;


/*!
*******************************************************************************
** \brief setting video encode bitrate range parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*stream index for setting the bitrate.*/
    GADI_U32            streamId;
    /*0: CBR; 1: VBR; 2: CBR keep quality; 3: VBR keep quality: GADI_VENC_BrcModeEnumT*/
    GADI_U8             brcMode;
    /*cbr mode, bit rate.*/
    GADI_U32            cbrAvgBps;
    /*vbr mode, min bit rate.*/
    GADI_U32            vbrMinbps;
    /*vbr mode, max bit rate.*/
    GADI_U32            vbrMaxbps;
}GADI_VENC_BitRateRangeT;

/*!
*******************************************************************************
** \brief settting video encode frame rate parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*stream index for setting the bitrate.*/
    GADI_U32                    streamId;
    /*encoding stream target frame rate value.*/
    GADI_VENC_FrameRateEnumT    fps;
}GADI_VENC_FrameRateT;


/*!
*******************************************************************************
** \brief digital PTZ setting parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*channel index.*/
    GADI_U32            channelId;
    /*zoom factor.*/
    GADI_U32            zoomFactor;
    /*zoom offset x.*/
    GADI_S32            offsetX;
    /*zoom offset y.*/
    GADI_S32            offsetY;
}GADI_VENC_DptzParamT;

/*!
*******************************************************************************
** \brief getting digital PTZ orginal information parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*channel index.*/
    GADI_U32            channelId;
    /*zoom x factor.*/
    GADI_U32            zoomFactorX;
    /*zoom y factor.*/
    GADI_U32            zoomFactorY;
    /*zoom offset x.*/
    GADI_S32            offsetX;
    /*zoom offset y.*/
    GADI_S32            offsetY;
}GADI_VENC_DptzOrgParamT;


/*!
*******************************************************************************
** \brief getting VENC attr information.
*******************************************************************************
*/
typedef struct
{
    /*stream id.*/
    GADI_U32 id;
    /*stream type.*/
    GADI_VENC_TypeEnumT type;
    /*stream state.*/
    GADI_VENC_StreamStateEnumT state;
}GADI_CHN_AttrT;

/*!
*******************************************************************************
** \brief getting VENC Stream information.
*******************************************************************************
*/
typedef struct
{
    GADI_U32    stream_id;
    GADI_U32    size;
    GADI_U32    frame_num;
    GADI_S8     pic_type;
    GADI_U32    PTS;
    GADI_U8     *addr;
    GADI_U32	stream_end : 1;
}GADI_VENC_StreamT;

/*!
*******************************************************************************
** \brief DSP map memory informations.
*******************************************************************************
*/
typedef struct
{
    GADI_U8    *addr;
    GADI_U32    length;
}GADI_VENC_DspMapInfoT;

//*****************************************************************************
//*****************************************************************************
//** Global Data
//*****************************************************************************
//*****************************************************************************



//*****************************************************************************
//*****************************************************************************
//** API Functions
//*****************************************************************************
//*****************************************************************************

#ifdef __cplusplus
extern "C" {
#endif


/*!
*******************************************************************************
** \brief Initialize the ADI video encode module.
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_OUT_OF_MEMORY
** - #GADI_VENC_ERR_ALREADY_INITIALIZED
**
** \sa gadi_vin_exit
**
*******************************************************************************
*/
GADI_ERR gadi_venc_init(void);

/*!
*******************************************************************************
** \brief Shutdown the ADI video encode module.
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_NOT_INITIALIZED
**
** \sa gadi_vin_init
**
*******************************************************************************
*/
GADI_ERR gadi_venc_exit(void);

/*!
*******************************************************************************
** \brief Open one ADI video venc module instance.
**
** \param[in] openParams a struct pointer of open prameters.
** \param[in] errorCodePtr pointer to return the error code.
**
** \return Return an valid handle of ADI vin module instance.
**
** \sa gadi_venc_close
**
*******************************************************************************
*/
GADI_SYS_HandleT gadi_venc_open(GADI_VENC_OpenParamsT* openParams, GADI_ERR* errorCodePtr);

/*!
*******************************************************************************
** \brief close one ADI video encode module instance.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_close(GADI_SYS_HandleT handle);

/*!
*******************************************************************************
** \brief close one ADI video encode module instance.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_close(GADI_SYS_HandleT handle);

/*!
*******************************************************************************
** \brief start encoding one stream.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  streamId stream id enum:GADI_VENC_StreamEnumT;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_stop_stream
**
*******************************************************************************
*/
GADI_ERR gadi_venc_start_stream(GADI_SYS_HandleT handle, GADI_U32 streamId);

/*!
*******************************************************************************
** \brief stop encoding one stream.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  streamId stream id enum:GADI_VENC_StreamEnumT;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_start_stream
**
*******************************************************************************
*/
GADI_ERR gadi_venc_stop_stream(GADI_SYS_HandleT handle, GADI_U32 streamId);

/*!
*******************************************************************************
** \brief force one stream IDR .
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  streamId stream id enum:GADI_VENC_StreamEnumT;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_force_idr(GADI_SYS_HandleT handle, GADI_U32 streamId);

/*!
*******************************************************************************
** \brief set one stream bit rate.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  brPar pointer of setting bitrate struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_set_bitrate(GADI_SYS_HandleT handle, GADI_VENC_BitRateRangeT* brPar);

/*!
*******************************************************************************
** \brief set one stream frame rate.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  frPar pointer of setting frame rate struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_set_framerate(GADI_SYS_HandleT handle, GADI_VENC_FrameRateT* frPar);

/*!
*******************************************************************************
** \brief set one channel state.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[out]  statePar pointer of channel state struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_channel_state(GADI_SYS_HandleT handle,
                                             GADI_VENC_ChannelStateT* statePar);

/*!
*******************************************************************************
** \brief get all(four) channels params.
**
** \param[in]   handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[out]  chansParams pointer of all channels parameters struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_channels_params(GADI_SYS_HandleT handle,
                                         GADI_VENC_ChannelsParamsT *chansParams);


/*!
*******************************************************************************
** \brief set all(four) channels parameters.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  bufsParams pointer of all channels parameters struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_set_channels_params(GADI_SYS_HandleT handle,
                                      GADI_VENC_ChannelsParamsT* chansParams);

/*!
*******************************************************************************
** \brief get one encode stream state.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[out]  statePar pointer of stream state struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_stream_state(GADI_SYS_HandleT handle,
                                               GADI_VENC_StreamStateT* statePar);

/*!
*******************************************************************************
** \brief get one encode stream format.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[out]  formatPar pointer of encode stream format struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_stream_format(GADI_SYS_HandleT handle,
                                             GADI_VENC_StreamFormatT *formatPar);

/*!
*******************************************************************************
** \brief set one encode stream format.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  formatPar pointer of encode stream format struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_set_stream_format(GADI_SYS_HandleT handle,
                                             GADI_VENC_StreamFormatT* formatPar);

/*!
*******************************************************************************
** \brief get one h264 encode stream configuration.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[out]  configPar pointer of h264 stream configuration struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_h264_config(GADI_SYS_HandleT handle,
                                               GADI_VENC_H264ConfigT* configPar);

/*!
*******************************************************************************
** \brief set one h264 encode stream configuration.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  configPar pointer of h264 stream configuration struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_set_h264_config(GADI_SYS_HandleT handle,
                                               GADI_VENC_H264ConfigT* configPar);

/*!
*******************************************************************************
** \brief get one mjpeg encode stream configuration.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[out]  configPar pointer of mjpeg stream configuration struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_mjpeg_config(GADI_SYS_HandleT handle,
                                              GADI_VENC_MjpegConfigT* configPar);

/*!
*******************************************************************************
** \brief set one mjpeg encode stream configuration.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  configPar pointer of mjpeg stream configuration struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_set_mjpeg_config(GADI_SYS_HandleT handle,
                                              GADI_VENC_MjpegConfigT* configPar);

/*!
*******************************************************************************
** \brief get digital ptz parameter.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[out]  dptzPar pointer of digital ptz struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_dptz_param(GADI_SYS_HandleT handle, GADI_VENC_DptzParamT* dptzPar);

/*!
*******************************************************************************
** \brief set digital ptz parameter.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]  dptzPar pointer of digital ptz struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_set_dptz_param(GADI_SYS_HandleT handle,
                                              GADI_VENC_DptzParamT* vencDptzPar);

/*!
*******************************************************************************
** \brief get digital ptz orginal parameter.
**
** \param[in]  handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]   vencDptzPar
** \param[out]  vencDptzOrgPar pointer of digital ptz orginal parameters struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_dptz_org_param(GADI_SYS_HandleT handle,
                                                GADI_VENC_DptzParamT *vencDptzPar,
                                                GADI_VENC_DptzOrgParamT *vencDptzOrgPar);

/*!
*******************************************************************************
** \brief map dsp memory.
**
** \param[in]  venHandle   Valid ADI venc instance handle previously opened by
**                          #gadi_venc_open.
** \param[out]  info       pointer of dsp map memory struct;
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_map_dsp(GADI_SYS_HandleT handle, GADI_VENC_DspMapInfoT *info);

/*!
*******************************************************************************
** \brief map bsb.
**
** \param[in]  venHandle     Valid ADI venc instance handle previously opened by
**                          #gadi_venc_open.
**
** \return
** - #GADI_OK
** - #GADI_VENC_ERR_FROM_DRIVER
** - #GADI_VENC_ERR_BAD_PARAMETER
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_map_bsb(GADI_SYS_HandleT handle);
/*!
*******************************************************************************
** \brief query status of parameter.
**
** \param[in]   handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]   streamId.
** \param[out]  chn_attr pointer of GADI_CHN_AttrT parameters struct;
**
** \return
** - #GADI_OK
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_query(GADI_SYS_HandleT handle, GADI_S32 streamId,
GADI_CHN_AttrT * chn_attr);
/*!
*******************************************************************************
** \brief get stream of current.
**
** \param[in]   handle     Valid ADI venc instance handle previously opened by
**                        #gadi_venc_open.
** \param[in]   streamId.
** \param[out]  stream    pointer of GADI_VENC_StreamT parameters struct;
**
** \return
** - #GADI_OK
**
** \sa gadi_venc_open
**
*******************************************************************************
*/
GADI_ERR gadi_venc_get_stream(GADI_SYS_HandleT handle, GADI_S32 streamId,
    GADI_VENC_StreamT * stream);

#ifdef __cplusplus
}
#endif



#endif /* _ADI_VENC_H_ */

