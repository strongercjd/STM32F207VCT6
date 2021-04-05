/**
  ******************************************************************************
  * @file    stm32f2x7_eth_bsp.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011 
  * @brief   Header for stm32f2x7_eth_bsp.c file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F2x7_ETH_BSP_H
#define __STM32F2x7_ETH_BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm322xg_eval.h"
//#include "stm322xg_eval_lcd.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define REL8201E_PHY_ADDRESS       0x00 /* Relative to STM322xG-EVAL Board */

/* Specific defines for EXTI line, used to manage Ethernet link status */
#define ETH_LINK_EXTI_LINE             EXTI_Line14
#define ETH_LINK_EXTI_PORT_SOURCE      EXTI_PortSourceGPIOB
#define ETH_LINK_EXTI_PIN_SOURCE       EXTI_PinSource14
#define ETH_LINK_EXTI_IRQn             EXTI15_10_IRQn 
/* PB14 */
#define ETH_LINK_PIN                   GPIO_Pin_14
#define ETH_LINK_GPIO_PORT             GPIOB
#define ETH_LINK_GPIO_CLK              RCC_AHB1Periph_GPIOB
/* PHY registers */
#define PHY_MICR                  0x11 /* MII Interrupt Control Register */
#define PHY_MICR_INT_EN           ((uint16_t)0x0002) /* PHY Enable interrupts */
#define PHY_MICR_INT_OE           ((uint16_t)0x0001) /* PHY Enable output interrupt events */
#define PHY_MISR                  0x12 /* MII Interrupt Status and Misc. Control Register */
#define PHY_MISR_LINK_INT_EN      ((uint16_t)0x0020) /* Enable Interrupt on change of link status */
#define PHY_LINK_STATUS           ((uint16_t)0x2000) /* PHY link status interrupt mask */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void  ETH_BSP_Config(void);
void ETH_MACDMA_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F2x7_ETH_BSP_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
