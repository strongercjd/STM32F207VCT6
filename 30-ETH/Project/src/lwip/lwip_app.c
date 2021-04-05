#include "lwip_app.h"
#include "stm32f2x7_eth.h"
#include "stm32f2x7_eth_bsp.h"
#include <stdio.h>

typedef enum 
{ 
  DHCP_START=0,
  DHCP_WAIT_ADDRESS,
  DHCP_ADDRESS_ASSIGNED,
  DHCP_TIMEOUT
} 
DHCP_State_TypeDef;


extern Struct_sys_change G_change;
extern uint32 sys_LED_mode;
extern uint32 G_timeTicks;
extern struct netif *netif_list;

DHCP_State_TypeDef DHCP_state = DHCP_START;
uint32 IPaddress = 0;

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

uint8_t G_iptab[4];
struct netif netif;
static struct tcp_pcb *tcp_server_pcb;
static struct tcp_pcb *client_pcb;

/**
  * @brief function used to send data
  * @param  tpcb: tcp control block
  * @param  p:  chain of pbuf's
  * @retval None 
  */
static void tcp_client_send(struct tcp_pcb *tpcb, struct pbuf *p)
{
  tcp_write(tpcb, p->payload, p->len, 1);
  tcp_output(tpcb);;
}

/**
  * @brief  This function implements the tcp_poll callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: tcp connection control block
  * @retval err_t: error code
  */
static err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
  uint8 data_buf[3];
  
  data_buf[0]=0x31;
  data_buf[1]=0x32;
  data_buf[2]=0x33;
  
  
  tcp_write(tpcb, &data_buf[0], 3, 1);
  tcp_output(tpcb); 
  printf("TCP client send heartbeat packet\r\n\r\n");
  return ERR_OK;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  arg: pointer on argument passed to callback
  * @param  tcp_pcb: tcp connection control block
  * @param  len: length of data sent 
  * @retval err_t: returned error code
  */
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  printf("TCP client send data success\r\n\r\n");
  return ERR_OK;
}

/**
  * @brief tcp_receiv callback
  * @param arg: argument to be passed to receive callback 
  * @param tpcb: tcp connection control block 
  * @param err: receive error code 
  * @retval err_t: retuned error  
  */
static err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{ 
  uint32 data_len;
  uint8 *p_data;
  
  if ((err == ERR_OK)&&(p != NULL)){
    tcp_recved(tpcb, p->tot_len);
    
    p_data=(uint8*)p->payload;
    data_len=p->len;
    
    printf("TCP server receives data：");
    for(int i=0;i<data_len;i++){
      printf("0X%x,",p_data[i]);
    }
    printf("\r\n");
    
    printf("TCP client returns the received data\r\n\r\n");
    /* send data */
    tcp_client_send(tpcb,p);
    
    pbuf_free(p);
  }
  else{
    G_change.sys_TCPclient_connect_OK = 0;
    /* remove callbacks */
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_poll(tpcb, NULL,0); 
    
    /* close tcp connection */
    tcp_close(tpcb);
  }  
  
  return ERR_OK;
}

/**
  * @brief Function called when TCP connection established
  * @param tpcb: pointer on the connection contol block
  * @param err: when connection correctly established err should be ERR_OK 
  * @retval err_t: returned error 
  */
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
  uint8 data_buf[3];
  
  if (err == ERR_OK)   
  { 
    /* copy data to pbuf */
    //pbuf_take(es->p_tx, (char*)data, strlen((char*)data));
    
    /* pass newly allocated es structure as argument to tpcb */
    tcp_arg(tpcb, NULL);
    
    /* initialize LwIP tcp_recv callback function */ 
    tcp_recv(tpcb, tcp_client_recv);
    
    /* initialize LwIP tcp_sent callback function */
    tcp_sent(tpcb, tcp_client_sent);
    
    /* initialize LwIP tcp_poll callback function */
    tcp_poll(tpcb, tcp_client_poll, 10);
    
    data_buf[0]=0x31;
    data_buf[1]=0x31;
    data_buf[2]=0x31;
    
    
    tcp_write(tpcb, &data_buf[0], 3, 1);
    tcp_output(tpcb);
    
    G_change.sys_TCPclient_connect_OK = 1;
    
    return ERR_OK;
  }
  else
  {
    G_change.sys_TCPclient_connect_OK = 0;
    /* close connection */
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_poll(tpcb, NULL,0);
    /* close tcp connection */
    tcp_close(tpcb);
  }
  return err;
}

