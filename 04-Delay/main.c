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

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


void delay_ms(uint16_t nms);
void delay_us(uint32_t nus);

RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  GPIO_InitTypeDef  GPIO_Init_s;
  
  RCC_GetClocksFreq(&RCC_Clocks);
  
  /* 使能GPIOE端口时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  /* 配置LED管脚 */
  GPIO_Init_s.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init_s.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init_s.GPIO_OType = GPIO_OType_PP;
  GPIO_Init_s.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_s.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_Init_s);


  while (1)
  {
    GPIO_SetBits(GPIOE,GPIO_Pin_4);  //熄灭LED灯                     
    delay_ms(500);//延时500ms
    GPIO_ResetBits(GPIOE,GPIO_Pin_4);//点亮LED灯                     
    delay_ms(500);//延时500ms
  }
}

void delay_us(uint32_t nus)
{
  uint32_t temp;
  SysTick->LOAD = RCC_Clocks.HCLK_Frequency/1000000/8*nus;
  SysTick->VAL=0X00;//清空计数器
  SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
  do
  {
    temp=SysTick->CTRL;//读取当前倒计数值
  }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
  SysTick->CTRL=0x00; //关闭计数器
  SysTick->VAL =0X00; //清空计数器
}
void delay_ms(uint16_t nms)
{
  uint32_t temp;
  SysTick->LOAD = RCC_Clocks.HCLK_Frequency/1000/8*nms;
  SysTick->VAL=0X00;//清空计数器
  SysTick->CTRL=0X01;//使能，减到零是无动作，采用外部时钟源
  do
  {
    temp=SysTick->CTRL;//读取当前倒计数值
  }while((temp&0x01)&&(!(temp&(1<<16))));//等待时间到达
  SysTick->CTRL=0x00; //关闭计数器
  SysTick->VAL =0X00; //清空计数器
}



/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
