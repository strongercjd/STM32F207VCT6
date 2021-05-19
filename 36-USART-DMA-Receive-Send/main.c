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
uint8_t UART_Buffer[100];
uint8_t UART_Receive_flg;
uint8_t UART_Receive_len;

#define UART_RX_LEN 32

void Uart_Reveice_DMA_Config(void);
void Uart_Send_DMA_Config(void);
void Uart_Send_DMA_Start(void);
void UART_Init(void);

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

  /* 使能GPIOE端口时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  /* 配置LED管脚 */
  GPIO_Init_s.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init_s.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init_s.GPIO_OType = GPIO_OType_PP;
  GPIO_Init_s.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_s.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_Init_s);
  
  UART_Receive_flg = 0;
  
  Uart_Reveice_DMA_Config();
  Uart_Send_DMA_Config();
  UART_Init();

  while (1)
  {
    if(UART_Receive_flg)
    {
      UART_Receive_flg = 0;
      Uart_Send_DMA_Start();
    }
  }
}

void UART_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  /* Enable UART1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  /* Connect PXx to USARTx_Tx*/
  GPIO_PinAFConfig(GPIOA, 9, GPIO_AF_USART1);
  
  /* Connect PXx to USARTx_Rx*/
  GPIO_PinAFConfig(GPIOA, 10, GPIO_AF_USART1);
  
  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  /* USART configuration */
  USART_Init(USART1, &USART_InitStructure);
  
  USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /*使能串口DMA接收*/
  USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
  /*使能串口DMA发送*/
  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
  
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}
/**
  * @brief  Configure the DMA controller according to the Stream parameters
  *         defined in main.h file 
  * @param  None
  * @retval None
  */
void Uart_Reveice_DMA_Config(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
    
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  
  /* Reset DMA Stream registers (for debug purpose) */
  DMA_DeInit(DMA2_Stream2);

  /* Check if the DMA Stream is disabled before enabling it.
     Note that this step is useful when the same Stream is used multiple times:
     enabled, then disabled then re-enabled... In this case, the DMA Stream disable
     will be effective only at the end of the ongoing data transfer and it will 
     not be possible to re-configure it before making sure that the Enable bit 
     has been cleared by hardware. If the Stream is used only once, this step might 
     be bypassed. */
  while (DMA_GetCmdStatus(DMA2_Stream2) != DISABLE)
  {
  }
  
  /* Configure DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //DMA请求发出通道
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//配置外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART_Buffer;//配置存储器地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//传输方向配置
  DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_LEN;//传输大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//memory地址自增
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设地址数据单位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//memory地址数据单位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA模式：正常模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级：高
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//FIFO 模式不使能.          
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;// FIFO 阈值选择
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发模式选择，可选单次模式、 4 节拍的增量突发模式、 8 节拍的增量突发模式或 16 节拍的增量突发模式。 
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发模式选择，可选单次模式、 4 节拍的增量突发模式、 8 节拍的增量突发模式或 16 节拍的增量突发模式。
  DMA_Init(DMA2_Stream2, &DMA_InitStructure); 
  
  /* DMA Stream enable */
  DMA_Cmd(DMA2_Stream2, ENABLE);
}


/**
  * @brief  Configure the DMA controller according to the Stream parameters
  *         defined in main.h file 
  * @param  None
  * @retval None
  */
void Uart_Send_DMA_Config(void)
{
  DMA_InitTypeDef  DMA_InitStructure;
    
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  
  /* Reset DMA Stream registers (for debug purpose) */
  DMA_DeInit(DMA2_Stream7);

  /* Check if the DMA Stream is disabled before enabling it.
     Note that this step is useful when the same Stream is used multiple times:
     enabled, then disabled then re-enabled... In this case, the DMA Stream disable
     will be effective only at the end of the ongoing data transfer and it will 
     not be possible to re-configure it before making sure that the Enable bit 
     has been cleared by hardware. If the Stream is used only once, this step might 
     be bypassed. */
  while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE)
  {
  }
  
  /* Configure DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //DMA请求发出通道
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//配置外设地址
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART_Buffer;//配置存储器地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//传输方向配置
  DMA_InitStructure.DMA_BufferSize = (uint32_t)UART_RX_LEN;//传输大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//memory地址自增
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设地址数据单位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//memory地址数据单位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//DMA模式：正常模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//优先级：高
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//FIFO 模式不使能.          
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;// FIFO 阈值选择
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//存储器突发模式选择，可选单次模式、 4 节拍的增量突发模式、 8 节拍的增量突发模式或 16 节拍的增量突发模式。 
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//外设突发模式选择，可选单次模式、 4 节拍的增量突发模式、 8 节拍的增量突发模式或 16 节拍的增量突发模式。
  DMA_Init(DMA2_Stream7, &DMA_InitStructure); 
  
  /* DMA Stream enable */
//  DMA_Cmd(DMA2_Stream7, ENABLE);
}

void Uart_Send_DMA_Start(void)
{
  DMA_SetCurrDataCounter(DMA2_Stream7,UART_Receive_len); 
  DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
  /* DMA Stream enable */
  DMA_Cmd(DMA2_Stream7, ENABLE);
}

void DealWith_UartData()
{
  DMA_Cmd(DMA2_Stream2, DISABLE);
  UART_Receive_flg = 1;
  UART_Receive_len = UART_RX_LEN - DMA_GetCurrDataCounter(DMA2_Stream2);
  UART_Buffer[UART_Receive_len] = 0;
  DMA_SetCurrDataCounter(DMA2_Stream2,UART_RX_LEN); 
  DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2);
  DMA_Cmd(DMA2_Stream2, ENABLE);
}
/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
