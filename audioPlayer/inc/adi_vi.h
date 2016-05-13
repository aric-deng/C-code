/*!
*****************************************************************************
** \file        ./adi/inc/adi_vi.h
**
** \brief       adi video input porting.
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2013-2014 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ADI_VI_H_
#define _ADI_VI_H_

#include "stdio.h"
#include "adi_types.h"


//*****************************************************************************
//*****************************************************************************
//** Defines and Macros
//*****************************************************************************
//*****************************************************************************

/*
*******************************************************************************
** Defines for general error codes of the module.
*******************************************************************************
*/
/*! Bad parameter passed. */
#define GADI_VI_ERR_BAD_PARAMETER                                          \
                              (GADI_VI_MODULE_BASE + GADI_ERR_BAD_PARAMETER)
/*! Memory allocation failed. */
#define GADI_VI_ERR_OUT_OF_MEMORY                                          \
                              (GADI_VI_MODULE_BASE + GADI_ERR_OUT_OF_MEMORY)
/*! Device already initialised. */
#define GADI_VI_ERR_ALREADY_INITIALIZED                                    \
                              (GADI_VI_MODULE_BASE + GADI_ERR_ALREADY_INITIALIZED)
/*! Device not initialised. */
#define GADI_VI_ERR_NOT_INITIALIZED                                        \
                              (GADI_VI_MODULE_BASE + GADI_ERR_NOT_INITIALIZED)
/*! Feature or function is not available. */
#define GADI_VI_ERR_FEATURE_NOT_SUPPORTED                                  \
                              (GADI_VI_MODULE_BASE + GADI_ERR_FEATURE_NOT_SUPPORTED)
/*! Timeout occured. */
#define GADI_VI_ERR_TIMEOUT                                                \
                              (GADI_VI_MODULE_BASE + GADI_ERR_TIMEOUT)
/*! The device is busy, try again later. */
#define GADI_VI_ERR_DEVICE_BUSY                                            \
                              (GADI_VI_MODULE_BASE + GADI_ERR_DEVICE_BUSY)
/*! Invalid handle was passed. */
#define GADI_VI_ERR_INVALID_HANDLE                                         \
                              (GADI_VI_MODULE_BASE + GADI_ERR_INVALID_HANDLE)
/*! Semaphore could not be created. */
#define GADI_VI_ERR_SEMAPHORE_CREATE                                       \
                               (GADI_VI_MODULE_BASE + GADI_ERR_SEMAPHORE_CREATE)
/*! The driver's used version is not supported. */
#define GADI_VI_ERR_UNSUPPORTED_VERSION                                    \
                               (GADI_VI_MODULE_BASE + GADI_ERR_UNSUPPORTED_VERSION)
/*! The driver's used version is not supported. */
#define GADI_VI_ERR_FROM_DRIVER                                            \
                               (GADI_VI_MODULE_BASE + GADI_ERR_FROM_DRIVER)
/*! The device/handle is not open.. */
#define GADI_VI_ERR_NOT_OPEN                                               \
                               (GADI_VI_MODULE_BASE + GADI_ERR_NOT_OPEN)


//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************
/*!
*******************************************************************************
** \brief video input sensor mirror mode enum.
*******************************************************************************
*/
typedef enum {
    GADI_VI_MIRROR_HORRIZONTALLY_VERTICALLY    = 0,
    GADI_VI_MIRROR_HORRIZONTALLY,
    GADI_VI_MIRROR_VERTICALLY,
    GADI_VI_MIRROR_NONE,
    GADI_VI_MIRROR_AUTO            = 255,
}GADI_VI_MirrorPatternEnumT;

/*!
*******************************************************************************
** \brief video input bayer data patter enum.
*******************************************************************************
*/
typedef enum {
    GADI_VI_BAYER_PATTERN_RG       = 0,
    GADI_VI_BAYER_PATTERN_BG,
    GADI_VI_BAYER_PATTERN_GR,
    GADI_VI_BAYER_PATTERN_GB,
    GADI_VI_BAYER_PATTERN_AUTO     = 255,
}GADI_VI_BayerPatternEnumT;

