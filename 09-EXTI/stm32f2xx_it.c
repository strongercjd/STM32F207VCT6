/**
  ******************************************************************************
  * @file    Project/STM32F2xx_StdPeriph_Template/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_it.h"
#include "main.h"
#include "LCD_disp.h"
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}
/***
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  /*KEY2:PE11*/
  if(EXTI_GetITStatus(EXTI_Line11) != RESET)
  {
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*2,"UP  ");
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*2,"Down");
    }
    /* Clear the EXTI line 11 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line11);
  }
  /*EY3:PE13*/
  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*4,"UP  ");
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*4,"Down");
    }
    /* Clear the EXTI line 13 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line13);
  }
  /*KEY1:PE14*/
  if(EXTI_GetITStatus(EXTI_Line14) != RESET)
  {
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*0,"UP  ");
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*0,"Down");
    }
    /* Clear the EXTI line 14 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line14);
  }
  /*KEY4:PE15*/
  if(EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*6,"UP  ");
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*6,"Down");
    }
    /* Clear the EXTI line 15 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line15);
  }
}
/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
