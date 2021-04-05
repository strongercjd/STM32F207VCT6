/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    17-March-2018
  * @brief   This file is responsible to offer board support package and is
  *          configurable by user.
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
#include "usb_bsp.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
* @{
*/

/** @defgroup USB_BSP
* @brief This file is responsible to offer board support package
* @{
*/

/** @defgroup USB_BSP_Private_Defines
* @{
*/
/**
* @}
*/


/** @defgroup USB_BSP_Private_TypesDefinitions
* @{
*/
/**
* @}
*/





/** @defgroup USB_BSP_Private_Macros
* @{
*/
/**
* @}
*/

/** @defgroup USBH_BSP_Private_Variables
* @{
*/

/**
* @}
*/

/** @defgroup USBH_BSP_Private_FunctionPrototypes
* @{
*/
/**
* @}
*/

/** @defgroup USB_BSP_Private_Functions
* @{
*/




/**
* @brief  USB_OTG_BSP_Init
*         Initializes BSP configurations
* @param  None
* @retval None
*/

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE * pdev)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  /* Configure SOF ID DM DP Pins */
  GPIO_InitStructure.GPIO_Pin = /*GPIO_Pin_8 |*/ GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_OTG1_FS);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_OTG1_FS);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_OTG1_FS);

//  /* Configure VBUS Pin */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//  /* Configure ID pin */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_OTG1_FS);


  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);

}

/**
* @brief  USB_OTG_BSP_EnableInterrupt
*         Enable USB Global interrupt
* @param  None
* @retval None
*/
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE * pdev)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
* @brief  USB_OTG_BSP_uDelay
*         This function provides delay time in micro sec
* @param  usec : Value of delay required in micro sec
* @retval None
*/
#if defined (USE_STM322xG_EVAL)
     /* This value is set for SYSCLK = 120 MHZ, User can adjust this value
      * depending on used SYSCLK frequency */
#define count_us   40

#elif defined(USE_STM324xG_EVAL) || defined(USE_STM324x9I_EVAL)
    /* This value is set for SYSCLK = 168 MHZ, User can adjust this value
     * depending on used SYSCLK frequency */
#define count_us   55

#else                           /* defined (USE_STM3210C_EVAL) */
    /* This value is set for SYSCLK = 72 MHZ, User can adjust this value
     * depending on used SYSCLK frequency */
#define count_us   12
#endif
void USB_OTG_BSP_uDelay(const uint32_t usec)
{
  SysCtlDelay(usec*(SystemCoreClock/3000000));
}


/**
* @brief  USB_OTG_BSP_mDelay
*          This function provides delay time in milli sec
* @param  msec : Value of delay required in milli sec
* @retval None
*/
void USB_OTG_BSP_mDelay(const uint32_t msec)
{
  USB_OTG_BSP_uDelay(msec * 1000);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
