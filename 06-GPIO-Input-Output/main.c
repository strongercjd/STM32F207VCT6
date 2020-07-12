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
#include "LCD_disp.h"
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  RCC_ClocksTypeDef  RCC_Clocks;
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  
  /*KEY1:PE14  KEY2:PE11  KEY3:PE13  KEY4:PE15*/
  /* Configure PE11 PE13 PE14 PE15 pin as input GPIO_PuPd_UP */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  
  LCD_Init();
  
  LCD_ShowString(0,16*0,"KEY1 is ");
  LCD_ShowString(0,16*2,"KEY2 is ");
  LCD_ShowString(0,16*4,"KEY3 is ");
  LCD_ShowString(0,16*6,"KEY4 is ");

  while (1)
  {
    /*KEY1*/
    if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_14)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*0,"UP  ");
      GPIO_SetBits(GPIOE,GPIO_Pin_4);  //熄灭LED灯
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*0,"Down");
      GPIO_ResetBits(GPIOE,GPIO_Pin_4);//点亮LED灯
    }
    
    /*KEY2*/
    if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_11)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*2,"UP  ");
      GPIO_SetBits(GPIOE,GPIO_Pin_4);  //熄灭LED灯
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*2,"Down");
      GPIO_ResetBits(GPIOE,GPIO_Pin_4);//点亮LED灯
    }
    
    /*KEY3*/
    if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*4,"UP  ");
      GPIO_SetBits(GPIOE,GPIO_Pin_4);  //熄灭LED灯
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*4,"Down");
      GPIO_ResetBits(GPIOE,GPIO_Pin_4);//点亮LED灯
    }
    
    /*KEY4*/
    if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_15)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*6,"UP  ");
      GPIO_SetBits(GPIOE,GPIO_Pin_4);  //熄灭LED灯
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*6,"Down");
      GPIO_ResetBits(GPIOE,GPIO_Pin_4);//点亮LED灯
    }
    
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

/*120Mhz时钟时，当ulCount为1时，函数耗时3个时钟，延时=3*1/120us=1/40us*/
/*
SystemCoreClock=120000000

us级延时,延时n微秒
SysCtlDelay(n*(SystemCoreClock/3000000));

ms级延时,延时n毫秒
SysCtlDelay(n*(SystemCoreClock/3000));

m级延时,延时n秒
SysCtlDelay(n*(SystemCoreClock/3));
*/

#if defined   (__CC_ARM) /*!< ARM Compiler */
__asm void
SysCtlDelay(unsigned long ulCount)
{
    subs    r0, #1;
    bne     SysCtlDelay;
    bx      lr;
}
#elif defined ( __ICCARM__ ) /*!< IAR Compiler */
void
SysCtlDelay(unsigned long ulCount)
{
    __asm("    subs    r0, #1\n"
       "    bne.n   SysCtlDelay\n"
       "    bx      lr");
}

#elif defined (__GNUC__) /*!< GNU Compiler */
void __attribute__((naked))
SysCtlDelay(unsigned long ulCount)
{
    __asm("    subs    r0, #1\n"
       "    bne     SysCtlDelay\n"
       "    bx      lr");
}

#elif defined  (__TASKING__) /*!< TASKING Compiler */                           
/*无*/
#endif /* __CC_ARM */

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
