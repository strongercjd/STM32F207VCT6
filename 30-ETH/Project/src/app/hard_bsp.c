#include "stm32f2xx.h"
#include "hard_bsp.h"
#include <stdio.h>

/* Private function prototypes -----------------------------------------------*/

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


extern uint32 G_timeTicks;
extern uint32 sys_LED_state;
extern uint32 sys_LED_time;
extern uint32 sys_LED_mode;

/*!
* @brief   LED_Init()
* 初始化LED指示灯IO
* @param 	NONE 
* @return	NONE
* @note
*/
void LED_Init(void)
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
}

/*!
* @brief LED_ray_run()
* @param 	NONE 
* @return	NONE
* @note
*/
void LED_ray_run(void)
{
  uint32 time;
  switch (sys_LED_mode){
  case 1 : 
    time = 10;
    break;
  case 2:
    time = 100;
    break;
  default :
    time = 100;
    break;
  }
  if((G_timeTicks-sys_LED_time)>=time){
    sys_LED_time=G_timeTicks;
    
    if(sys_LED_state == 0){
      GPIO_ResetBits(GPIOE,GPIO_Pin_4);
      sys_LED_state++;
    }
    else{
      GPIO_SetBits(GPIOE,GPIO_Pin_4);
      sys_LED_state = 0;
    }
    
  }
}

/******************************************************************************
** Function name:       timer4Init
** Descriptions:        定时器4初始化函数
** Input parameters:    None
** output parameters:   None
** Created Date:      
*******************************************************************************/
void timer4Init(void)
{  
  uint16 PrescalerValue;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;  
  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
  
  /* Enable the TIM4 gloabal Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 1000) - 1;	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 9; //10MS
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up|TIM_OPMode_Repetitive;


  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM4, PrescalerValue, TIM_PSCReloadMode_Immediate);  

  /* TIM4 Interrupts enable */
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);	

}


/***************************************************************************************
** 函数名称 ：UART_Init
** 函数功能 ：
** 入口参数 ：
** 出口参数 ：无
****************************************************************************************/
void UART_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  
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
  
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

