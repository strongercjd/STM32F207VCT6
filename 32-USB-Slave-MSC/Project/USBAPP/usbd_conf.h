/**
  ******************************************************************************
  * @file    usbd_conf.h
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   USB Device configuration file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      <http://www.st.com/SLA0044>
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_CONF
  * @brief This file is the device library configuration file
  * @{
  */ 

/** @defgroup USBD_CONF_Exported_Defines
  * @{
  */ 


#define USBD_CFG_MAX_NUM           1
#define USBD_ITF_MAX_NUM           1
#define USB_MAX_STR_DESC_SIZ       64 

#define USBD_SELF_POWERED               

/* Class Layer Parameter */

#define MSC_IN_EP                    0x81
#define MSC_OUT_EP                   0x01
#ifdef USE_USB_OTG_HS  
#ifdef USE_ULPI_PHY
#define MSC_MAX_PACKET               512
#else
#define MSC_MAX_PACKET               64
#endif
#else  /*USE_USB_OTG_FS*/
#define MSC_MAX_PACKET                64
#endif


#define MSC_MEDIA_PACKET             4096

/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 

#endif //__USBD_CONF__H__

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

