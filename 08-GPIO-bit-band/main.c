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



/*LED配置---PE4*/
#define  GPIO_IDR_OFFSET  (GPIOE_BASE+0x10 - PERIPH_BASE)
#define  GPIO_ODR_OFFSET  (GPIOE_BASE+0x14 - PERIPH_BASE)

#define  GPIO_BitNumber	 4
#define  GPIO_OUT_BB		 (PERIPH_BB_BASE + (GPIO_ODR_OFFSET * 32) + (GPIO_BitNumber * 4))    
#define  GPIO_OUT_DATA	         *(__IO uint32_t *)GPIO_OUT_BB

#define  GPIO_IN_BB		 (PERIPH_BB_BASE + (GPIO_IDR_OFFSET * 32) + (GPIO_BitNumber * 4))    
#define  GPIO_IN_DATA	         *(__IO uint32_t *)GPIO_IN_BB

#define  GPIO_DIR_REG		 *(__IO uint32_t *)(GPIOE_BASE+0X00)


#define PE4_SET()  GPIO_OUT_DATA = 1   //!< IO写1
#define PE4_CLR()  GPIO_OUT_DATA = 0   //!< IO写0
#define PE4_OUT()  GPIO_DIR_REG = (((GPIO_DIR_REG) & 0xFFFFFCFF) | 0x00000100)  //!IO输出
#define PE4_IN()   GPIO_DIR_REG = ((GPIO_DIR_REG) & 0xFFFFFCFF)  //!IO输入
#define PE4_READ() GPIO_IN_DATA   //!<IO读入


/*GPIO配置---PXX*/
#define GPIO_SET(GPIOx_BASE,GPIO_BitNumber)   *(__IO uint32_t *)((PERIPH_BB_BASE + ((GPIOx_BASE+0x14 - PERIPH_BASE) * 32) + (GPIO_BitNumber * 4))) = 1   //!< IO写1
#define GPIO_CLR(GPIOx_BASE,GPIO_BitNumber)   *(__IO uint32_t *)((PERIPH_BB_BASE + ((GPIOx_BASE+0x14 - PERIPH_BASE) * 32) + (GPIO_BitNumber * 4))) = 0   //!< IO写0

#define GPIO_OUT(GPIOx_BASE,GPIO_BitNumber)   *(__IO uint32_t *)(GPIOx_BASE+0X00)  &= ~(GPIO_MODER_MODER0 << (GPIO_BitNumber * 2));\
                                              *(__IO uint32_t *)(GPIOx_BASE+0X00) |= ((GPIO_Mode_OUT) << (GPIO_BitNumber * 2)); //!IO输出

#define GPIO_IN(GPIOx_BASE,GPIO_BitNumber)    *(__IO uint32_t *)(GPIOx_BASE+0X00)  &= ~(GPIO_MODER_MODER0 << (GPIO_BitNumber * 2));\
                                              *(__IO uint32_t *)(GPIOx_BASE+0X00) |= ((GPIO_Mode_IN) << (GPIO_BitNumber * 2));  //!IO输入
                                              
#define GPIO_READ(GPIOx_BASE,GPIO_BitNumber)  *(__IO uint32_t *)((PERIPH_BB_BASE + ((GPIOx_BASE+0x10 - PERIPH_BASE) * 32) + (GPIO_BitNumber * 4)))   //!<IO读入


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
  
  GPIO_OUT(GPIOE_BASE,4);
  
  GPIO_IN(GPIOE_BASE,14);
  GPIO_IN(GPIOE_BASE,11);
  GPIO_IN(GPIOE_BASE,13);
  GPIO_IN(GPIOE_BASE,15);

  while (1)
  {
    /*KEY1*/
    if(GPIO_READ(GPIOE_BASE,14)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*0,"UP  ");
      GPIO_SET(GPIOE_BASE,4);
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*0,"Down");
      GPIO_CLR(GPIOE_BASE,4);
    }
    
    /*KEY2*/
    if(GPIO_READ(GPIOE_BASE,11)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*2,"UP  ");
      GPIO_SET(GPIOE_BASE,4);
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*2,"Down");
      GPIO_CLR(GPIOE_BASE,4);
    }
    
    /*KEY3*/
    if(GPIO_READ(GPIOE_BASE,13)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*4,"UP  ");
      GPIO_SET(GPIOE_BASE,4);
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*4,"Down");
      GPIO_CLR(GPIOE_BASE,4);
    }
    
    /*KEY4*/
    if(GPIO_READ(GPIOE_BASE,15)){
      Set_Pen_Color(LCD_grenn);
      LCD_ShowString(8*8,16*6,"UP  ");
      GPIO_SET(GPIOE_BASE,4);
    }else{
      Set_Pen_Color(LCD_yellow);
      LCD_ShowString(8*8,16*6,"Down");
      GPIO_CLR(GPIOE_BASE,4);
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
