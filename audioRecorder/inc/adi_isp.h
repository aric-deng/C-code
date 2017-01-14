/*!
*****************************************************************************
** \file        adi/include/adi_isp.h
**
** \brief       API for ISP and 3A.
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2015 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ADI_ISP_H_
#define _ADI_ISP_H_

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
#define GADI_ISP_ERR_BAD_PARAMETER                                          \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_BAD_PARAMETER)
/*! Memory allocation failed. */
#define GADI_ISP_ERR_OUT_OF_MEMORY                                          \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_OUT_OF_MEMORY)
/*! Device already initialised. */
#define GADI_ISP_ERR_ALREADY_INITIALIZED                                    \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_ALREADY_INITIALIZED)
/*! Device not initialised. */
#define GADI_ISP_ERR_NOT_INITIALIZED                                        \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_NOT_INITIALIZED)
/*! Feature or function is not available. */
#define GADI_ISP_ERR_FEATURE_NOT_SUPPORTED                                  \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_FEATURE_NOT_SUPPORTED)
/*! Timeout occured. */
#define GADI_ISP_ERR_TIMEOUT                                                \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_TIMEOUT)
/*! The device is busy, try again later. */
#define GADI_ISP_ERR_DEVICE_BUSY                                            \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_DEVICE_BUSY)
/*! Invalid handle was passed. */
#define GADI_ISP_ERR_INVALID_HANDLE                                         \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_INVALID_HANDLE)
/*! Semaphore could not be created. */
#define GADI_ISP_ERR_SEMAPHORE_CREATE                                       \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_SEMAPHORE_CREATE)
/*! The driver's used version is not supported. */
#define GADI_ISP_ERR_UNSUPPORTED_VERSION                                    \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_UNSUPPORTED_VERSION)
/*! The driver's used version is not supported. */
#define GADI_ISP_ERR_FROM_DRIVER                                            \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_FROM_DRIVER)
/*! The device/handle is not open.. */
#define GADI_ISP_ERR_NOT_OPEN                                               \
                              (GADI_ISP_MODULE_BASE + GADI_ERR_NOT_OPEN)



/*! Gamma table look-up-table size.*/
#define GADI_ISP_GAMMA_LUT_SIZE     256

#define GADI_AWB_MAX_SPEED          0xFF
#define GADI_AWB_MIN_SPEED          0x01
#define GADI_AWB_AUTO_SPEED         0x80
//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************

typedef enum {

    GADI_ISP_EXPOSURE_TYPE_AUTO,

    GADI_ISP_EXPOSURE_TYPE_MANUAL,

} GADI_ISP_ExpTypeEnumT;


typedef enum {

    GADI_ISP_ANTIFLICKER_FREQ_50HZ,

    GADI_ISP_ANTIFLICKER_FREQ_60HZ

} GADI_ISP_AntiFlickerFreqEnumT;

typedef enum {

    GADI_ISP_WB_TYPE_AUTO,

    GADI_ISP_WB_TYPE_MANUAL,

} GADI_ISP_WbTypeEnumT;

typedef enum {

    GADI_ISP_AWB_ALGO_AUTO,

    GADI_ISP_AWB_ALGO_GRAYWORLD,

    GADI_ISP_AWB_ALGO_NUM,

} GADI_ISP_AwbAlgoEnumT;

typedef enum {

    GADI_ISP_AWB_COLOR_TEMP_AUTO,

    GADI_ISP_AWB_COLOR_TEMP_2800,

    GADI_ISP_AWB_COLOR_TEMP_3200,

    GADI_ISP_AWB_COLOR_TEMP_4500,

    GADI_ISP_AWB_COLOR_TEMP_5500,

    GADI_ISP_AWB_COLOR_TEMP_6500,

    GADI_ISP_AWB_COLOR_TEMP_7500,

    GADI_ISP_AWB_COLOR_TEMP_NUM,

} GADI_ISP_AwbColorTempEnumT;


