/**
  ******************************************************************************
  * @file    usbd_usr.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   This file includes the user application layer
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

/* Includes ------------------------------------------------------------------ */
#include "usbd_usr.h"
#include "usb_bsp.h"
/** @addtogroup USBD_USER
  * @{
  */

//表示USB连接状态
//0,没有连接;
//1,已经连接;
uint8_t bDeviceState=0;		//默认没有连接  


/** @defgroup USBD_USR_Private_Variables
  * @{
  */
/* Points to the DEVICE_PROP structure of current device */
/* The purpose of this register is to speed up the execution */

USBD_Usr_cb_TypeDef USR_cb = {
  USBD_USR_Init,
  USBD_USR_DeviceReset,
  USBD_USR_DeviceConfigured,
  USBD_USR_DeviceSuspended,
  USBD_USR_DeviceResumed,

  USBD_USR_DeviceConnected,
  USBD_USR_DeviceDisconnected,
};

/**
  * @}
  */

/**
  * @}
  */



/** @defgroup USBD_USR_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_USR_Private_Functions
  * @{
  */

/**
* @brief  Displays the message on LCD on device lib initialization
* @param  None
* @retval None
*/
void USBD_USR_Init(void)
{
  printf("USBD_USR_Init\r\n");
}

/**
* @brief  Displays the message on LCD on device reset event
* @param  speed : device speed
* @retval None
*/
void USBD_USR_DeviceReset(uint8_t speed)
{
  switch (speed)
  {
  case USB_OTG_SPEED_HIGH:
    printf("USB Device Library v1.1.0  [HS]\r\n");
    break; 
  case USB_OTG_SPEED_FULL: 
    printf("USB Device Library v1.1.0  [FS]\r\n");
    break;
  default:
    printf("USB Device Library v1.1.0  [??]\r\n"); 
    break;
  }
}


/**
* @brief  Displays the message on LCD on device config event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConfigured(void)
{
  printf("MSC Interface started.\r\n");
}

/**
* @brief  Displays the message on LCD on device suspend event 
* @param  None
* @retval None
*/
void USBD_USR_DeviceSuspended(void)
{
  printf("Device In suspend mode.\r\n");
}


/**
* @brief  Displays the message on LCD on device resume event
* @param  None
* @retval None
*/
void USBD_USR_DeviceResumed(void)
{
  printf("Device Resumed\r\n");
}

/**
* @brief  USBD_USR_DeviceConnected
*         Displays the message on LCD on device connection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceConnected(void)
{
  bDeviceState=1;
  printf("USB Device Connected.\r\n");
}


/**
* @brief  USBD_USR_DeviceDisonnected
*         Displays the message on LCD on device disconnection Event
* @param  None
* @retval Status
*/
void USBD_USR_DeviceDisconnected(void)
{
  bDeviceState=0;
  printf("USB Device Disconnected.\r\n");
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