/**
  * @brief  Connects to the TCP echo server
  * @param  None
  * @retval None
  */
void tcp_client_init(void)
{
  struct ip_addr DestIPaddr;
  
  /* create new tcp pcb */
  client_pcb = tcp_new();
  
  IP4_ADDR( &DestIPaddr, 192, 168, 1, 101 );
  
  /* connect to destination address/port */
  tcp_connect(client_pcb,&DestIPaddr,5005,tcp_client_connected);
  
  printf("Create TCP client success");
  printf("\r\nTCP client IP  : %d.%d.%d.%d", G_iptab[3], G_iptab[2], G_iptab[1], G_iptab[0]);
  printf("\r\nTCP client port: ");
  printf("\r\nremote TCP server：192.168.1.101");
  printf("\r\nremote TCP port: 5005");
  printf("\r\n\r\n");
}


/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_client_receive(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  uint32 data_len;
  uint8 *p_data;
  
  /* Connect to the remote client */
  udp_connect(upcb, addr, port);
  
  
  if(p != NULL){
    
    p_data=(uint8*)p->payload;
    data_len=p->len;
    
    printf("UDP client receives data：");
    for(int i=0;i<data_len;i++){
      printf("0X%x,",p_data[i]);
    }
    printf("\r\n");
    
  }
  
  printf("UDP client returns the received data\r\n\r\n");
  /* send udp data */
  udp_send(upcb, p); 
  
  /* free pbuf */
  pbuf_free(p);
  
  /* free the UDP connection, so we can accept new clients */
  //udp_remove(upcb);   
}

  /**
  * @brief  Connect to UDP echo server
  * @param  None
  * @retval None
*/
void udp_client_init(void)
{
  struct udp_pcb *upcb;
  struct ip_addr DestIPaddr;
  struct pbuf *p;
  err_t err;
  
  uint8 data_buf[3];
  

  data_buf[0]=0x31;
  data_buf[1]=0x31;
  data_buf[2]=0x31;
  
  
  /* Create a new UDP control block  */
  upcb = udp_new();
  
  /*assign destination IP address */
  IP4_ADDR( &DestIPaddr, 192, 168, 16, 101 );
  
  /* configure destination IP address and port */
  err= udp_connect(upcb, &DestIPaddr, 4321);
  
  if (err == ERR_OK)
  {
    /* Set a receive callback for the upcb */
    udp_recv(upcb, udp_client_receive, NULL);
    
    printf("\r\nCreate UDP client success");
    printf("\r\nremote UDP server：192.168.16.101");
    printf("\r\nremote UDP port: 4321");
    printf("\r\n\r\n");
    
    /* allocate pbuf from pool*/
    p = pbuf_alloc(PBUF_TRANSPORT,strlen((char *)data_buf),PBUF_POOL);
    
    if (p != NULL)
    {
      /* copy data to pbuf */
      pbuf_take(p, (uint8*)data_buf, strlen((char *)data_buf));
      
      /* send udp data */
      udp_send(upcb, p); 
      
      /* Free the p buffer */
      pbuf_free(p);
    }
    
    /* free the UDP connection, so we can accept new clients */
    //udp_disconnect(upcb);
    
  }
}


/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_server_receive(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{

  uint32 data_len;
  uint8 *p_data;
  /* Connect to the remote client */
  udp_connect(upcb, addr, port);
  
  
  if(p != NULL){
    
    p_data=(uint8*)p->payload;
    data_len=p->len;
    
    printf("UDP server receives data：");
    for(int i=0;i<data_len;i++){
      printf("0X%x,",p_data[i]);
    }
    printf("\r\n");
    
  }
  /* Tell the client that we have accepted it */
  udp_send(upcb, p);
  
  printf("UDP server returns the received data\r\n\r\n");

  /* free the UDP connection, so we can accept new clients */
  udp_disconnect(upcb);
	
  /* Free the p buffer */
  pbuf_free(p);
   
}

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_server_init(void)
{
   struct udp_pcb *upcb;
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, 5007);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_server_receive, NULL);
        
        printf("Create UDP server success");
        printf( "\r\nUDP server IP：%d.%d.%d.%d", G_iptab[3], G_iptab[2], G_iptab[1], G_iptab[0]);
        printf("\r\nUDP server port: 5007");
        printf("\r\n\r\n");
      }
      else
      {
        printf("can not bind pcb");
      }
   }
   else
   {
     printf("can not create pcb");
   } 
}


