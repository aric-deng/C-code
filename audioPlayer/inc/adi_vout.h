/*!
*****************************************************************************
** \file        ./adi/inc/adi_vout.h
**
** \brief       adi video output porting.
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2013-2014 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ADI_VOUT_H_
#define _ADI_VOUT_H_

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
#define GADI_VOUT_ERR_BAD_PARAMETER                                          \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_BAD_PARAMETER)
/*! Memory allocation failed. */
#define GADI_VOUT_ERR_OUT_OF_MEMORY                                          \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_OUT_OF_MEMORY)
/*! Device already initialised. */
#define GADI_VOUT_ERR_ALREADY_INITIALIZED                                    \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_ALREADY_INITIALIZED)
/*! Device not initialised. */
#define GADI_VOUT_ERR_NOT_INITIALIZED                                        \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_NOT_INITIALIZED)
/*! Feature or function is not available. */
#define GADI_VOUT_ERR_FEATURE_NOT_SUPPORTED                                  \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_FEATURE_NOT_SUPPORTED)
/*! Timeout occured. */
#define GADI_VOUT_ERR_TIMEOUT                                                \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_TIMEOUT)
/*! The device is busy, try again later. */
#define GADI_VOUT_ERR_DEVICE_BUSY                                            \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_DEVICE_BUSY)
/*! Invalid handle was passed. */
#define GADI_VOUT_ERR_INVALID_HANDLE                                         \
                              (GADI_VOUT_MODULE_BASE + GADI_ERR_INVALID_HANDLE)
/*! Semaphore could not be created. */
#define GADI_VOUT_ERR_SEMAPHORE_CREATE                                       \
                               (GADI_VOUT_MODULE_BASE + GADI_ERR_SEMAPHORE_CREATE)
/*! The driver's used version is not supported. */
#define GADI_VOUT_ERR_UNSUPPORTED_VERSION                                    \
                               (GADI_VOUT_MODULE_BASE + GADI_ERR_UNSUPPORTED_VERSION)
/*! The driver's used version is not supported. */
#define GADI_VOUT_ERR_FROM_DRIVER                                            \
                               (GADI_VOUT_MODULE_BASE + GADI_ERR_FROM_DRIVER)
/*! The device/handle is not open.. */
#define GADI_VOUT_ERR_NOT_OPEN                                               \
                               (GADI_VOUT_MODULE_BASE + GADI_ERR_NOT_OPEN)


//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************
/*!
*******************************************************************************
** \brief video output support resolution enum.
*******************************************************************************
*/
typedef enum {
    /*video output resolution: 480I.*/
    GADI_VOUT_RESOLUTION_480I = 0,
    /*video output resolution: 576I.*/
    GADI_VOUT_RESOLUTION_576I,
    /*video output resolution: max number*/
    GADI_VOUT_RESOLUTION_NUMBER,
} GADI_VOUT_ResolutionModeEnumT;

/*!
*******************************************************************************
** \brief video output support output device enum.
*******************************************************************************
*/
typedef enum {
    /*video output via auto select.(currently not support).*/
    GADI_VOUT_DEVICE_AUTO = 0,
    /*video output disable, enable overlay.*/
    GADI_VOUT_DEVICE_DISABLE = 1,
    /*video output via CVBS, disable overlay.*/
    GADI_VOUT_DEVICE_CVBS = 2,
    /*video output max device number.*/
    GADI_VOUT_DEVICE_NUMBER,
} GADI_VOUT_DeviceEnumT;

/*!
*******************************************************************************
** \brief video output framebuffer id.
*******************************************************************************
*/
typedef enum {
    /*video output framebuffer 0.*/
    GADI_VOUT_FB_0 = 0,
    /*video output framebuffer 1.*/
    GADI_VOUT_FB_1 = 1,
    /*video output support max framebuffer number.*/
    GADI_VOUT_FB_NUMBER,
} GADI_VOUT_FbEnumT;


//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************

/*!
*******************************************************************************
** \brief video output setting parameters struct.
*******************************************************************************
*/
typedef struct
{
    /*video output resolution mode,*/
    GADI_VOUT_ResolutionModeEnumT   resoluMode;
    /*video output device type.*/
    GADI_VOUT_DeviceEnumT           deviceType;
} GADI_VOUT_SettingParamsT;


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
** \brief Initialize the ADI video out module.
**
** \return
** - #GADI_OK
** - #GADI_VOUT_ERR_ALREADY_INITIALIZED
** - #GADI_VOUT_ERR_OUT_OF_MEMORY
**
** \sa gadi_vout_exit
**
*******************************************************************************
*/
GADI_ERR gadi_vout_init(void);

/*!
*******************************************************************************
** \brief Shutdown the ADI video output module.
**
** \return
** - #GADI_OK
** - #GADI_VOUT_ERR_NOT_INITIALIZED
**
** \sa gadi_vout_init
**
*******************************************************************************
*/
GADI_ERR gadi_vout_exit(void);

/*!
*******************************************************************************
** \brief Open one ADI video out module instance.
**
** \param[in] openParams a struct pointer of open prameters.
** \param[in] errorCodePtr pointer to return the error code.
**
** \return Return an valid handle of ADI vout module instance.
**
** \sa gadi_vout_close
**
*******************************************************************************
*/
GADI_SYS_HandleT gadi_vout_open(GADI_ERR* errorCodePtr);

/*!
*******************************************************************************
** \brief close one ADI video output module instance.
**
** \param[in]  handle     Valid ADI vin instance handle previously opened by
**                        #gadi_vout_open.
**
** \return
** - #GADI_OK
** - #GADI_VOUT_ERR_BAD_PARAMETER
**
** \sa gadi_vout_open
**
*******************************************************************************
*/
GADI_ERR gadi_vout_close(GADI_SYS_HandleT handle);

/*!
*******************************************************************************
** \brief get video output parameters.
**
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vout_open.
** \param[in]  vinParams   pointer to return the video output parameters struct.
**
** \return
** - #GADI_OK
** - #GADI_VOUT_ERR_NOT_OPEN
** - #GADI_VOUT_ERR_FROM_DRIVER
**
** \sa gadi_vout_open
**
*******************************************************************************
*/
GADI_ERR gadi_vout_get_params(GADI_SYS_HandleT handle, GADI_VOUT_SettingParamsT* voutParams);

/*!
*******************************************************************************
** \brief set video output parameters.
**
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vin_open.
** \param[in]  vinParams   input pointer of video output parameters struct.
**
** \return
** - #GADI_OK
** - #GADI_VOUT_ERR_NOT_OPEN
** - #GADI_VOUT_ERR_FROM_DRIVER
** - #GADI_VOUT_ERR_BAD_PARAMETER
**
** \sa gadi_vout_open
**
*******************************************************************************
*/
GADI_ERR gadi_vout_set_params(GADI_SYS_HandleT handle, GADI_VOUT_SettingParamsT* voutParams);


/*!
*******************************************************************************
** \brief select vout framebuffer.
**
** \param[in]  handle     Valid dsp ADI instance handle previously opened by
**                        #gadi_vin_open.
** \param[in]  fbId       framebuffer index.
**
** \return
** - #GADI_OK
** - #GADI_VOUT_ERR_NOT_OPEN
** - #GADI_VOUT_ERR_FROM_DRIVER
** - #GADI_VOUT_ERR_BAD_PARAMETER
**
** \sa gadi_vout_open
**
*******************************************************************************
*/
GADI_ERR gadi_vout_select_fb(GADI_SYS_HandleT handle, GADI_U8 fbId);

#ifdef __cplusplus
    }
#endif


#endif /* _ADI_VOUT_H_ */
