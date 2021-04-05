#include "stm32f2xx.h"
#include "main.h"
#include "lwip_app.h"
#include <stdio.h>

uint32 G_timeTicks;
uint32 sys_LED_state;
uint32 sys_LED_time;
uint32 sys_LED_mode = 1;
Struct_sys_change G_change;

/*!
* @brief main()
* @param 	NONE 
* @return	NONE
* @note
*/
int main(){
  uint32_t line = 0;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  timer4Init();
  LED_Init();
  UART_Init();
  LCD_Init();
  
  ETH_BSP_Config();
  
  printf("\r\n======================================================================");
  printf("\r\n=               (C) COPYRIGHT 2020                                   =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                ST207_ETH Program Demo                              =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                                           By Firefly               =");
  printf("\r\n======================================================================");
  printf("\r\n\r\n");
  
  LCD_ShowString(0,16*(line++),"========================================");
  LCD_ShowString(0,16*(line++),"=       (C) COPYRIGHT 2020             =");
  LCD_ShowString(0,16*(line++),"=                                      =");
  LCD_ShowString(0,16*(line++),"=        ST207_ETH Program Demo        =");
  LCD_ShowString(0,16*(line++),"=                                      =");
  LCD_ShowString(0,16*(line++),"=                        By Firefly    =");
  LCD_ShowString(0,16*(line++),"========================================");
  
  while(1){
    Deal_with_Ethernet_MAC_IP();
    LED_ray_run();
  } 
}

