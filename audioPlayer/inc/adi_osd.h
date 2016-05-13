/*!
*****************************************************************************
** \file        ./adi/inc/adi_osd.h
**
** \brief       adi osd porting.
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2013-2014 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ADI_OSD_H_
#define _ADI_OSD_H_

#include "stdio.h"
#include "adi_types.h"


//*****************************************************************************
//*****************************************************************************
//** Defines and Macros
//*****************************************************************************
//*****************************************************************************
#define OSD_PLANE_NUM           (4)
#define OSD_AREA_NUM            (3)/*the number of areas of one plane.*/


/*
*******************************************************************************
** Defines for general error codes of the module.
*******************************************************************************
*/
/*! Bad parameter passed. */
#define GADI_OSD_ERR_BAD_PARAMETER                                          \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_BAD_PARAMETER)
/*! Memory allocation failed. */
#define GADI_OSD_ERR_OUT_OF_MEMORY                                          \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_OUT_OF_MEMORY)
/*! Device already initialised. */
#define GADI_OSD_ERR_ALREADY_INITIALIZED                                    \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_ALREADY_INITIALIZED)
/*! Device not initialised. */
#define GADI_OSD_ERR_NOT_INITIALIZED                                        \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_NOT_INITIALIZED)
/*! Feature or function is not available. */
#define GADI_OSD_ERR_FEATURE_NOT_SUPPORTED                                  \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_FEATURE_NOT_SUPPORTED)
/*! Timeout occured. */
#define GADI_OSD_ERR_TIMEOUT                                                \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_TIMEOUT)
/*! The device is busy, try again later. */
#define GADI_OSD_ERR_DEVICE_BUSY                                            \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_DEVICE_BUSY)
/*! Invalid handle was passed. */
#define GADI_OSD_ERR_INVALID_HANDLE                                         \
                              (GADI_OSD_MODULE_BASE + GADI_ERR_INVALID_HANDLE)
/*! Semaphore could not be created. */
#define GADI_OSD_ERR_SEMAPHORE_CREATE                                       \
                               (GADI_OSD_MODULE_BASE + GADI_ERR_SEMAPHORE_CREATE)
/*! The driver's used version is not supported. */
#define GADI_OSD_ERR_UNSUPPORTED_VERSION                                    \
                               (GADI_OSD_MODULE_BASE + GADI_ERR_UNSUPPORTED_VERSION)
/*! The driver's used version is not supported. */
#define GADI_OSD_ERR_FROM_DRIVER                                            \
                               (GADI_OSD_MODULE_BASE + GADI_ERR_FROM_DRIVER)
/*! The device/handle is not open.. */
#define GADI_OSD_ERR_NOT_OPEN                                               \
                               (GADI_OSD_MODULE_BASE + GADI_ERR_NOT_OPEN)


//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************
/*!
*******************************************************************************
** \brief the parameters device of osd to display.
*******************************************************************************
*/
typedef enum
{
    GADI_OSD_ON_UNKOWN  = 0,
    GADI_OSD_ON_CVBS    = 1,
    GADI_OSD_ON_STREAM  = 2,
}GADI_OSD_DevicesE;



//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************


/*!
*******************************************************************************
** \brief the struct of osd plane's area.
*******************************************************************************
*/
typedef struct
{
    /*the index of plane(0~3), each stream has only one plane,
    so plane index means stream index.*/
    GADI_U8         planeId;
    /*the index of area(0~2), each plane has 3 areas.*/
    GADI_U8         areaId;
}GADI_OSD_AreaIndexT;

/*!
*******************************************************************************
** \brief the mapping address informations struct of osd plane's area.
*******************************************************************************
*/
typedef struct
{
    /*the index of area.*/
    GADI_U8         areaId;
    /*yuv colour look-up table start address.*/
    GADI_U8         *clutStartAddr;
    /*number of bytes of colour look-up table.*/
    GADI_U32        clutSize;
    /*plane area start address.*/
    GADI_U8         *areaStartAddr;
    /*number of bytes of area.*/
    GADI_U32        areaSize;
}GADI_OSD_AreaMappingT;