typedef enum {

    GADI_ISP_SENSOR_UNKNOWN = 0,

    GADI_ISP_SENSOR_AR0130,

    GADI_ISP_SENSOR_AR0330,

    GADI_ISP_SENSOR_GC1004,

    GADI_ISP_SENSOR_H22,

    GADI_ISP_SENSOR_IMX222,

    GADI_ISP_SENSOR_IMX238,

    GADI_ISP_SENSOR_OV2710,

    GADI_ISP_SENSOR_OV9710,

    GADI_ISP_SENSOR_JXH42,

    GADI_ISP_SENSOR_BG0701,

    GADI_ISP_SENSOR_BYD0703,

    GADI_ISP_SENSOR_GC1014,

    GADI_ISP_SENSOR_PP1210,

    GADI_ISP_SENSOR_SC1015,

    GADI_ISP_SENSOR_SC1020,

    GADI_ISP_SENSOR_SC1035,

    GADI_ISP_SENSOR_SC1042,

    GADI_ISP_SENSOR_SC1045,

    GADI_ISP_SENSOR_SC2035,

} GADI_ISP_SensorModelEnumT;

//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************

typedef struct {

    GADI_U32 ispModuleIndex;

} GADI_ISP_OpenParamsT;


/*!
*******************************************************************************
** \brief Attributes parameter for AE algorithom.
*******************************************************************************
*/
typedef struct {

    /*! indicate the convergence speed of AE algorithm. */
    GADI_U32        speed;

    /*! values from 1 to 8000.
        means the counts of shutter works each second.
        The value of shutterTimeMin should be larger than shutterTimeMax.
        For the larger the value is, the smaller shutter time will be.
    */
    GADI_U32        shutterTimeMin;

    /*! values from 1 to 8000.
        means the counts of shutter works each second.
    */
    GADI_U32        shutterTimeMax;

    GADI_U32        gainMax;

    GADI_U32        gainMin;
    /*!
        ae target ratio level
        possible value: [0, 0xFF]
        default value: 0x80,
        [0,   0x7f]  decrease ae target
        [0x80,0xff]  increase ae target
    */
    GADI_U32        tagetRatio;
} GADI_ISP_AeAttrT;

typedef struct {

} GADI_ISP_AeLibInitParamT;

typedef struct {

} GADI_ISP_AeLibProcParamT;


typedef void (*GADI_ISP_AeLibInitFuncT)(GADI_SYS_HandleT ispHandle,
                                         GADI_ISP_AeLibInitParamT *paramPtr);
typedef void (*GADI_ISP_AeLibProcFuncT)(GADI_SYS_HandleT ispHandle,
                                         GADI_ISP_AeLibProcParamT *paramPtr);
typedef void (*GADI_ISP_AeLibExitFuncT)(GADI_SYS_HandleT ispHandle);

typedef struct {

    GADI_ISP_AeLibInitFuncT aeInitFunc;

    GADI_ISP_AeLibProcFuncT aeProcFunc;

    GADI_ISP_AeLibExitFuncT aeExitFunc;

} GADI_ISP_AeLibT;

typedef struct {

    /*!
        parameter to enable or disable the antiflicker feature.
        possible value:
        - 0     disable the antiflicker
        - 1     enable the antiflicker
    */
    GADI_U8                         enableDisable;

    /*!
    ** parameter to indicate the frequency in which the flicker will be
    ** suppressed.
    */
    GADI_ISP_AntiFlickerFreqEnumT   freq;

} GADI_ISP_AntiFlickerParamT;


typedef struct {

    /* 1 - 8000 */
    GADI_U32        shutterTime;

    /* 0 - 64 */
    GADI_U32        gain;

} GADI_ISP_MeAttrT;

typedef struct {

    GADI_U16 highCCM[9];

    GADI_U16 midCCM[9];

    GADI_U16 lowCCM[9];

} GADI_ISP_ColorMatrixT;



