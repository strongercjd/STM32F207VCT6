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
#include "i2c_io.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

#define	BCD_to_HEC(b)	(((b>>4)*10)+(b&0x0f))
#define	HEC_to_BCD(h)	(((h/10)<<4)|(h%10))

typedef struct{
  uint8_t  second;
  uint8_t  minute;
  uint8_t  hour;
  uint8_t  date;
  uint8_t  month;
  uint8_t  week;
  uint16_t year;
}Struct_TimeType;

Struct_TimeType time_G;

/* Private function prototypes -----------------1------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
* @brief  Main program.
* @param  None
* @retval None
*/
int main(void)
{
  
  uint8_t G_time_temp[7];
  uint8_t G_time_set[7];
  char str[200];
  
  RCC_ClocksTypeDef RCC_Clocks;
  GPIO_InitTypeDef  GPIO_Init_s;
  
  /* SysTick end of count event each 1ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
  
  /* Enable the GPIO_LED Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  /* Configure the GPIO_LED pin */
  GPIO_Init_s.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init_s.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init_s.GPIO_OType = GPIO_OType_PP;
  GPIO_Init_s.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_s.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_Init_s);
  
  LCD_Init();
  
  I2C_IO_init();
  
  G_time_set[0] = HEC_to_BCD(10);//second
  G_time_set[1] = HEC_to_BCD(10);//minute
  G_time_set[2] = HEC_to_BCD(10);//hour
  G_time_set[3] = HEC_to_BCD(25);//date
  G_time_set[4] = HEC_to_BCD(5);//week
  G_time_set[5] = HEC_to_BCD(10)&0x7F;//month   
  G_time_set[6] = HEC_to_BCD(19);//year
  
  I2C_Write_nbyte(0xA2, 0x02,G_time_set, 7);
  
  LCD_ShowString(0,0,"initial time is 2019-10-25 10:10:10");
  
  Set_Pen_Color(LCD_blue);
  while (1)
  {
    /* Insert 1s delay */
    Delay(1000);
    
    if(!(I2C_Read_nbyte(0xa2, 0x02,G_time_temp,7))){
      GPIO_ResetBits(GPIOE,GPIO_Pin_4);
      Delay(200);
    }
    
    time_G.second = BCD_to_HEC((G_time_temp[0]&0x7f));
    time_G.minute = BCD_to_HEC((G_time_temp[1]&0x7f));
    time_G.hour = BCD_to_HEC((G_time_temp[2]&0x3f));
    time_G.date = BCD_to_HEC((G_time_temp[3]&0x3f));
    time_G.week = BCD_to_HEC((G_time_temp[4]&0x07));
    
    if(time_G.week==0){
      time_G.week=7;
    }
    time_G.month = BCD_to_HEC((G_time_temp[5]&0x1f));
    time_G.year = BCD_to_HEC(G_time_temp[6]);
    
    if(G_time_temp[5]&0x80){
      time_G.year+=1900;  
    }
    else{
      time_G.year+=2000; 
    }
    sprintf(str,"now time is %d-%d-%d %d:%d:%d",time_G.year,time_G.month,time_G.date,time_G.hour,time_G.minute,time_G.second);
    LCD_ShowString(0,16*3,(char*)str);
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
