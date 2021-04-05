#include "stm32f2xx.h"
#include "Ex_Flash.h"


uint8_t Sec_Data[Ex_FLASH_SecSize];

/* 移植接口——start */

extern uint8_t SPI_ReadWriteByte(uint8_t data);

#define	    Ex_Flash_CS_Low()		GPIO_ResetBits(GPIOE,GPIO_Pin_12);
#define     Ex_Flash_CS_High()          GPIO_SetBits(GPIOE,GPIO_Pin_12);

uint8_t MCU_ReadWriteByte(uint8_t TxData)
{
  return SPI_ReadWriteByte(TxData);
}

/* 移植接口——end */

/**
  * @brief  使能写功能 
  * @param  None
  * @retval None
  */
void Ex_FLASH_WriteEnable()
{
  Ex_Flash_CS_Low();
  MCU_ReadWriteByte(0x06);
  Ex_Flash_CS_High();
}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's status  register  
  * @param  None
  * @retval None
  */
void Ex_FLASH_WaitForWriteEnd(void)
{
    uint8_t FLASH_Status = 0;

    /* Select the FLASH: Chip Select low */
    Ex_Flash_CS_Low();

    /* Send "Read Status Register" instruction */
    MCU_ReadWriteByte(RDSR);

    /* Loop as long as the memory is busy with a write cycle */
    do
    {
        /* Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in FLASH_Status variable */
        FLASH_Status = MCU_ReadWriteByte(Dummy_Byte);
    }
    while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

    /* Deselect the FLASH: Chip Select high */
    Ex_Flash_CS_High();
}

/**
  * @brief  Erases the entire FLASH
  * @param  None
  * @retval None    
  */
void Ex_FLASH_BulkErase(void)
{
    /* Send write enable instruction */
    Ex_FLASH_WriteEnable();

    /* Bulk Erase */
    /* Select the FLASH: Chip Select low */
    Ex_Flash_CS_Low();
    /* Send Bulk Erase instruction  */
    MCU_ReadWriteByte(BE);
    /* Deselect the FLASH: Chip Select high */
    Ex_Flash_CS_High();

    /* Wait the end of Flash writing */
    Ex_FLASH_WaitForWriteEnd();
}

/**
  * @brief  Erases the Sector
  * @param  Sec_Num  Sector num
  * @retval Function execution result   
  */
uint8_t Ex_FLASH_SectorErase(uint32_t Sec_Num)
{
    uint32_t SectorAddr = 0;
    uint32_t i = 0;
    SectorAddr = Sec_Num * Ex_FLASH_SecSize;
    if(SectorAddr>Ex_FLASH_Max_Addr) 
      return Ex_Flash_Over_Addr;
    
    Ex_Flash_RD_Sec(Sec_Num,Sec_Data);
    for(i=0;i<Ex_FLASH_SecSize;i++){
      if(Sec_Data[i]!=0XFF)break;  
    }
    if(i==Ex_FLASH_SecSize){
      return Ex_Flash_OK;
    }
    /* Send write enable instruction */
    Ex_FLASH_WriteEnable();

    /* Sector Erase */
    /* Select the FLASH: Chip Select low */
    Ex_Flash_CS_Low();
    /* Send Sector Erase instruction */
    MCU_ReadWriteByte(0x20);
    /* Send SectorAddr high nibble address byte */
    MCU_ReadWriteByte((SectorAddr & 0xFF0000) >> 16);
    /* Send SectorAddr medium nibble address byte */
    MCU_ReadWriteByte((SectorAddr & 0xFF00) >> 8);
    /* Send SectorAddr low nibble address byte */
    MCU_ReadWriteByte(SectorAddr & 0xFF);
    /* Deselect the FLASH: Chip Select high */
    Ex_Flash_CS_High();

    /* Wait the end of Flash writing */
    Ex_FLASH_WaitForWriteEnd();
    
    return Ex_Flash_OK;
}

/**
  * @brief  Read the Sector
  * @param  Sec_Num  :Sector num
  * @param  RcvBuf   :pointer to the buffer
  * @retval Function execution result   
  */
