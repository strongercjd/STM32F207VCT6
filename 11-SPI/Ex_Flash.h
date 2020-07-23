#ifndef _Ex_Flash_H
#define _Ex_Flash_H 

/* Private define ------------------------------------------------------------*/
#define WRITE      0x02  /* Write to Memory instruction */
#define QUADWRITE  0x32  /* Quad Write to Memory instruction */
#define WRSR       0x01  /* Write Status Register instruction */
#define WREN       0x06  /* Write enable instruction */

#define READ       0x03  /* Read from Memory instruction */
#define QUADREAD   0x6B  /* Read from Memory instruction */
#define RDSR       0x05  /* Read Status Register instruction  */
#define RDID       0x9F  /* Read identification */
#define SE         0x20  /* Sector Erase instruction */
#define BE         0xC7  /* Bulk Erase instruction */    

#define WTSR       0x05  /* Write Status Register instruction  */


#define WIP_Flag   0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte 0xA5


#define  Ex_FLASH_PageSize    0x100

#define  Ex_FLASH_SecSize     0x1000



#define OK              1
#define ERROR           0

typedef enum{
  Ex_Flash_OK,
  Ex_Flash_ERR,
}Flash_enum;


typedef enum{
  Winbond_Flash,
  GD_Flash,
}Flash_stype_enum;
typedef enum{
  Flash_2M,
  Flash_4M,
}Flash_Memory_enum;

typedef struct{
  Flash_stype_enum  Flash_stype;
  Flash_Memory_enum Flash_Memory;
  uint32_t Flash_Memory_value;
}EX_Flash_struct;

uint8_t Ex_FLASH_Write(uint32_t WriteAddr,uint8_t* pBuffer,  uint16_t NumByteToWrite);
uint8_t Ex_Flash_RD_Sec(uint32_t Sec_Num,uint8_t* RcvBuf);
uint8_t Ex_Flash_RD(uint32_t ReadAddr,uint8_t* RcvBuf,uint32_t NByte);
void Ex_FLASH_ReadUID(uint8_t *uid);
uint32_t Ex_FLASH_ReadID(void);
uint8_t Ex_FLASH_SectorErase(uint32_t Sec_Num);

/* 调用接口——start */
//必须先调用EX_FLASH_init();(在初始化完硬件层后调用1次即可)，再调用其他读写擦除接口
Flash_enum EX_FLASH_init(void);
Flash_enum ExFlash_RD(uint32_t temp_addr,uint8_t *data_cach,uint32_t data_len);
Flash_enum ExFlash_WR_NByte(uint32_t temp_addr,uint8_t* data_cach,uint32_t data_len );
Flash_enum ExFlash_Erase_Sec(uint32_t Dst);
/* 调用接口——end */

#endif