typedef struct {

    /*!
        0: disable;
        1: enable
    */
    GADI_U8     enableDisable;

    /*!
        Gamma Table
    */
    GADI_U16    gammaTable[GADI_ISP_GAMMA_LUT_SIZE];

} GADI_ISP_GammaAttrT;


typedef struct {

    /*!
        set the convergence speed of AE algorithm.
        the valid value of this field is #GADI_AWB_MIN_SPEED to #GADI_AWB_MAX_SPEED
        and the internal automatical value value is #GADI_AWB_AUTO_SPEED.
        Lower than #GADI_AWB_AUTO_SPEED will decrease the speed of AWB and
        larger than #GADI_AWB_AUTO_SPEED will increase the speed.
    */
    GADI_U8                     speed;

    /*!
        the algorithm of the auto white blance
    */
    GADI_ISP_AwbAlgoEnumT       algo;

    /*!
        specify the environment color temperture to make the AWB work
        more effecient.
    */
    GADI_ISP_AwbColorTempEnumT  colorTemp;

} GADI_ISP_AwbAttrT;

typedef struct {

} GADI_ISP_AwbLibInitParamT;

typedef struct {

} GADI_ISP_AwbLibProcParamT;


typedef void (*GADI_ISP_AwbLibInitFuncT)(GADI_SYS_HandleT ispHandle,
                                         GADI_ISP_AwbLibInitParamT *paramPtr);
typedef void (*GADI_ISP_AwbLibProcFuncT)(GADI_SYS_HandleT ispHandle,
                                         GADI_ISP_AwbLibProcParamT *paramPtr);
typedef void (*GADI_ISP_AwbLibExitFuncT)(GADI_SYS_HandleT ispHandle);

typedef struct {

    GADI_ISP_AwbLibInitFuncT awbInitFunc;

    GADI_ISP_AwbLibProcFuncT awbProcFunc;

    GADI_ISP_AwbLibExitFuncT awbExitFunc;

} GADI_ISP_AwbLibT;


/*!
*******************************************************************************
** \brief Attributes parameter for Manual White Blance.
*******************************************************************************
*/
typedef struct {

    /* 0x0000 - 0x4000 */
    GADI_U32                gainRed;

    /* 0x0000 - 0x4000 */
    GADI_U32                gainGreen;

    /* 0x0000 - 0x4000 */
    GADI_U32                gainBlue;

} GADI_ISP_MwbAttrT;



typedef struct {

    /*!
        0: disable;
        1: enable;
    */
    GADI_U8     enableDisable;

    /*!
        0: disable manual[using auto mode];
        1: enable manual [disable auto mode];
    */
    GADI_U8     enableManual;

    /*!
        sharpeness level
        possible value: [0, 0xFF]
        default value: 0x80
    */
    GADI_S32    level;

} GADI_ISP_SharpenAttrT;


typedef struct {

    /*!
        0: disable;
        1: enable
    */
    GADI_U8     enableDisable;

    /*!
        Shading parameter T.B.D
    */

} GADI_ISP_ShadingAttrT;


typedef struct {

    /*!
        0: disable;
        1: enable
    */
    GADI_U8     enableDisable;

    /*!
        Denoise level
        possible value: [0, 0xFF]
        default value : 0x1;
    */
    GADI_S32    level;

} GADI_ISP_DenoiseAttrT;


typedef struct {

    /*!
        Black level value for
            R   blackLevel[0]
            Gr  blackLevel[1]
            Gb  blackLevel[2]
            B   blackLevel[3]
    */
    GADI_S32    blackLevel[4];

} GADI_ISP_BlackLevelAttrT;


typedef struct {

    /*!
        0: disable
        1: enable
    */
    GADI_S32    enableDisable;

    /*!
        Not working now.
    */
    GADI_S32    level;

} GADI_ISP_BacklightAttrT;


