#include  "i2c_io.h"
#include "stm32f2xx.h"

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
#define I2C_DELAY()  SysCtlDelay_IIC(30)



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
  GPIO_Init_s.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init_s.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_Init_s);	
  
  
  GPIO_Init_s.GPIO_Pin = GPIO_Pin_14 ;
  GPIO_Init_s.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(GPIOC, &GPIO_Init_s);
  
  
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
  //IO输出
  SDA_OUT(); 
  SCL_OUT();
  //IO置0
  SDA_CLR();  
  SCL_CLR(); 
  I2C_DELAY();
  SCL_SET();
  //延时
  I2C_DELAY();  
  I2C_DELAY();
  I2C_DELAY();
  //SDA置1
  SDA_SET();
  I2C_DELAY();
  I2C_DELAY();
}
/*******************************************************************************
** Function name:       I2C_Send_byte
** Descriptions:        主机从I2C总线发送一个字节
** Input parameters:    data
** output parameters:   0-false 1_ture
** Created Date:        
*******************************************************************************/
uint8_t I2C_Send_byte(uint8_t data)
{
  uint8_t k;
  //发送8bit数据
  for(k=0;k<8;k++){
    
    I2C_DELAY();
    if(data&0x80){
      SDA_SET();
    }
    else{
      SDA_CLR();
    }
    data=data<<1;
    I2C_DELAY();
    SCL_SET();
    I2C_DELAY();
    I2C_DELAY();
    SCL_CLR();
  }
  //延时读取ACK响应
  I2C_DELAY();
  SDA_SET();
  //置为输入线
  SDA_IN();
  I2C_DELAY();
  SCL_SET();   
  I2C_DELAY(); //这里出现了问题，延时变的无限大
  //读数据
  k=SDA_READ();
  if(k){ ////NACK响应
    return 0;
  }
  I2C_DELAY();
  SCL_CLR();
  I2C_DELAY();
  SDA_OUT();
  if(k){ ////NACK响应
    return 0;
  }
  return 1;
  
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
  uint8_t k,data;
  //接收8bit数据
  //置为输入线
  
  SDA_IN();
  data=0;
  for(k=0;k<8;k++){
    I2C_DELAY();
    SCL_SET();
    I2C_DELAY();
    data=data<<1;
    data=data|SDA_READ();//读数据
    I2C_DELAY();
    SCL_CLR();
    I2C_DELAY(); 
  }
  //返回ACK响应
  //置为输出线
  SDA_OUT();
  if(flg){
    SDA_SET(); //输出1-NACK
  }else{
    SDA_CLR();//输出0-ACK
  }
  I2C_DELAY();
  SCL_SET();
  I2C_DELAY();
  I2C_DELAY();
  SCL_CLR();
  I2C_DELAY();
  SDA_OUT();
  //返回读取的数据
  return (uint8_t)data;
}
/*******************************************************************************
** Function name:       I2C_Write_nbyte
** Descriptions:        向I2C总线器件上指定的地址写入多个数据
** Input parameters:    1=addr,2_sub_addr,3-p,4-num
** output parameters:   0-false,1-ture
** Created Date:        
*******************************************************************************/
uint8_t I2C_Write_nbyte(uint8_t addr,uint8_t sub_addr,uint8_t *data_cache,uint32_t num)
{
  uint32_t temp_state,k;
  //启动I2C总线
  //I2C_Start();
  I2C_Start();
  //发送器件地址
  temp_state=I2C_Send_byte(addr&0xFE);//写模式
  if(!temp_state){
    I2C_Stop();
    return 0;
  }
  //发送器件子地址
  temp_state=I2C_Send_byte(sub_addr);
  if(!temp_state){
    I2C_Stop();
    return 0;
  }
  //发送后面的数据
  for(k=0;k<num;k++){
    
    temp_state=I2C_Send_byte(data_cache[k]);//写
    if(!temp_state){
      I2C_Stop();
      return 0;
    }   
  }
  
  I2C_Stop();
  return 1;
  
}
/*******************************************************************************
** Function name:       I2C_Read_nbyte
** Descriptions:        从I2C总线器件上指定的地址读取多个数据
** Input parameters:    1=addr,2_sub_addr,3-p,4-num
** output parameters:   0-false,1-ture
** Created Date:        
*******************************************************************************/
uint8_t I2C_Read_nbyte(uint8_t addr,uint8_t sub_addr,uint8_t *data_cache,uint32_t num)
{
  uint32_t temp_state,k;
  //启动I2C总线
  I2C_Start();
  //发送器件地址
  temp_state=I2C_Send_byte(addr&0xFE);//写模式
  if(!temp_state){
    I2C_Stop();
    return 0;
  }
  //发送器件子地址
  temp_state=I2C_Send_byte(sub_addr);
  if(!temp_state){
    I2C_Stop();
    return 0;
  }
  //发送器件地址
  I2C_Start();
  temp_state=I2C_Send_byte(addr|0x01);//读模式
  if(!temp_state){
    I2C_Stop();
    return 0;
  }
  //读取后面的数据
  for(k=0;k<num;k++){
    if(k == (num-1)){
      data_cache[k]=I2C_Receive_byte(1);//读取最后一个字节，发送NACK
    }else{
      data_cache[k]=I2C_Receive_byte(0);//读取数据  
    }
  }
  
  I2C_Stop();
 
  
  
  return 1; 
  
}
