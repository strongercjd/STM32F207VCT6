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
#include "stm32f2x_sdio_sd.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

SD_Error Status = SD_OK;
#define BLOCK_SIZE            512 /* Block Size in Bytes */
#define NUMBER_OF_BLOCKS      100  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)

uint8_t Buffer_MultiBlock[MULTI_BUFFER_SIZE];

void NVIC_Configuration(void);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t line = 0;
  uint32_t num = 0;
  RCC_ClocksTypeDef RCC_Clocks;
  GPIO_InitTypeDef  GPIO_Init_s;
  char LCD_string[200];
  
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
  
  GPIO_SetBits(GPIOE,GPIO_Pin_4);//熄灭LED灯
  
  /* Interrupt Config */
  NVIC_Configuration();
  
  LCD_Init();
  
  if((Status = SD_Init()) != SD_OK)
  {
    LCD_ShowString(0,16*(line++),"SD is ERR");
  }
  else
  {
    LCD_ShowString(0,16*(line++),"SD is OK");
    sprintf(LCD_string,"SD Total Size:%dMbye \r\n",SDCardInfo.CardCapacity/1024/1024);
    LCD_ShowString(0,16*(line++),(uint8_t*)LCD_string);
    sprintf(LCD_string,"SD Block Size:%d byte \r\n",SDCardInfo.CardBlockSize);
    LCD_ShowString(0,16*(line++),(uint8_t*)LCD_string);
  }
  
  /*擦除数据测试*/
  if (Status == SD_OK)
  {
    Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS));//擦除数据
  }
  if (Status == SD_OK)
  {
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    
    /* Wait until end of DMA transfer */
    while(SD_GetStatus() != SD_TRANSFER_OK);
    
    for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
    {
      /* In some SD Cards the erased state is 0xFF, in others it's 0x00 */
      if ((Buffer_MultiBlock[num] != 0xFF) && (Buffer_MultiBlock[num] != 0x00))
      {
        break;
      }
    }
    if(num == BLOCK_SIZE * NUMBER_OF_BLOCKS)
    {
      LCD_ShowString(0,16*(line++),"SD Erase Test is OK");
    }
    else
    {
      LCD_ShowString(0,16*(line++),"SD Erase Test is ERR");
    }
  }
  
  /*单块数据读写测试*/
  for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
  {
    Buffer_MultiBlock[num] = 0xAA;
  }
  if (Status == SD_OK)
  {
    /* Write block of 512 bytes on address 0 */
    Status = SD_WriteBlock(Buffer_MultiBlock, 0x00, BLOCK_SIZE);
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }
  
  for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
  {
    Buffer_MultiBlock[num] = 0x00;
  }
  if (Status == SD_OK)
  {
    /* Read block of 512 bytes from address 0 */
    Status = SD_ReadBlock(Buffer_MultiBlock, 0x00, BLOCK_SIZE);
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    
    for(num=0;num<BLOCK_SIZE;num++)
    {
      if (Buffer_MultiBlock[num] != 0xAA)
      {
        break;
      }
    }
    if(num == BLOCK_SIZE )
    {
      LCD_ShowString(0,16*(line++),"SD Single Block Test is OK");
    }
    else
    {
      LCD_ShowString(0,16*(line++),"SD Single Block Test is ERR");
    }
  }
  
  /*多块数据读写测试*/
  for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
  {
    Buffer_MultiBlock[num] = 0xAA;
  }
  if (Status == SD_OK)
  {
    /* Write multiple block of many bytes on address 0 */
    Status = SD_WriteMultiBlocks(Buffer_MultiBlock, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }
  
  for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
  {
    Buffer_MultiBlock[num] = 0x00;
  }
  if (Status == SD_OK)
  {
    /* Read block of many bytes from address 0 */
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
    
    for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
    {
      if (Buffer_MultiBlock[num] != 0xAA)
      {
        break;
      }
    }
    if(num == (BLOCK_SIZE * NUMBER_OF_BLOCKS) )
    {
      LCD_ShowString(0,16*(line++),"SD Multi Block Test is OK");
    }
    else
    {
      LCD_ShowString(0,16*(line++),"SD Multi Block Test is ERR");
    }
  }
  
  /*测试读写接口*/
  for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
  {
    Buffer_MultiBlock[num] = 0xAA;
  }
  SD_WriteDisk(Buffer_MultiBlock,10,NUMBER_OF_BLOCKS);
  for(num=0;num<(BLOCK_SIZE * NUMBER_OF_BLOCKS);num++)
  {
    Buffer_MultiBlock[num] = 0x00;
  }
  SD_ReadDisk(Buffer_MultiBlock,10,NUMBER_OF_BLOCKS);



  while (1)
  {
    GPIO_SetBits(GPIOE,GPIO_Pin_4);  //熄灭LED灯
    Delay(500);                      //延时500ms
    GPIO_ResetBits(GPIOE,GPIO_Pin_4);//点亮LED灯
    Delay(500);                      //延时500ms
  }
}

/**
  * @brief  Configures SDIO IRQ channel.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_InitStructure.NVIC_IRQChannel = SD_SDIO_DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_Init(&NVIC_InitStructure);  
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
//微秒级的延时
void delay_us(uint32_t delay_us)
{    
  volatile unsigned int num;
  volatile unsigned int t;

  
  for (num = 0; num < delay_us; num++)
  {
    t = 11;
    while (t != 0)
    {
      t--;
    }
  }
}
//毫秒级的延时
void delay_ms(uint16_t delay_ms)
{    
  volatile unsigned int num;
  for (num = 0; num < delay_ms; num++)
  {
    delay_us(1000);
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
