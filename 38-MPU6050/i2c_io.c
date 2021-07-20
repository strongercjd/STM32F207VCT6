#include  "i2c_io.h"
#include "stm32f2xx.h"
#include "sys_delay.h"

/*
 * @brief  SysCtlDelay
 * @param  ulCount 延时值，必须大于0
 * @retval None
 */
void SysCtlDelay_IIC(unsigned long ulCount)
{
  SysCtlDelay(ulCount);
}


/////定义时钟频率,300KHz
#define I2C_DELAY()  SysCtlDelay_IIC(100)


void MPU_IIC_Delay()
{
  SysCtlDelay(2*(SystemCoreClock/3000000));
}
/*******************************************************************************
** Function name:       I2C_IO_init
** Descriptions:        I2C总线IO初始化
** Input parameters:    None
** output parameters:   None
** Created Date:        
*******************************************************************************/
void I2C_IO_init(void)
{
  
  GPIO_InitTypeDef  GPIO_Init_s;	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_Init_s.GPIO_Pin =  GPIO_Pin_15 ;
  GPIO_Init_s.GPIO_Mode = GPIO_Mode_OUT; 
  GPIO_Init_s.GPIO_OType = GPIO_OType_OD;
  GPIO_Init_s.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init_s.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_Init_s);	
  
  GPIO_Init_s.GPIO_Pin = GPIO_Pin_14 ;
  GPIO_Init_s.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOC, &GPIO_Init_s);
  
  SDA_SET();  
  SCL_SET(); 
}
      
/*******************************************************************************
** Function name:       I2C_Start
** Descriptions:        开始发送一个I2C起始信号
** Input parameters:    None
** output parameters:   None
** Created Date:        
*******************************************************************************/
void I2C_Start(void)
{
    //IO输出
    SDA_OUT(); 
    SCL_OUT(); 
    I2C_DELAY();
    //IO置高
    SDA_SET();  
    SCL_SET(); 
    //延时
    I2C_DELAY();  
    //为低
    SDA_CLR();
    I2C_DELAY();
    I2C_DELAY();
    SCL_CLR();
}
/*******************************************************************************
** Function name:       I2C_Stop
** Descriptions:        发送一个I2C总线结束信号
** Input parameters:    None
** output parameters:   None
** Created Date:        
*******************************************************************************/
void I2C_Stop(void)
{
  SDA_OUT(); //sda线输出
  SCL_CLR(); 
  SDA_CLR();//STOP:when CLK is high DATA change form low to high
  MPU_IIC_Delay();
  SCL_SET(); 
  SDA_SET();//发送I2C总线结束信号
  MPU_IIC_Delay();	  
}
/*******************************************************************************
** Function name:       I2C_Wait_Ack
** Descriptions:        等待应答信号到来
** Input parameters:    None
** output parameters:   1，接收应答失败,0，接收应答成功
** Created Date:        
*******************************************************************************/
uint8_t I2C_Wait_Ack(void)
{
  uint8_t ucErrTime=0;
  SDA_IN();     //SDA设置为输入  
  SDA_SET();
  MPU_IIC_Delay();	   
  SCL_SET();  
  MPU_IIC_Delay();	 
  while(SDA_READ())
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      //      MPU_IIC_Stop();
      return 1;
    }
  }
  SCL_CLR();//时钟输出0 	   
  return 0;    

} 
/*******************************************************************************
** Function name:       I2C_Send_byte
** Descriptions:        主机从I2C总线发送一个字节
** Input parameters:    data
** output parameters:   0-false 1_ture
** Created Date:        
*******************************************************************************/
uint8_t I2C_Send_byte(uint8_t txd)
{
  uint8_t t;   
  SDA_OUT(); 	    
  SCL_CLR();//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
    if(txd&0x80){
      SDA_SET();
    }
    else{
      SDA_CLR();
    }
    txd<<=1; 	  
    SCL_SET();
    MPU_IIC_Delay(); 
//    I2C_DELAY();
    SCL_CLR();	
    MPU_IIC_Delay();
//    I2C_DELAY();
  }
  
  return 1;
}
//产生ACK应答
void MPU_IIC_Ack(void)
{
  SCL_CLR();
  SDA_OUT();
  SDA_CLR();
  MPU_IIC_Delay();
  SCL_SET();
  MPU_IIC_Delay();
  SCL_CLR();
}
//不产生ACK应答		    
void MPU_IIC_NAck(void)
{
  SCL_CLR();
  SDA_OUT();
  SDA_SET();
  MPU_IIC_Delay();
  SCL_SET();
  MPU_IIC_Delay();
  SCL_CLR();
}	

/*******************************************************************************
** Function name:       I2C_Receive_byte
** Descriptions:        主机从I2C总线接收一个字节
** Input parameters:    None
** output parameters:   data
** Created Date:        
*******************************************************************************/
uint8_t I2C_Receive_byte(uint8_t flg)
{
  //读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
  
  unsigned char i,receive=0;
  SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
  {
    SCL_CLR();
    MPU_IIC_Delay();
    SCL_SET();
    receive<<=1;
    if(SDA_READ())
      receive++;   
    MPU_IIC_Delay(); 
  }					 
  if (!flg)
    MPU_IIC_NAck();//发送nACK
  else
    MPU_IIC_Ack(); //发送ACK   
  return receive;


}


