#ifndef _LWIP_APP_H
#define _LWIP_APP_H 

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "hard_bsp.h"
#include "stm32f2x7_eth_bsp.h"
#include "main.h"
   
typedef struct{
  uint32 sys_enet_init_state;//以太网初始化状态
  uint32 sys_phy_config_time;//以太网配置时间
  uint32 sys_check_link_time;//检查网线连接正常时间
  uint32 sys_MAC_change;     //MAC是否改变
  uint32 sys_IP_change;      //IP是否改变
  uint32 sys_ARPTimer;       //ARP时间
  uint32 sys_DHCPTimer;      //DHCP时间
  uint32 sys_DHCP_COARSE_Timer;//DHCP粗糙处理时间
  uint32 sys_TCP_time;       //循环调用TCP内核时间
  uint32 sys_DHCP_OK;        //DHCP完成
  uint32 sys_TCPclient_connect_OK;//TCP客户端连接服务器成功
}Struct_sys_change;


/*以太网上电初始化配置状态  sys_enet_init_state  */  
#define   ETH_RETST           0    //以太网复位状态
#define   ETH_RETST_WAIT      1    //以太网复位异常状态
#define   ETH_CONFIG          2    //以太网通信模式配置
#define   ETH_PHY_RST         3    //以太网PHY复位状态
#define   ETH_PHY_LINK        4    //以太网PHY_LINK状态
#define   ETH_PHY_AUTO        5    //以太网PHY_自动协商
#define   ETH_PHY_SET         6    //以太网PHY设置状态
#define   ETH_LWIP_INIT       7    //以太网LWIP初始化状态
#define   ETH_INIT_OK         8    //以太网初始化完成状态

/*以太网配置时间sys_phy_config_time*/
#define  PHY_CONFIG_SEC  20        //以太网配置时间20*10ms

/*检查网线连接正常时间 sys_check_link_time*/
#define  ETH_CHECK_SEC   100       //以太网配置时间100*10ms

/*ARP时间  sys_ARPTimer*/
#define  ARP_TIME       5*100//5s

/*DHCP时间  sys_DHCPTimer*/
#define DHCP_FINE_TIME  50 

/*DHCP粗糙处理时间  sys_DHCP_COARSE_Timer*/
#define DHCP_COARSE_TIME    60*100 

/*TCP时间  sys_TCP_time*/
#define TCP_TMR      25

#define MAX_DHCP_TRIES        4

void LwIP_Init(void);

#endif