/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  p:  chain of pbuf's
  * @retval None
  */
void tcp_server_send(struct tcp_pcb *tpcb)
{
  uint8 data_buf[3];

  data_buf[0]=0x31;
  data_buf[1]=0x32;
  data_buf[2]=0x33;
  
  
  tcp_write(tpcb, &data_buf[0], 3, TCP_WRITE_FLAG_COPY);
  tcp_output(tpcb);                                
}

/**
  * @brief  This function is used to send large packets of data back for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  p:  chain of pbuf's
  * @retval None
  */
void tcp_server_send_back(struct tcp_pcb *tpcb, struct pbuf *p)
{
  struct pbuf *ptr;

  
  while (p != NULL)
  {
    /* get pointer on pbuf from es structure */
    ptr = p;
    tcp_recved(tpcb, p->len); 
    
    
    /* enqueue data for transmission */
    tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    tcp_output(tpcb);

    
    /* continue with next pbuf in chain (if any) */
    p = ptr->next;
     
  }
  
}

/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{

  if ((err == ERR_OK) && (p != NULL)){

    tcp_server_send_back(tpcb, p);
    
    printf("TCP server returns the received data\r\n\r\n");
    
    pbuf_free(p);
    
  }else{
    if(tpcb->state == CLOSE_WAIT)
    {
      
      printf("TCP client downline\r\n");
      printf("TCP client IP  : %d.%d.%d.%d\r\n",(uint8)(tpcb->remote_ip.addr),
             (uint8)((tpcb->remote_ip.addr)>>8),
             (uint8)((tpcb->remote_ip.addr)>>16),
             (uint8)((tpcb->remote_ip.addr)>>24));
      printf("TCP client port: %d",tpcb->remote_port);
      printf("\r\n\r\n");
      
      tcp_arg(tpcb, NULL);
      tcp_sent(tpcb, NULL);
      tcp_recv(tpcb, NULL);
      tcp_err(tpcb, NULL);
      tcp_poll(tpcb, NULL, 0);
      /* close tcp connection */
      tcp_close(tpcb);
    }
  }
  
  return ERR_OK;
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
  uint8 data_buf[3];

  data_buf[0]=0x31;
  data_buf[1]=0x31;
  data_buf[2]=0x31;
  
  
  tcp_write(tpcb, &data_buf[0], 3, 1);
  tcp_output(tpcb); 
  
  printf("TCP server sends data at intervals of 5 seconds\r\n\r\n");
  return ERR_OK;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs. 
  * @param  arg: pointer on argument parameter 
  * @param  err: not used
  * @retval None
  */
static void tcp_server_error(void *arg, err_t err)
{
  printf("TCP server made a mistake\r\n\r\n");
}

/** Function prototype for tcp sent callback functions. Called when sent data has
 * been acknowledged by the remote side. Use it to free corresponding resources.
 * This also means that the pcb has now space available to send new data.
 *
 * @param arg Additional argument to pass to the callback function (@see tcp_arg())
 * @param tpcb The connection pcb for which data has been acknowledged
 * @param len The amount of bytes acknowledged
 * @return ERR_OK: try to send some data by calling tcp_output
 *            Only return ERR_ABRT if you have called tcp_abort from within the
 *            callback function!
 */
err_t tcp_sever_sent_ok(void *arg, struct tcp_pcb *tpcb,u16_t len)
{
  printf("TCP server Send data successfully\r\n\r\n");
  return ERR_OK;
}
/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used 
  * @retval err_t: error status
  */
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;

  printf("New TCP client contact\r\n");
  printf("TCP client IP  : %d.%d.%d.%d\r\n",(uint8)(newpcb->remote_ip.addr),
                                            (uint8)((newpcb->remote_ip.addr)>>8),
                                            (uint8)((newpcb->remote_ip.addr)>>16),
                                            (uint8)((newpcb->remote_ip.addr)>>24));
  printf("TCP client port: %d",newpcb->remote_port);
  printf("\r\n\r\n");
  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);
  
  /* pass newly allocated es structure as argument to newpcb */
  tcp_arg(newpcb, NULL);
  
  /* initialize lwip tcp_recv callback function for newpcb  */ 
  tcp_recv(newpcb, tcp_server_recv);
  
  /* initialize lwip tcp_err callback function for newpcb  */
  tcp_err(newpcb, tcp_server_error);
  
  /* initialize lwip tcp_poll callback function for newpcb */
  tcp_poll(newpcb, tcp_server_poll, 10);
  
  /* initialize lwip tcp_sever_sent_ok callback function for newpcb */
  tcp_sent(newpcb, tcp_sever_sent_ok);
  
  ret_err = ERR_OK;
  
  return ret_err;  
}
/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
void tcp_server_init(void)
{
  /* create new tcp pcb */
  tcp_server_pcb = tcp_new();

  if (tcp_server_pcb != NULL)
  {
    err_t err;
    
    /* bind echo_pcb to port 7 (ECHO protocol) */
    err = tcp_bind(tcp_server_pcb, IP_ADDR_ANY, 7);
    
    if (err == ERR_OK)
    {
      /* start tcp listening for echo_pcb */
      tcp_server_pcb = tcp_listen(tcp_server_pcb);
      
      /* initialize LwIP tcp_accept callback function */
      tcp_accept(tcp_server_pcb, tcp_server_accept);
      
      printf("Create TCP server success");
      printf("\r\nTCP server IP  : %d.%d.%d.%d", G_iptab[3], G_iptab[2], G_iptab[1], G_iptab[0]);
      printf("\r\nTCP server port: 7");
      printf("\r\n\r\n");
    }
    else 
    {
      printf("Can not bind pcb\n");
    }
  }
  else
  {
    printf("Can not create new pcb\n");
  }
}


