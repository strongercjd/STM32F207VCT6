/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "Ex_Flash.h"
#include "stm32f2x_sdio_sd.h"

/* Definitions of physical drive number for each drive */

#define Ex_Flash	0	//外部串行NorFlash
#define SD_CARD  	1	//SD卡
#define USB_DISK  	2	//U盘

/*Flash参数*/
#define FATFS_SECTOR_SIZE 	512	
#define FATFS_BLOCK_SIZE   	8         //每个BLOCK有8个扇区
#define FATFS_SECTOR_COUNT      2*1024*1024      //GD25Q16,前2M字节给FATFS占用

/*SD卡参数*/

extern USBH_HOST              USB_Host;//USB HOST结构体，存储主机相关状态。



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
  DSTATUS stat;
  int result;
  
  switch (pdrv) {
  case Ex_Flash :
    result = Ex_FLASH_ReadID();
    
    stat =0;
    return stat;
    break;
    
  case SD_CARD :
    result = SD_Init();
    if(result != SD_OK)
    {
      stat =STA_NOINIT;
    }else{
      stat =0;
    }
    return stat;
    break;
  case USB_DISK://U盘
    if(USBH_UDISK_Status())
      return 0;	//U盘连接成功,则返回1.否则返回0		  
    else 
      return 1;	

  }
  
  return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
        case Ex_Flash :
          result = Ex_FLASH_ReadID();
          
          stat =0;
          return stat;
          break;
        case SD_CARD :
          result = SD_Init();
          if(result != SD_OK)
          {
            stat =STA_NOINIT;
          }else{
            stat =0;
          }
          return stat;
          break;
        case USB_DISK://U盘
          if(USBH_UDISK_Status())
            return 0;	//U盘连接成功,则返回1.否则返回0		  
          else 
            return 1;	
          
	}
        
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
        DSTATUS stat;
	int result;
        
        if (!count)
          return RES_PARERR;
	switch (pdrv) {
        case Ex_Flash:
          for(;count>0;count--)
          {
            Ex_Flash_RD(sector*FATFS_SECTOR_SIZE,buff,FATFS_SECTOR_SIZE);
            sector++;
            buff+=FATFS_SECTOR_SIZE;
          }
          res = RES_OK;
          return res;
          break;
        case SD_CARD :
          res=SD_ReadDisk(buff,sector,count);
          if(res)
          {
            SD_Init();
            res=SD_ReadDisk(buff,sector,count);	
          }
          if(res)
          {
            stat =STA_NOINIT;
          }else{
            stat =0;
          }
          return stat;
          break;
        case USB_DISK://U盘 
          res=USBH_UDISK_Read(buff,sector,count);
          return res;	  
          break;
	}


	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;
        DSTATUS stat;

	switch (pdrv) {
        case Ex_Flash:
          for(;count>0;count--)
          {
            Ex_FLASH_Write(sector*FATFS_SECTOR_SIZE,(uint8_t*)buff,FATFS_SECTOR_SIZE);
            sector++;
            buff+=FATFS_SECTOR_SIZE;
          }
          res = 0;
          return res;
          break;
        case SD_CARD :
          res=SD_WriteDisk((uint8_t*)buff,sector,count);
          if(res)
          {
            SD_Init();
            res=SD_WriteDisk((uint8_t*)buff,sector,count);
          }
          if(res)
          {
            stat =STA_NOINIT;
          }else{
            stat =0;
          }
          return stat;
          break;
        case USB_DISK://U盘
          res=USBH_UDISK_Write((uint8_t*)buff,sector,count); 
          return res;
          break;

	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
        case Ex_Flash:
          switch(cmd)
          {
          case CTRL_SYNC:
            res = RES_OK; 
            break;	 
          case GET_SECTOR_SIZE:
            *(WORD*)buff = FATFS_SECTOR_SIZE;
            res = RES_OK;
            break;	 
          case GET_BLOCK_SIZE:
            *(WORD*)buff = FATFS_BLOCK_SIZE;
            res = RES_OK;
            break;	 
          case GET_SECTOR_COUNT:
            *(DWORD*)buff = FATFS_SECTOR_COUNT;
            res = RES_OK;
            break;
          default:
            res = RES_PARERR;
            break;
          }
          return res;
          break;
        case SD_CARD :
          switch(cmd)
          {
          case CTRL_SYNC:
            res = RES_OK; 
            break;	 
          case GET_SECTOR_SIZE:
            *(DWORD*)buff = 512; 
            res = RES_OK;
            break;	 
          case GET_BLOCK_SIZE:
            *(WORD*)buff = SDCardInfo.CardBlockSize;
            res = RES_OK;
            break;	 
          case GET_SECTOR_COUNT:
            *(DWORD*)buff = SDCardInfo.CardCapacity/512;
            res = RES_OK;
            break;
          default:
            res = RES_PARERR;
            break;
          }
          return res;
          break;
        case USB_DISK :
          switch(cmd)
          {
          case CTRL_SYNC:
            res = RES_OK; 
            break;	 
          case GET_SECTOR_SIZE:
            *(WORD*)buff=512;
            res = RES_OK;
            break;	 
          case GET_BLOCK_SIZE:
            *(WORD*)buff=512;
            res = RES_OK;
            break;	 
          case GET_SECTOR_COUNT:
            *(DWORD*)buff=USBH_MSC_Param.MSCapacity;
            res = RES_OK;
            break;
          default:
            res = RES_PARERR;
            break;
          }
          return res;
          break;
	}

	return RES_PARERR;
}
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}