uint8_t Ex_Flash_RD_Sec(uint32_t Sec_Num,uint8_t* RcvBuf)
{
  uint32_t addr = 0;
  uint32_t i;
  addr = Sec_Num * Ex_FLASH_SecSize;
  if(addr>Ex_FLASH_Max_Addr) 
    return Ex_Flash_Over_Addr;
  Ex_Flash_CS_Low();			
  MCU_ReadWriteByte(0x0B);
  MCU_ReadWriteByte(((addr & 0xFFFFFF) >> 16));
  MCU_ReadWriteByte(((addr & 0xFFFF) >> 8));
  MCU_ReadWriteByte(addr & 0xFF);
  
  MCU_ReadWriteByte(0xFF);
  for (i = 0; i < Ex_FLASH_SecSize; i++)		
  {
    RcvBuf[i] = MCU_ReadWriteByte(Dummy_Byte);		
  }
  Ex_Flash_CS_High();			
  return (Ex_Flash_OK);
}

uint8_t Ex_Flash_RD(uint32_t ReadAddr,uint8_t* RcvBuf,uint32_t NByte)
{
  uint32_t i = 0;
  if((ReadAddr+NByte)>Ex_FLASH_Max_Addr) 
    return Ex_Flash_Over_Addr;
  Ex_Flash_CS_Low();			
  MCU_ReadWriteByte(0x0B);
  MCU_ReadWriteByte(((ReadAddr & 0xFFFFFF) >> 16));
  MCU_ReadWriteByte(((ReadAddr & 0xFFFF) >> 8));
  MCU_ReadWriteByte(ReadAddr & 0xFF);
  
  MCU_ReadWriteByte(0xFF);
  for (i = 0; i < NByte; i++)		
  {
    RcvBuf[i] = MCU_ReadWriteByte(Dummy_Byte);		
  }
  Ex_Flash_CS_High();			
  return (Ex_Flash_OK);
}
 /**
  * @brief  Writes more than one byte to the FLASH(写小于一个页(256字节)的数据)
  * @param  WriteAddr : FLASH's internal address to write to
  * @param  pBuffer   : pointer to the buffer
  * @param  NumByteToWrite : number of bytes to write to the FLASH
  * @retval None    
  */
uint8_t Ex_Flash_PageWrite(uint32_t WriteAddr,uint8_t* pBuffer,  uint16_t NumByteToWrite)
{
  if((WriteAddr+NumByteToWrite)>Ex_FLASH_Max_Addr) 
    return Ex_Flash_Over_Addr;
  Ex_FLASH_WriteEnable();
  Ex_Flash_CS_Low();
  MCU_ReadWriteByte(WRITE);
  /* Send WriteAddr high nibble address byte to write to */
  MCU_ReadWriteByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  MCU_ReadWriteByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  MCU_ReadWriteByte(WriteAddr & 0xFF);

  /* while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
    MCU_ReadWriteByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }
  
  /* Deselect the FLASH: Chip Select high */
  Ex_Flash_CS_High();

  /* Wait the end of Flash writing */
  Ex_FLASH_WaitForWriteEnd();
  
  return Ex_Flash_OK;
}

/**
  * @brief  Writes block of data to the FLASH(写数据到Flash，写之前需要把相应的扇区擦除)
  * @param  WriteAddr : FLASH's internal address to write to
  * @param  pBuffer   : pointer to the buffer
  * @param  NumByteToWrite : number of bytes to write to the FLASH
  * @retval None    
  */