/*!
*******************************************************************************
** \brief the parameters struct of osd plane's area.
*******************************************************************************
*/
typedef struct
{
    /*the index of plane(0~3).*/
    GADI_U8         planeId;
    /*the index of area(0~2).*/
    GADI_U8         areaId;
    /*the enable/disable flag of area(0:disable, 1:enable).*/
    GADI_U8         enable;
    /*area width.*/
    GADI_U16        width;
    /*area height.*/
    GADI_U16        height;
    /*area x offset.*/
    GADI_U16        offsetX;
    /*area y offset.*/
    GADI_U16        offsetY;
}GADI_OSD_AreaParamsT;

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
** \brief Initialize the ADI osd module.
**
** \return
** - #GADI_OK
** - #GADI_OSD_ERR_OUT_OF_MEMORY
** - #GADI_OSD_ERR_ALREADY_INITIALIZED
**
** \sa gadi_osd_exit
**
*******************************************************************************
*/
GADI_ERR gadi_osd_init(void);

/*!
*******************************************************************************
** \brief Shutdown the ADI osd module.
**
** \return
** - #GADI_OK
** - #GADI_OSD_ERR_NOT_INITIALIZED
**
** \sa gadi_osd_init
**
*******************************************************************************
*/
GADI_ERR gadi_osd_exit(void);

/*!
*******************************************************************************
** \brief open the ADI osd module.
**
** \param[in] errorCodePtr pointer to return the error code.
**
** \return Return an valid handle of ADI osd module instance.
**
** \sa gadi_osd_close
**
*******************************************************************************
*/
GADI_SYS_HandleT gadi_osd_open(GADI_ERR* errorCodePtr);

/*!
*******************************************************************************
** \brief switch the ADI osd module.
**
** \param[in]  handle     Valid ADI osd instance handle previously opened by
**                        #gadi_osd_open.
** \param[in]  device     Valid GADI_OSD_DevicesE, the osd witch device to show .
**
** \return
** - #GADI_OK
** - #GADI_OSD_ERR_BAD_PARAMETER
**
** \sa gadi_osd_open
**
*******************************************************************************
*/
GADI_ERR gadi_osd_switch(GADI_SYS_HandleT handle, GADI_OSD_DevicesE device);

/*!
*******************************************************************************
** \brief close one ADI osd module instance.
**
** \param[in]  handle     Valid ADI osd instance handle previously opened by
**                        #gadi_osd_open.
**
** \return
** - #GADI_OK
** - #GADI_OSD_ERR_BAD_PARAMETER
**
** \sa gadi_osd_open
**
*******************************************************************************
*/
GADI_ERR gadi_osd_close(GADI_SYS_HandleT handle);

/*!
*******************************************************************************
** \brief get area mapping address information of osd plane.
**
** \param[in]  handle     Valid ADI osd instance handle previously opened by
**                        #gadi_osd_open.
** \param[in]  areaIndex  struct of area index, include plane id & area id.
** \param[out] areaMapping  pointer of area mapping struct.
**
** \return
** - #GADI_OK
** - #GADI_OSD_ERR_BAD_PARAMETER
**
** \sa gadi_osd_open
**
*******************************************************************************
*/
GADI_ERR gadi_osd_get_area_mapping(GADI_SYS_HandleT handle,
                                            GADI_OSD_AreaIndexT areaIndex,
                                            GADI_OSD_AreaMappingT *areaMapping);

/*!
*******************************************************************************
** \brief set area parameters of osd plane.
**
** \param[in]  handle     Valid ADI osd instance handle previously opened by
**                        #gadi_osd_open.
** \param[in] areaParams  pointer of area parameters struct.
**
** \return
** - #GADI_OK
** - #GADI_OSD_ERR_FROM_DRIVER
** - #GADI_OSD_ERR_BAD_PARAMETER
**
** \sa gadi_osd_get_area_params
**
*******************************************************************************
*/
GADI_ERR gadi_osd_set_area_params(GADI_SYS_HandleT
                                      handle, GADI_OSD_AreaParamsT *areaParams);

/*!
*******************************************************************************
** \brief set area parameters of osd plane.
**
** \param[in]  handle     Valid ADI osd instance handle previously opened by
**                        #gadi_osd_open.
** \param[in/out] areaParams  pointer of area parameters struct, must give out
**                            plane id & area id.
**
** \return
** - #GADI_OK
** - #GADI_OSD_ERR_FROM_DRIVER
** - #GADI_OSD_ERR_BAD_PARAMETER
**
** \sa gadi_osd_set_area_params
**
*******************************************************************************
*/
GADI_ERR gadi_osd_get_area_params(GADI_SYS_HandleT handle,
                                               GADI_OSD_AreaParamsT *areaParams);


#ifdef __cplusplus
    }
#endif


#endif /* _ADI_OSD_H_ */