typedef struct {

    /*!
        0: disable auto contrast
        1: enable  auto contrast
    */
    GADI_S32    enableAuto;

    /*!
        manual contrast strength.
        possible value: [0, 0xFF]
        default value: 0x80
    */
    GADI_S32    manualStrength;
    /*!
        auto contrast strength
        possible value: [0, 0xFF]
        default value: 0x80
    */
    GADI_S32    autoStrength;

} GADI_ISP_ContrastAttrT;

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
** \brief Init ISP software environment.
*******************************************************************************
*/
GADI_ERR gadi_isp_init(void);

/*!
*******************************************************************************
** \brief Exit ISP software environment.
*******************************************************************************
*/
GADI_ERR gadi_isp_exit(void);

/*!
*******************************************************************************
** \brief Open and initialize the ISP hardware module and bind it to an software
**        handle.
*******************************************************************************
*/
GADI_SYS_HandleT gadi_isp_open(GADI_ISP_OpenParamsT *openParamPtr,
                               GADI_ERR *errorCodePtr);

/*!
*******************************************************************************
** \brief Shutdown the ISP hardware module and release the software handle.
*******************************************************************************
*/
GADI_ERR gadi_isp_close(GADI_SYS_HandleT ispHandle);

/*!
*******************************************************************************
** \brief Via this API we can set the tuned parameter into ADI library which
**        could be used by 3A algorithm and other hardware components.
*******************************************************************************
*/
GADI_ERR gadi_isp_load_param(GADI_SYS_HandleT ispHandle, const char* paramFile);

GADI_ERR gadi_isp_start(GADI_SYS_HandleT ispHandle);

GADI_ERR gadi_isp_stop(GADI_SYS_HandleT ispHandle);

GADI_ERR gadi_isp_set_exposure_type(GADI_SYS_HandleT ispHandle,
                                    GADI_ISP_ExpTypeEnumT type);

GADI_ERR gadi_isp_set_ae_attr(GADI_SYS_HandleT ispHandle, GADI_ISP_AeAttrT *attrPtr);


GADI_ERR gadi_isp_get_ae_attr(GADI_SYS_HandleT ispHandle, GADI_ISP_AeAttrT *attrPtr);


GADI_ERR gadi_isp_set_ae_lib(GADI_SYS_HandleT ispHandle,
                                    GADI_ISP_AeLibT customAeLib);

GADI_ERR gadi_isp_set_me_attr(GADI_SYS_HandleT ispHandle, GADI_ISP_MeAttrT *attrPtr);


GADI_ERR gadi_isp_get_me_attr(GADI_SYS_HandleT ispHandle, GADI_ISP_MeAttrT *attrPtr);


GADI_ERR gadi_isp_set_slow_framerate(GADI_SYS_HandleT ispHandle,
                                        GADI_U8 factor);

GADI_ERR gadi_isp_get_slow_framerate(GADI_SYS_HandleT ispHandle,
                                        GADI_U8 *factor);

GADI_ERR gadi_isp_set_antiflicker(GADI_SYS_HandleT ispHandle,
                                  GADI_ISP_AntiFlickerParamT *paramPtr);

GADI_ERR gadi_isp_get_antiflicker(GADI_SYS_HandleT ispHandle,
                                  GADI_ISP_AntiFlickerParamT *paramPtr);
GADI_ERR gadi_isp_set_ccm(GADI_SYS_HandleT ispHandle,
                          GADI_ISP_ColorMatrixT *paramPtr);

GADI_ERR gadi_isp_get_ccm(GADI_SYS_HandleT ispHandle,
                          GADI_ISP_ColorMatrixT *paramPtr);

GADI_ERR gadi_isp_set_gamma_attr(GADI_SYS_HandleT ispHandle,
                                        GADI_ISP_GammaAttrT *attrPtr);

GADI_ERR gadi_isp_get_gamma_attr(GADI_SYS_HandleT ispHandle,
                                        GADI_ISP_GammaAttrT *attrPtr);

GADI_ERR gadi_isp_set_wb_type(GADI_SYS_HandleT ispHandle,
                              GADI_ISP_WbTypeEnumT type);