uint8_t Ex_FLASH_BufferWrite(uint32_t WriteAddr,uint8_t* pBuffer,  uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
    if((WriteAddr+NumByteToWrite)>Ex_FLASH_Max_Addr) 
      return Ex_Flash_Over_Addr;
    Addr = WriteAddr % Ex_FLASH_PageSize;
    count = Ex_FLASH_PageSize - Addr;
    NumOfPage =  NumByteToWrite / Ex_FLASH_PageSize;
    NumOfSingle = NumByteToWrite % Ex_FLASH_PageSize;
    /* WriteAddr is Ex_FLASH_PageSize aligned  */
    if (Addr == 0)
    {   
        /* NumByteToWrite < Ex_FLASH_PageSize */
        if (NumOfPage == 0) 
        {
            Ex_Flash_PageWrite( WriteAddr,pBuffer, NumByteToWrite);
        }
        else /* NumByteToWrite > Ex_FLASH_PageSize */
        {
            while (NumOfPage--)
            {
                Ex_Flash_PageWrite( WriteAddr,pBuffer, Ex_FLASH_PageSize);
                WriteAddr +=  Ex_FLASH_PageSize;
                pBuffer += Ex_FLASH_PageSize;
            }
            Ex_Flash_PageWrite(WriteAddr, pBuffer, NumOfSingle);
        }
    }
    else /* WriteAddr is not Ex_FLASH_PageSize aligned  */
    {
        if (NumOfPage == 0)
        {
            /* (NumByteToWrite + WriteAddr) > Ex_FLASH_PageSize */
            if (NumOfSingle > count) 
            {
                temp = NumOfSingle - count;
                Ex_Flash_PageWrite( WriteAddr, pBuffer,count);
                WriteAddr +=  count;
                pBuffer += count;
                Ex_Flash_PageWrite(WriteAddr, pBuffer, temp);
            }
            else
            {
                Ex_Flash_PageWrite(WriteAddr, pBuffer, NumByteToWrite);
            }
        }
        else /* NumByteToWrite > Ex_FLASH_PageSize */
        {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / Ex_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % Ex_FLASH_PageSize;

            Ex_Flash_PageWrite(WriteAddr, pBuffer, count);
            WriteAddr +=  count;
            pBuffer += count;

            while (NumOfPage--)
            {
                Ex_Flash_PageWrite(WriteAddr, pBuffer, Ex_FLASH_PageSize);
                WriteAddr +=  Ex_FLASH_PageSize;
                pBuffer += Ex_FLASH_PageSize;
            }

            if (NumOfSingle != 0)
            {
                Ex_Flash_PageWrite(WriteAddr, pBuffer, NumOfSingle);
            }
        }
    }
    return Ex_Flash_OK;
}

/**
  * @brief  Writes block of data to the FLASH(写数据到Flash)
  * @param  WriteAddr : FLASH's internal address to write to
  * @param  pBuffer   : pointer to the buffer
  * @param  NumByteToWrite : number of bytes to write to the FLASH
  * @retval None    
  */