/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init(void)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;

  /* Initializes the dynamic memory heap defined by MEM_SIZE.*/
  mem_init();

  /* Initializes the memory pools defined by MEMP_NUM_x.*/
  memp_init();

//  IP4_ADDR(&ipaddr, 192, 168, 16, 211);
//  IP4_ADDR(&netmask, 255, 255 , 255, 0);
//  IP4_ADDR(&gw, 192, 168, 16, 1);
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;

  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
            struct ip_addr *netmask, struct ip_addr *gw,
            void *state, err_t (* init)(struct netif *netif),
            err_t (* input)(struct pbuf *p, struct netif *netif))
    
   Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/
  netif_add(&netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

  /*  Registers the default network interface.*/
  netif_set_default(&netif);

  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&netif);
}

/**
  * @brief  Called when a frame is received
  * @param  None
  * @retval None
  */
void LwIP_Pkt_Handle(void)
{
  /* Read a received packet from the Ethernet buffers and send it to the lwIP for handling */
  ethernetif_input(&netif);
}

/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
uint8 LwIP_DHCP_Process_Handle()
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
  
  uint8_t iptab[4];
  uint8_t iptxt[20];
  
  switch (DHCP_state)
  {
    case DHCP_START:
    {
      dhcp_start(&netif);
      IPaddress = 0;
      DHCP_state = DHCP_WAIT_ADDRESS;
      printf("Looking for DHCP server please wait...");
      printf("\r\n\r\n");
    }
    break;

    case DHCP_WAIT_ADDRESS:
    {
      /* Read the new IP address */
      IPaddress = netif.ip_addr.addr;

      if (IPaddress!=0) 
      {
        DHCP_state = DHCP_ADDRESS_ASSIGNED;	

        iptab[0] = (uint8_t)(IPaddress >> 24);
        iptab[1] = (uint8_t)(IPaddress >> 16);
        iptab[2] = (uint8_t)(IPaddress >> 8);
        iptab[3] = (uint8_t)(IPaddress);
        
        G_iptab[0] = iptab[0];
        G_iptab[1] = iptab[1];
        G_iptab[2] = iptab[2];
        G_iptab[3] = iptab[3];
        
        printf( "DHCP get IP：%d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);
        printf("\r\n\r\n");
        
        ipaddr.addr=netif.ip_addr.addr;
        netmask.addr=netif.netmask.addr;
        gw.addr=netif.gw.addr;
        
        /* Stop DHCP */
        dhcp_stop(&netif);
        
        netif_set_down(&netif);
        
        netif_set_addr(&netif,&ipaddr,&netmask,&gw);
        
        netif_set_default(&netif);
        
        netif_set_up(&netif);
        
        G_change.sys_DHCP_OK = 1;
        
        return 0;
      }
      else
      {
        /* DHCP timeout */
        if (netif.dhcp->tries > MAX_DHCP_TRIES)
        {
          DHCP_state = DHCP_TIMEOUT;

          /* Stop DHCP */
          dhcp_stop(&netif);
          
          return 1;

        }
      }
    }
    break;
    default: break;
  }
  
  return 1;
}
/*************************************************************************
** Function name:       Deal_with_Ethernet_MAC_IP
** Descriptions:        处理以太网初始化、MAC地址改变和IP地址改变
** Input parameters:    None
** output parameters:   None
** Created Date:        
*************************************************************************/
uint8 Deal_with_Ethernet_MAC_IP(void)
{
  if(G_change.sys_enet_init_state != ETH_INIT_OK){
    switch(G_change.sys_enet_init_state){
      case ETH_RETST:
      case ETH_RETST_WAIT:
      case ETH_CONFIG:
      case ETH_PHY_RST:
      case ETH_PHY_LINK:
      case ETH_PHY_AUTO:
      case ETH_PHY_SET:{
        ETH_MACDMA_Config();
        break;
      }
      case ETH_LWIP_INIT:{
        printf("Network connection successful.......");
        printf("\r\n\r\n");
        /* Initilaize the LwIP stack */
        LwIP_Init();
        G_change.sys_enet_init_state = ETH_INIT_OK;
        G_change.sys_IP_change=1;
        sys_LED_mode = 2;
        break;
      }
    }
    return 0;
  }
  if(G_change.sys_enet_init_state == ETH_INIT_OK){
    /* Fine DHCP periodic process every 500ms */
    if (G_timeTicks - G_change.sys_DHCPTimer >= DHCP_FINE_TIME)
    {
      G_change.sys_DHCPTimer =  G_timeTicks;
      dhcp_fine_tmr();
      if ((DHCP_state != DHCP_ADDRESS_ASSIGNED)&&(DHCP_state != DHCP_TIMEOUT))
      { 
        /* process DHCP state machine */
        LwIP_DHCP_Process_Handle();    
      }
    }
    
    /* DHCP Coarse periodic process every 60s */
    if (G_timeTicks - G_change.sys_DHCP_COARSE_Timer >= DHCP_COARSE_TIME)
    {
      G_change.sys_DHCP_COARSE_Timer =  G_timeTicks;
      dhcp_coarse_tmr();
    }
  }
  
  if(G_change.sys_IP_change == 1){
    if(G_change.sys_DHCP_OK == 1){
      tcp_server_init();
      udp_server_init();
      udp_client_init();
      tcp_client_init();
      G_change.sys_IP_change = 0;
    }
  }
  if(G_change.sys_MAC_change == 1){
  }
  
  
  if((G_timeTicks - G_change.sys_check_link_time)>ETH_CHECK_SEC){
    G_change.sys_check_link_time = G_timeTicks;
    
    if(G_change.sys_TCPclient_connect_OK == 0)
    {
      tcp_client_init();
    }
    
    /* Check whether the link interrupt has occurred or not */
    if(!(ETH_ReadPHYRegister(REL8201E_PHY_ADDRESS, PHY_BSR) & PHY_Linked_Status))
    {
      printf("Network disconnect.......");
      printf("\r\n\r\n");
      sys_LED_mode = 1;
      DHCP_state = DHCP_START;
      netif_set_down(&netif);
      netif_list = (&netif)->next ;
      G_change.sys_enet_init_state = ETH_RETST;
    }
  }
  
  //ARP每5s周期性调用一次
  if((G_timeTicks - G_change.sys_ARPTimer) == ARP_TIME){
    G_change.sys_ARPTimer = G_timeTicks;
    etharp_tmr();
  }
  
  /* TCP periodic process every 250 ms */
  if (G_timeTicks - G_change.sys_TCP_time >= TCP_TMR)
  {
    G_change.sys_TCP_time =  G_timeTicks;
    tcp_tmr();
  }
  
  return 0;
}


