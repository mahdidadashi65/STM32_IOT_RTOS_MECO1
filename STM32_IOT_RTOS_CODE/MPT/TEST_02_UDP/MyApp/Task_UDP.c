//           ****************************************************
//          **   Processor      : STM32F107VCT6                   **
//         ***   Frequency      : 8MHz ExtClock 72MHZ CPU Clock   ***
//        ****   AUTHOR         : Mahdi Dadashi                   ****
//       *****   STM32CubeMX    : V5.40                           *****
//      ******   STM32CubeF1    : V1.8.0                          ******
//       *****   Compiler       : KEIL uVision V5.26              *****
//        ****   Instagram      : instagram.com/mahdidadashi65/   ****
//         ***   Telegram       : t.me/mahdidadashi65/            ***
//          **   Github         : github.com/mahdidadashi65/      **
//           ****************************************************
//.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''                                 ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''      www.mahdidadashi.ir        ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''                                 ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._

#include "main.h"
#include "MyApp.h"
#include "Task_UDP.h"
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdio.h>
#include "usart.h"




#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/api.h"
#include "lwip/sys.h"


#define UDPECHO_THREAD_PRIO  ( tskIDLE_PRIORITY + 4 )

static struct netconn *conn;
static struct netbuf *buf;
static ip_addr_t *addr;
static unsigned short port;
/*-----------------------------------------------------------------------------------*/
static void udpecho_thread(void *arg)
{
  err_t err, recv_err;
  
  LWIP_UNUSED_ARG(arg);

  conn = netconn_new(NETCONN_UDP);
  if (conn!= NULL)
  {
    err = netconn_bind(conn, IP_ADDR_ANY, 50001);
    if (err == ERR_OK)
    {
      while (1) 
      {
        recv_err = netconn_recv(conn, &buf);
      
        if (recv_err == ERR_OK) 
        {
          addr = netbuf_fromaddr(buf);
          port = netbuf_fromport(buf);
          netconn_connect(conn, addr, port);
          buf->addr.addr = 0;
          // buf to string
          // if string == IP
          // ip 192.168.1.10 to buf
          netconn_send(conn,buf);
          netbuf_delete(buf);
        }
      }
    }
    else
    {
      netconn_delete(conn);
    }
  }
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */

TaskHandle_t xHandle_udp;

void init_task_udp(void)
{		 
  Log("init_task_udp Start\r\n");
 
  //sys_thread_new("udpecho_thread", udpecho_thread, NULL, DEFAULT_THREAD_STACKSIZE,UDPECHO_THREAD_PRIO );

  
  if( xTaskCreate(udpecho_thread,"udpecho_thread",1024,(void*) 0,0,&xHandle_udp) != pdPASS )
  {         
    Log("task_udp Create Err\r\n");
  }
  else
  {
    Log("task_udp Created\r\n");
  }
     
}