GADI_ERR gadi_isp_set_awb_attr(GADI_SYS_HandleT ispHandle,
                               GADI_ISP_AwbAttrT *attrPtr);

GADI_ERR gadi_isp_get_awb_attr(GADI_SYS_HandleT ispHandle,
                               GADI_ISP_AwbAttrT *attrPtr);

GADI_ERR gadi_isp_set_awb_lib(GADI_SYS_HandleT ispHandle,
                                    GADI_ISP_AwbLibT customAwbLib);

GADI_ERR gadi_isp_set_mwb_attr(GADI_SYS_HandleT ispHandle,
                               GADI_ISP_MwbAttrT *attrPtr);
GADI_ERR gadi_isp_get_mwb_attr(GADI_SYS_HandleT ispHandle,
                               GADI_ISP_MwbAttrT *attrPtr);

/*!
*******************************************************************************
** \brief This API is used to adjust the saturation of the video image.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[in]   value       Saturation value to be set.
**                          Possible value : [0x00 ~ 0x7F]
**                          Default value: 0x40
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
** - #GADI_ISP_ERR_FROM_DRIVER      Error on driver level.
**
** \sa
** - #gadi_isp_get_saturation
*******************************************************************************
*/
GADI_ERR gadi_isp_set_saturation(GADI_SYS_HandleT ispHandle, GADI_S32 value);

/*!
*******************************************************************************
** \brief This API is used to retrieve the saturation setting in use.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[in]   valuePtr    Pointer to retrieve the saturation value.
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
**
** \sa
** - #gadi_isp_set_saturation
*******************************************************************************
*/
GADI_ERR gadi_isp_get_saturation(GADI_SYS_HandleT ispHandle, GADI_S32 *valuePtr);

/*!
*******************************************************************************
** \brief This API is used to adjust the contrast of the video image.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[in]   attrPtr     Saturation value to be set.support auto and manual.
**                          Possible value : [0x00 ~ 0xFF]
**                          Default value: 0x80
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
** - #GADI_ISP_ERR_FROM_DRIVER      Error on driver level.
**
** \sa
** - #gadi_isp_set_contrast_attr
*******************************************************************************
*/
GADI_ERR gadi_isp_set_contrast_attr(GADI_SYS_HandleT ispHandle, GADI_ISP_ContrastAttrT *attrPtr);

/*!
*******************************************************************************
** \brief This API is used to retrieve the contrast setting in use.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[out]   attrPtr    Pointer to retrieve the saturation value.
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
**
** \sa
** - #gadi_isp_get_contrast_attr
*******************************************************************************
*/
GADI_ERR gadi_isp_get_contrast_attr(GADI_SYS_HandleT ispHandle, GADI_ISP_ContrastAttrT *attrPtr);

/*!
*******************************************************************************
** \brief This API is used to adjust the hue of the video image.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[in]   value       Saturation value to be set.
**                          Possible value : [0x00 ~ 0xFF]
**                          Default value : 0x80
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
** - #GADI_ISP_ERR_FROM_DRIVER      Error on driver level.
**
** \sa
** - #gadi_isp_get_saturation
*******************************************************************************
*/
GADI_ERR gadi_isp_set_hue(GADI_SYS_HandleT ispHandle, GADI_S32 value);

/*!
*******************************************************************************
** \brief This API is used to retrieve the hue value setting in use.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[in]   valuePtr    Pointer to retrieve the saturation value.
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
**
** \sa
** - #gadi_isp_set_saturation
*******************************************************************************
*/
GADI_ERR gadi_isp_get_hue(GADI_SYS_HandleT ispHandle, GADI_S32 *valuePtr);

/*!
*******************************************************************************
** \brief This API is used to adjust the hue of the video image.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[in]   value       Saturation value to be set.
**                          Possible value : [0x00 ~ 0xFF]
**                          Default value : 0x80
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
** - #GADI_ISP_ERR_FROM_DRIVER      Error on driver level.
**
** \sa
** - #gadi_isp_get_brightness
*******************************************************************************
*/
GADI_ERR gadi_isp_set_brightness(GADI_SYS_HandleT ispHandle, GADI_S32 value);

