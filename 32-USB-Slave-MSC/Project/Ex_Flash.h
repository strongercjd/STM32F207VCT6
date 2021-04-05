#ifndef _Ex_Flash_H
#define _Ex_Flash_H 
#include <stdint.h> 

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

#define  Ex_FLASH_Max_Addr    (Ex_FLASH_SecSize*512)

#define Ex_Flash_OK          0
#define Ex_Flash_Over_Addr   1

uint8_t Ex_FLASH_Write(uint32_t WriteAddr,uint8_t* pBuffer,  uint16_t NumByteToWrite);
uint8_t Ex_Flash_RD_Sec(uint32_t Sec_Num,uint8_t* RcvBuf);
uint8_t Ex_Flash_RD(uint32_t ReadAddr,uint8_t* RcvBuf,uint32_t NByte);
void Ex_FLASH_ReadUID(uint8_t *uid);
uint32_t Ex_FLASH_ReadID(void);
uint8_t Ex_FLASH_SectorErase(uint32_t Sec_Num);

#endif













