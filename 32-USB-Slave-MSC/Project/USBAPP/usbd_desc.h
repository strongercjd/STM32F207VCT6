/**
  ******************************************************************************
  * @file    usbd_desc.h
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   header file for the usbd_desc.c file
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

#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_req.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USB_DESC
  * @brief general defines for the usb device library file
  * @{
  */ 

/** @defgroup USB_DESC_Exported_Defines
  * @{
  */
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05
#define USB_SIZ_DEVICE_DESC                     18
#define USB_SIZ_STRING_LANGID                   4

#if !defined (USE_STM3210C_EVAL)
#define         DEVICE_ID1          (0x1FFF7A10)
#define         DEVICE_ID2          (0x1FFF7A14)
#define         DEVICE_ID3          (0x1FFF7A18)

#else
#define         DEVICE_ID1          (0x1FFFF7E8)
#define         DEVICE_ID2          (0x1FFFF7EA)
#define         DEVICE_ID3          (0x1FFFF7EC)  
#endif

#define  USB_SIZ_STRING_SERIAL       0x1A
/**
  * @}
  */ 


/** @defgroup USBD_DESC_Exported_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 



/** @defgroup USBD_DESC_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_Variables
  * @{
  */ 
extern  uint8_t USBD_DeviceDesc  [USB_SIZ_DEVICE_DESC];
extern  uint8_t USBD_StrDesc[USB_MAX_STR_DESC_SIZ];
extern  uint8_t USBD_OtherSpeedCfgDesc[USB_LEN_CFG_DESC]; 
extern  uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC];
extern  uint8_t USBD_LangIDDesc[USB_SIZ_STRING_LANGID];
extern  USBD_DEVICE USR_desc; 
/**
  * @}
  */ 

/** @defgroup USBD_DESC_Exported_FunctionsPrototype
  * @{
  */ 


uint8_t *     USBD_USR_DeviceDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USBD_USR_LangIDStrDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USBD_USR_ManufacturerStrDescriptor ( uint8_t speed , uint16_t *length);
uint8_t *     USBD_USR_ProductStrDescriptor ( uint8_t speed , uint16_t *length);
uint8_t *     USBD_USR_SerialStrDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USBD_USR_ConfigStrDescriptor( uint8_t speed , uint16_t *length);
uint8_t *     USBD_USR_InterfaceStrDescriptor( uint8_t speed , uint16_t *length);

#ifdef USB_SUPPORT_USER_STRING_DESC
uint8_t *     USBD_USR_USRStringDesc (uint8_t speed, uint8_t idx , uint16_t *length);  
#endif /* USB_SUPPORT_USER_STRING_DESC */  
  
/**
  * @}
  */ 

#endif /* __USBD_DESC_H */

/**
  * @}
  */ 

/**
* @}
*/ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
