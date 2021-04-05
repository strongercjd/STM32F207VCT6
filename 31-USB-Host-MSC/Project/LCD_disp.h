#ifndef __LCD_DISP_H 
#define __LCD_DISP_H

#include "stm32f2xx.h"



/******************************* LMT028 显示屏的FSMC地址 ***************************/
//#define      macEXMC_Addr_LMT028_CMD         ( ( uint32_t ) 0x60000000 )	     //FSMC_Bank1_NORSRAM1用于LCD命令操作的地址
//#define      macEXMC_Addr_LMT028_DATA        ( ( uint32_t ) 0x60010000 )             //FSMC_Bank1_NORSRAM1用于LCD数据操作的地址

typedef struct
{
  uint8_t LCD_CMD;//用于LCD命令操作
  uint8_t LCD_DATA;//用于LCD数据操作
} LCD_TypeDef;
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0000FFFF))
#define LCD             ((LCD_TypeDef *) LCD_BASE)


#define      LCD_RD_GPIO      GPIOD
#define      LCD_WR_GPIO      GPIOD
#define      LCD_CS_GPIO      GPIOD
#define      LCD_DC_GPIO      GPIOD

#define      LCD_RST_GPIO     GPIOC
#define      LCD_BK_GPIO      GPIOC
#define      LCD_FRAME_GPIO   GPIOC

#define      LCD_RD_Pin      GPIO_Pin_4 
#define      LCD_WR_Pin      GPIO_Pin_5 
#define      LCD_CS_Pin      GPIO_Pin_7 
#define      LCD_DC_Pin      GPIO_Pin_11 
#define      LCD_RST_Pin     GPIO_Pin_6 
#define      LCD_BK_Pin      GPIO_Pin_7 
#define      LCD_FRAME_Pin   GPIO_Pin_8




#define  LCD_red       0xf800
#define  LCD_grenn     0x07e0
#define  LCD_blue      0x001f
#define  LCD_yellow    0xffe0
#define  LCD_purple    0x780f
#define  LCD_drgay     0x7bef
#define  LCD_cyan      0x07ff
#define  LCD_olive     0x7be0
#define  LCD_black     0x0000

void LCD_Init (void);
void LMT028_Init (void);
void LMT028_GPIO_Config ();
void LMT028_FSMC_Config (void);
void LMT028_Rst (void);
void LMT028_REG_Config (void);
void LMT028_Write_Cmd (uint8_t usCmd);
void LMT028_Write_Data (uint8_t usData);
void LCD_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void clear_lcd_area(uint16_t X,uint16_t Y,uint16_t width,uint16_t height);
void Delay1ms(uint16_t k);
void SysCtlDelay(unsigned long ulCount);
void timer3_init();
void Set_Pen_Color(uint16_t Pen_Color);
void Set_Back_Color(uint16_t Pen_Color);
void LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t color);
void LCD_ShowString(uint16_t x,uint16_t y,uint8_t *s);
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t num,uint16_t size,uint16_t mode);
#endif
