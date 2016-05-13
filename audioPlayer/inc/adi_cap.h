/*!
*****************************************************************************
** \file        adi/include/adi_cap.h
**
** \brief       ADI still picture capture module header file
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2014 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ADI_CAP_H_
#define _ADI_CAP_H_



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
#define GADI_CAP_ERR_BAD_PARAMETER                                          \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_BAD_PARAMETER)
/*! Memory allocation failed. */
#define GADI_CAP_ERR_OUT_OF_MEMORY                                          \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_OUT_OF_MEMORY)
/*! Device already initialised. */
#define GADI_CAP_ERR_ALREADY_INITIALIZED                                    \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_ALREADY_INITIALIZED)
/*! Device not initialised. */
#define GADI_CAP_ERR_NOT_INITIALIZED                                        \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_NOT_INITIALIZED)
/*! Feature or function is not available. */
#define GADI_CAP_ERR_FEATURE_NOT_SUPPORTED                                  \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_FEATURE_NOT_SUPPORTED)
/*! Timeout occured. */
#define GADI_CAP_ERR_TIMEOUT                                                \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_TIMEOUT)
/*! The device is busy, try again later. */
#define GADI_CAP_ERR_DEVICE_BUSY                                            \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_DEVICE_BUSY)
/*! Invalid handle was passed. */
#define GADI_CAP_ERR_INVALID_HANDLE                                         \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_INVALID_HANDLE)
/*! Semaphore could not be created. */
#define GADI_CAP_ERR_SEMAPHORE_CREATE                                       \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_SEMAPHORE_CREATE)
/*! The driver's used version is not supported. */
#define GADI_CAP_ERR_UNSUPPORTED_VERSION                                    \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_UNSUPPORTED_VERSION)
/*! The driver's used version is not supported. */
#define GADI_CAP_ERR_FROM_DRIVER                                            \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_FROM_DRIVER)
/*! The device/handle is not open.. */
#define GADI_CAP_ERR_NOT_OPEN                                               \
                              (GADI_CAP_MODULE_BASE + GADI_ERR_NOT_OPEN)


#define GADI_CAP_QUALITY_MAX        100

#define GADI_CAP_QUALITY_MIN        1

#define GADI_CAP_NUMBER_MAX         100

#define GADI_CAP_NUMBER_MIN         1

//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************



//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************

/*!
*******************************************************************************
** \brief Open parameter for still picture capture module.
*******************************************************************************
*/
typedef struct {

    /*!
        quality factor of JPEG picture.
        min - 1
        max - 100
    */
    GADI_S32        quality;



} GADI_CAP_OpenParamsT;

/*!
*******************************************************************************
** \brief Start parameter for still picture capture module.
*******************************************************************************
*/
typedef struct {

    /*!
        file name and path to contain the captured picture.
    */
    const char*     fileName;

    /*!
        flag to capture raw picture.
    */
    GADI_U8         capRaw;

    /*!
        flag to capture jpeg picture.
    */
    GADI_U8         capJpeg;

    /*!
        flag to capture raw thumbnail picture.
    */
    GADI_U8         capThumbnail;

    /*!
        number of pictures to be captured.
        min - 1
        max - 100
    */
    GADI_U8         capNum;

    /*!
        width of jpeg picture to be captured.
    */
    GADI_U32        jpegWidth;

    /*!
        height of jpeg picture to be captured.
    */
    GADI_U32        jpegHeight;

    /*!
        width of thumbnail picture to be captured.
    */
    GADI_U32        thumbnailWidth;

    /*!
        height of thumbnail picture to be captured.
    */
    GADI_U32        thumbnialHeight;

} GADI_CAP_StartParamsT;


/*!
*******************************************************************************
** \brief Start parameter for capture one JPEG.
*******************************************************************************
*/
typedef struct {

    /*!
        JPEG file handle.
    */
    FILE*           fileHandle;

    /*!
        width of jpeg picture to be captured.
    */
    GADI_U32        jpegWidth;

    /*!
        height of jpeg picture to be captured.
    */
    GADI_U32        jpegHeight;

} GADI_CAP_JpegParamst;

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

GADI_ERR gadi_cap_init(void);

GADI_SYS_HandleT gadi_cap_open(GADI_CAP_OpenParamsT *openParamPtr,
                               GADI_ERR *errCodePtr);
GADI_ERR gadi_cap_close(GADI_SYS_HandleT capHandle);

GADI_ERR gadi_cap_start(GADI_SYS_HandleT capHandle,
                        GADI_CAP_StartParamsT *startParamsPtr);

GADI_ERR gadi_cap_stop(GADI_SYS_HandleT capHandle);

GADI_ERR gadi_cap_jpeg(GADI_SYS_HandleT capHandle,
                       GADI_CAP_JpegParamst* jpegPar);

GADI_ERR gadi_cap_exit(void);

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif



#endif /* _ADI_CAP_H_ */

