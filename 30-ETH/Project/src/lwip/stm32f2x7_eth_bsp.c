/**
  ******************************************************************************
  * @file    stm32f2x7_eth_bsp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    07-October-2011 
  * @brief   STM32F2x7 Ethernet hardware configuration.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f2x7_eth.h"
#include "stm32f2x7_eth_bsp.h"
#include "main.h"
#include "lwip_app.h"
#include "hard_bsp.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t  EthInitStatus = 0;
__IO uint8_t EthLinkStatus = 0;
extern Struct_sys_change G_change;
/* Private function prototypes -----------------------------------------------*/
void ETH_GPIO_Config(void);
void ETH_MACDMA_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  ETH_BSP_Config
  * @param  None
  * @retval None
  */
void ETH_BSP_Config(void)
{
  /* Configure the GPIO ports for ethernet pins */
  ETH_GPIO_Config();
}

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
void ETH_MACDMA_Config(void)
{
  ETH_InitTypeDef ETH_InitStructure;
  if(G_change.sys_enet_init_state == ETH_RETST){
    /* Enable ETHERNET clock  */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                           RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
    
    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();
    
    /* Software reset */
    ETH_SoftwareReset();
    
    G_change.sys_enet_init_state = ETH_RETST_WAIT;
    
    return ;
  }
  
  if(G_change.sys_enet_init_state == ETH_RETST_WAIT){
    /* Wait for software reset */
    while (ETH_GetSoftwareResetStatus() == SET){
      G_change.sys_enet_init_state = ETH_RETST_WAIT;
      return;
    }
    G_change.sys_enet_init_state = ETH_CONFIG;
    return;
  }

  /* ETHERNET Configuration --------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);                                        //初始化网络为默认值

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;         //开启网络自适应功能
  //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
  //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
  //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;               //关闭反馈
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;     //关闭重传功能
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;//关闭自动去除PDA/CRC功能 
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;                    //关闭接收所有的帧
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//允许接收所有广播帧
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;          //关闭混合模式的地址过滤  
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//对于组播地址使用完美地址过滤
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;   //对单播地址使用完美地址过滤 
#ifdef CHECKSUM_BY_HARDWARE                                                      //使用硬件帧校验
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;            //开启ipv4和TCP/UDP/ICMP的帧校验和卸载
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  //当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
  //这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //开启丢弃TCP/IP错误帧
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;                 //开启接收数据的存储转发模式        
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;               //开启发送数据的存储转发模式     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;                  //禁止转发错误帧       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;//不转发过小的好帧  
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;                   //打开处理第二帧功能
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;                 //开启DMA传输的地址对齐功能      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                                   //开启固定突发功能                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;                       //DMA发送的最大突发长度为32个节拍           
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;                       //DMA接收的最大突发长度为32个节拍
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  EthInitStatus = ETH_Init(&ETH_InitStructure, REL8201E_PHY_ADDRESS);                          //配置ETH
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void ETH_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI |
                         RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH |
                         RCC_AHB1Periph_GPIOF, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);  

  /* Configure MCO (PA8) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Output HSE clock (25MHz) on MCO pin (PA8) to clock the PHY */
  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
  
  /* PHY_CLOCK_MCO */
  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_MII);

/* Ethernet pins configuration ************************************************/
   /*
        ETH_MDIO -------------------------> PA2
        ETH_MDC --------------------------> PC1
        ETH_PPS_OUT ----------------------> PB5
        ETH_MII_CRS ----------------------> PH2
        ETH_MII_COL ----------------------> PH3
        ETH_MII_RX_ER --------------------> PI10
        ETH_MII_RXD2 ---------------------> PH6
        ETH_MII_RXD3 ---------------------> PH7
        ETH_MII_TX_CLK -------------------> PC3
        ETH_MII_TXD2 ---------------------> PC2
        ETH_MII_TXD3 ---------------------> PB8
        ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
        ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
        ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
        ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
        ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
        ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
        ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
                                                  */

  /* Configure PA0,PA1, PA2,PA3 and PA7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);
  
  /* Configure PB0,PB1,PB8,PB10,PB11,PB12 and PB13 */
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);
  
  /* Configure PC1, PC2, PC3, PC4 and PC5 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource3, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
}


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