/*!
*******************************************************************************
** \brief video input support frame rate.
*******************************************************************************
*/
typedef enum {
    /*use default sensor frame rate.*/
    GADI_VI_FPS_AUTO   = 0,
    /*fps:1.*/
    GADI_VI_FPS_1      = 1,
    GADI_VI_FPS_2      = 2,
    GADI_VI_FPS_3      = 3,
    GADI_VI_FPS_4      = 4,
    GADI_VI_FPS_5      = 5,
    GADI_VI_FPS_6      = 6,
    GADI_VI_FPS_10     = 10,
    GADI_VI_FPS_12     = 12,
    GADI_VI_FPS_13     = 13,
    GADI_VI_FPS_14     = 14,
    GADI_VI_FPS_15     = 15,
    GADI_VI_FPS_20     = 20,
    GADI_VI_FPS_24     = 24,
    GADI_VI_FPS_25     = 25,
    GADI_VI_FPS_30     = 30,
    GADI_VI_FPS_50     = 50,
    GADI_VI_FPS_60     = 60,
    GADI_VI_FPS_120    = 120,
    /*fps:3.125.*/
    GADI_VI_FPS_3_125  = 3125000,
    /*fps:3.75.*/
    GADI_VI_FPS_3_75   = 37500,
    /*fps:6.25.*/
    GADI_VI_FPS_6_25   = 62500,
    /*fps:7.5.*/
    GADI_VI_FPS_7_5    = 750,
    /*fps:12.5.*/
    GADI_VI_FPS_12_5   = 1250,
    /*fps:23.976.*/
    GADI_VI_FPS_23_976 = 23976000,
    /*fps:29.97.*/
    GADI_VI_FPS_29_97  = 299700,
    /*fps:59.94.*/
    GADI_VI_FPS_59_94  = 599400,
}GADI_VI_FrameRateEnumT;

/*!
*******************************************************************************
** \brief video input sensor operation enum.
*******************************************************************************
*/
typedef enum {
    GADI_VI_LINEAR_MODE = 0,
    /*currently, support WDR mode(only ar0331 sensor).*/
    GADI_VI_HDR_MODE,
} GADI_VI_SensorOpModeEnumT;

//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************

/*!
*******************************************************************************
** \brief video input mirror mode struct.
*******************************************************************************
*/
typedef struct
{
    /*enum GADI_VI_MirrorPatternEnumT pattern*/
    GADI_VI_MirrorPatternEnumT       mirrorPattern;
    /*enum GADI_VI_BayerPatternEnumT bayer_pattern*/
    GADI_VI_BayerPatternEnumT        bayerPattern;
} GADI_VI_MirrorModeParamsT;

/*!
*******************************************************************************
** \brief video input setting parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*input video resolution mode,
    alway set 0: means use default sensor video output resolution.*/
    GADI_U32                    resoluMode;
    /*input video frame rate.*/
    GADI_VI_FrameRateEnumT     frameRate;
    /*input video mirror mode.*/
    GADI_VI_MirrorModeParamsT  mirrorMode;
} GADI_VI_SettingParamsT;


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
** \brief Initialize the ADI video input module.
**
** \return
** - #GADI_OK
** - #GADI_DSP_ERR_ALREADY_INITIALIZED
** - #GADI_DSP_ERR_OUT_OF_MEMORY
**
** \sa gadi_vi_exit
**
*******************************************************************************
*/
GADI_ERR gadi_vi_init(void);

/*!
*******************************************************************************
** \brief Shutdown the ADI video input module.
**
** \return
** - #GADI_OK
** - #GADI_DSP_ERR_NOT_INITIALIZED
**
** \sa gadi_vi_init
**
*******************************************************************************
*/
GADI_ERR gadi_vi_exit(void);