/*!
*******************************************************************************
** \brief This API is used to retrieve the brightness value setting in use.
**
**
** \param[in]   ispHandle   Valid ISP ADI instance handle previously opened by
**                          #gadi_isp_open.
** \param[in]   valuePtr    Pointer to retrieve the brightness value.
**
**
** \return
** - #GADI_OK                       On success.
** - #GADI_ISP_ERR_BAD_PARAMETER    Invalid parameters.
**
** \sa
** - #gadi_isp_set_brightness
*******************************************************************************
*/
GADI_ERR gadi_isp_get_brightness(GADI_SYS_HandleT ispHandle, GADI_S32 *valuePtr);

GADI_ERR gadi_isp_set_sharpen_attr(GADI_SYS_HandleT ispHandle,
                                   GADI_ISP_SharpenAttrT *attrPtr);

GADI_ERR gadi_isp_get_sharpen_attr(GADI_SYS_HandleT ispHandle,
                                   GADI_ISP_SharpenAttrT *attrPtr);

GADI_ERR gadi_isp_set_shading_attr(GADI_SYS_HandleT ispHandle,
                                   GADI_ISP_ShadingAttrT *attrPtr);

GADI_ERR gadi_isp_get_shading_attr(GADI_SYS_HandleT ispHandle,
                                   GADI_ISP_ShadingAttrT *attrPtr);

GADI_ERR gadi_isp_set_denoise_attr(GADI_SYS_HandleT ispHandle,
                                   GADI_ISP_DenoiseAttrT *attrPtr);

GADI_ERR gadi_isp_get_denoise_attr(GADI_SYS_HandleT ispHandle,
                                   GADI_ISP_DenoiseAttrT *attrPtr);

GADI_ERR gadi_isp_set_blacklevel_attr(GADI_SYS_HandleT ispHandle,
                                    GADI_ISP_BlackLevelAttrT *attrPtr);

GADI_ERR gadi_isp_get_blacklevel_attr(GADI_SYS_HandleT ispHandle,
                                    GADI_ISP_BlackLevelAttrT *attrPtr);

GADI_ERR gadi_isp_set_backlight_attr(GADI_SYS_HandleT ispHandle,
                                    GADI_ISP_BacklightAttrT *attrPtr);

GADI_ERR gadi_isp_get_backlight_attr(GADI_SYS_HandleT ispHandle,
                                    GADI_ISP_BacklightAttrT *attrPtr);

GADI_ERR gadi_isp_get_sensor_model(GADI_SYS_HandleT ispHandle,
                                   GADI_ISP_SensorModelEnumT *modelPtr);

GADI_ERR gadi_isp_tuning_start(GADI_SYS_HandleT ispHandle);

GADI_ERR gadi_isp_tuning_stop(GADI_SYS_HandleT ispHandle);

void* gadi_isp_get_sensor_bin_data(void);

GADI_U32 gadi_isp_get_sensor_id(void);

GADI_ERR gadi_isp_set_day_night_mode(GADI_U8 isDay);

GADI_ERR gadi_isp_set_vin_frame(GADI_SYS_HandleT ispHandle, GADI_S32 fps);

GADI_ERR gadi_isp_reset_3a_static(GADI_SYS_HandleT ispHandle);
GADI_U16 gadi_isp_get_ae_rgb_luma_value(void);
GADI_U16 gadi_isp_get_ae_luma_value(void);
GADI_ERR gadi_set_auto_local_exposure_mode(GADI_U32 mode);

//addedd by Hu Yin
GADI_ERR gadi_isp_set_meter_mode(GADI_U8 mode);

GADI_ERR gadi_isp_set_roi( GADI_S32* roi);
//end
#ifdef __cplusplus
}
#endif



#endif /* _ADI_ISP_H_ */

