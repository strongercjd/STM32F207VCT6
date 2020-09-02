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

__IO uint16_t IC3ReadValue1 = 0, IC3ReadValue2 = 0;
__IO uint16_t CaptureNumber = 0;
__IO uint32_t Capture = 0;
__IO uint32_t TIM1Freq = 0;

extern Capture_TypeDef  Capture_Structure;

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

void TIM1_UP_TIM10_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET){//溢出
    if(Capture_Structure.Capture_OK == 0){//还没有捕获成功
      if(Capture_Structure.Capture_Falling){//已经捕获到低电平
	if(Capture_Structure.Capture_Overflow_Time == 0XFFFF){//超出测量范围
	  Capture_Structure.Capture_OK = 1;
	  Capture_Structure.Capture_Value = 0XFFFF;
	}else{
	  Capture_Structure.Capture_Overflow_Time++;
	}
      }
    }	
  }
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update); //清除中断标志位
}

/**
* @brief  This function handles TIM1 global interrupt request.
* @param  None
* @retval None
*/
void TIM1_CC_IRQHandler(void)
{ 
  if(Capture_Structure.Capture_OK == 0){//还没有捕获成功
    
    if(TIM_GetITStatus(TIM1, TIM_IT_CC2) == SET) //捕获2发生捕获事件
    {	
      if(Capture_Structure.Capture_Falling)			
      {	  			
	Capture_Structure.Capture_OK = 1;		
	Capture_Structure.Capture_Value=TIM_GetCapture2(TIM1);//获取当前的捕获值.
	TIM_OC2PolarityConfig(TIM1,TIM_ICPolarity_Falling);
      }else{ //还未开始,第一次捕获下降沿
	Capture_Structure.Capture_OK = 0;
	Capture_Structure.Capture_Overflow_Time = 0;
	Capture_Structure.Capture_Value = 0;
	Capture_Structure.Capture_Falling = 1;		//标记捕获到了下降沿
	TIM_Cmd(TIM1,DISABLE ); 
	TIM_SetCounter(TIM1,0);
	TIM_OC2PolarityConfig(TIM1,TIM_ICPolarity_Rising);		//设置为上升沿捕获
	TIM_Cmd(TIM1,ENABLE );
      }		    
    }
    
  }
  TIM_ClearITPendingBit(TIM1, TIM_IT_CC2); //清除中断标志位
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