uint8_t Ex_FLASH_Write(uint32_t WriteAddr,uint8_t* pBuffer,  uint16_t NumByteToWrite)
{
    //uint32_t Start_Sec;
    uint32_t Start_Addr;
    uint32_t Sec_num;
    uint32_t End_Addr;
    uint32_t Sec_Off;
    uint32_t i;
    if((WriteAddr+NumByteToWrite)>Ex_FLASH_Max_Addr) 
      return Ex_Flash_Over_Addr;
    //Start_Sec = WriteAddr/Ex_FLASH_SecSize;
    Start_Addr = WriteAddr % Ex_FLASH_SecSize;
    
    if(Start_Addr ==0){
      Sec_num = NumByteToWrite/Ex_FLASH_SecSize;
      End_Addr = (WriteAddr+NumByteToWrite)%Ex_FLASH_SecSize;
    }else {
      if(NumByteToWrite <Ex_FLASH_SecSize){
        Sec_num = 0;
        End_Addr = (WriteAddr+NumByteToWrite)%Ex_FLASH_SecSize;
      }else{
        Sec_num = (NumByteToWrite - (Ex_FLASH_SecSize-Start_Addr))/Ex_FLASH_SecSize;
        End_Addr = (WriteAddr+NumByteToWrite)%Ex_FLASH_SecSize;
      }
    }
    if(Start_Addr != 0)
    {
      Ex_Flash_RD_Sec(WriteAddr/Ex_FLASH_SecSize,Sec_Data);
      Sec_Off = WriteAddr%Ex_FLASH_SecSize;
      for(i=0;i<NumByteToWrite;i++){
        if(Sec_Data[Sec_Off+i]!=0XFF)break;  
      }
      if(i<NumByteToWrite){
        Ex_FLASH_SectorErase(WriteAddr/Ex_FLASH_SecSize);
        
        Ex_FLASH_BufferWrite((WriteAddr-Start_Addr), Sec_Data, Start_Addr);
        Ex_FLASH_BufferWrite(WriteAddr, pBuffer, NumByteToWrite);
        if((Start_Addr+NumByteToWrite)<Ex_FLASH_SecSize)
          Ex_FLASH_BufferWrite(WriteAddr+NumByteToWrite, &Sec_Data[WriteAddr+NumByteToWrite], (Ex_FLASH_SecSize-WriteAddr+NumByteToWrite));
      }else{
        Ex_FLASH_BufferWrite(WriteAddr, pBuffer, NumByteToWrite);
      }
      WriteAddr = WriteAddr + (Ex_FLASH_SecSize -Start_Addr);
      pBuffer = pBuffer + (Ex_FLASH_SecSize -Start_Addr);  
    }
    
    while (Sec_num--)
    {
      Ex_FLASH_SectorErase(WriteAddr/Ex_FLASH_SecSize);
      Ex_FLASH_BufferWrite(WriteAddr, pBuffer, Ex_FLASH_SecSize);
      WriteAddr +=  Ex_FLASH_SecSize;
      pBuffer += Ex_FLASH_SecSize;
    }
    
    if(End_Addr != 0)
    {
      Ex_Flash_RD_Sec(WriteAddr/Ex_FLASH_SecSize,Sec_Data);
      Sec_Off = WriteAddr%Ex_FLASH_SecSize;
      for(i=0;i<NumByteToWrite;i++){
        if(Sec_Data[Sec_Off+i]!=0XFF)break;  
      }
      if(i<NumByteToWrite){
        Ex_FLASH_SectorErase(WriteAddr/Ex_FLASH_SecSize);
        Ex_FLASH_BufferWrite(WriteAddr, pBuffer, End_Addr);
        Ex_FLASH_BufferWrite((WriteAddr+End_Addr), &Sec_Data[End_Addr], (Ex_FLASH_SecSize-End_Addr));
      }else{
        Ex_FLASH_BufferWrite(WriteAddr, pBuffer, End_Addr);
      }
      
    }
    return Ex_Flash_OK;
}

/**
  * @brief  Reads FLASH UID
  * @param  None
  * @retval FLASH identification
  */
void Ex_FLASH_ReadUID(uint8_t *uid)
{
  uint8_t i;
  /* Select the FLASH: Chip Select low */
  Ex_Flash_CS_Low();			
  MCU_ReadWriteByte(0x4B);
  MCU_ReadWriteByte(0xFF);
  MCU_ReadWriteByte(0xFF);
  MCU_ReadWriteByte(0xFF);
  MCU_ReadWriteByte(0xFF);
  for (i = 0; i < 8; i++){	
    uid[i] = MCU_ReadWriteByte(Dummy_Byte);		
  }
  /* Deselect the FLASH: Chip Select high */
  Ex_Flash_CS_High();
}

 /**
  * @brief  Reads FLASH identification
  * @param  None
  * @retval FLASH identification
  */
uint32_t Ex_FLASH_ReadID(void)
{
    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /* Select the FLASH: Chip Select low */
    Ex_Flash_CS_Low();

    /* Send "RDID " instruction */
    MCU_ReadWriteByte(RDID);

    /* Read a byte from the FLASH */
    Temp0 = MCU_ReadWriteByte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp1 = MCU_ReadWriteByte(Dummy_Byte);

    /* Read a byte from the FLASH */
    Temp2 = MCU_ReadWriteByte(Dummy_Byte);

    /* Deselect the FLASH: Chip Select high */
    Ex_Flash_CS_High();

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

    return Temp;
}