/*!
*******************************************************************************
** \brief Open one ADI video input module instance.
**
** \param[in] errorCodePtr pointer to return the error code.
**
** \return Return an valid handle of ADI vin module instance.
**
** \sa gadi_vi_close
**
*******************************************************************************
*/
GADI_SYS_HandleT gadi_vi_open(GADI_ERR* errorCodePtr);

/*!
*******************************************************************************
** \brief close one ADI video input module instance.
**
** \param[in]  handle     Valid ADI vin instance handle previously opened by
**                        #gadi_vi_open.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_close(GADI_SYS_HandleT handle);

/*!
*******************************************************************************
** \brief get video input resolution.
**
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  viWidth   pointer to return the video input width.
** \param[in]  viHeight  pointer to return the video input height.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_get_resolution(GADI_SYS_HandleT handle, GADI_U32* viWidth, GADI_U32* viHeight);

/*!
*******************************************************************************
** \brief get video input information.
**
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  viSettingParams   pointer to return the video input information struct.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_get_params(GADI_SYS_HandleT handle, GADI_VI_SettingParamsT* viSettingParams);


/*!
*******************************************************************************
** \brief set video input information.
**
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vin_open.
** \param[in]  vinSettingParams   input pointer of video input information struct.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vin_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_set_params(GADI_SYS_HandleT handle, GADI_VI_SettingParamsT* viSettingParams);

/*!
*******************************************************************************
** \brief get special sensor operation mode(Liner,WDR,HDR), currently, only ar0331
**        support
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  mode   input pointer of sensor operation mode.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_get_operation_mode(GADI_SYS_HandleT handle, GADI_U32 *mode);

/*!
*******************************************************************************
** \brief set special sensor operation mode(Liner,WDR,HDR), currently, only ar0331
**        support.
**
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  mode   input value of sensor operation mode.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vin_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_set_operation_mode(GADI_SYS_HandleT handle, GADI_U32 mode);


/*!
*******************************************************************************
** \brief enable or disable VI. Enable VI means start to receive sensor data.
**
** \param[in]  handle    Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  enable    1: enable, 0: disable.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_enable(GADI_SYS_HandleT handle, GADI_U32 enable);

/*!
*******************************************************************************
** \brief set sensor output mirror/rotate mode.
**
** \param[in]  handle    Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  mirrorModePtr  input pointer of VI data mirror struct.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_set_mirror_mode(GADI_SYS_HandleT handle,
                        GADI_VI_MirrorModeParamsT  *mirrorModePtr);

/*!
*******************************************************************************
** \brief get sensor output mirror/rotate mode.
**
** \param[in]  handle    Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  mirrorModePtr  input pointer of VI data mirror struct.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_get_mirror_mode(GADI_SYS_HandleT handle,
                        GADI_VI_MirrorModeParamsT  *mirrorModePtr);

/*!
*******************************************************************************
** \brief set VI module slows shutter mode.
**
** \param[in]  handle    Valid dsp ADI instance handle previously opened by
**                        #gadi_vi_open.
** \param[in]  slowshuttermode  0:enter slow shutter mode, 1:restore normal mode.
** \param[out] Framerate  return VI framerate.
**
** \return
** - #GADI_OK
** - #GADI_VI_ERR_NOT_OPEN
** - #GADI_VI_ERR_FROM_DRIVER
** - #GADI_VI_ERR_BAD_PARAMETER
**
** \sa gadi_vi_open
**
*******************************************************************************
*/
GADI_ERR gadi_vi_set_slowshutter_mode(GADI_SYS_HandleT handle,
                        GADI_U32  slowshuttermode,
                        GADI_S32* Framerate);

/*!
*******************************************************************************
** \brief set ir cut mode.
**
** \**
** \sa gadi_vi_set_ircut_control
**
*******************************************************************************
*/
#define GADI_SET_IRCUT_CLEAR    0
#define GADI_SET_IRCUT_NIGHT    1
#define GADI_SET_IRCUT_DAY      2
GADI_ERR gadi_vi_set_ircut_control(GADI_SYS_HandleT handle, u32 mode);

#ifdef __cplusplus
    }
#endif


#endif /* _ADI_VI_H_ */
