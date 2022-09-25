/**
  ******************************************************************************
  * @file    Project/STM32F2xx_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body
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
#include "main.h"
    #include "software_task.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

RCC_ClocksTypeDef RCC_Clocks;
uint32_t Systick_Count_ms = 0;
uint8_t open_led(uint8_t task_id, uint16_t events, void *p)
{
  GPIO_ResetBits(GPIOE,GPIO_Pin_4);
  return 0;
}
uint8_t close_led(uint8_t task_id, uint16_t events, void *p)
{
  GPIO_SetBits(GPIOE,GPIO_Pin_4);
  return 0;
}
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
float time_ms;
int main(void)
{
  GPIO_InitTypeDef  GPIO_Init_s;
  
  /* 配置SysTick为1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
  /* 使能GPIOE端口时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  /* 配置LED管脚 */
  GPIO_Init_s.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init_s.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init_s.GPIO_OType = GPIO_OType_PP;
  GPIO_Init_s.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_s.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_Init_s);
  
  SW_task_create(0,open_led,500);
  SW_task_create(1,close_led,1000);
  SW_task_start_scheduler();
  while (1)
  {
    SW_task_scheduler();
  }
}



/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
