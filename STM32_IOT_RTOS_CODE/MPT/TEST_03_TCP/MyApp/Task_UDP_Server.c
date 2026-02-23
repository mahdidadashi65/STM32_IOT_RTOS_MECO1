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
#include "Task_UDP_Server.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"



/*-----------------------------------------------------------------------------------*/

TaskHandle_t xHandle_udp_server;

static struct netconn *conn_server;
static struct netbuf *buf_server;
static ip_addr_t *addr_client;
static unsigned short port_client;
char msg_client[200];
char msg_server[200];
/*-----------------------------------------------------------------------------------*/


//static void udpecho_thread(void *arg)
//{
//  err_t err, recv_err;
//  
//  LWIP_UNUSED_ARG(arg);

//  conn_server = netconn_new(NETCONN_UDP);
//  if (conn_server!= NULL)
//  {
//    err = netconn_bind(conn_server, IP_ADDR_ANY, 50001);
//    if (err == ERR_OK)
//    {
//      while (1) 
//      {
//        recv_err = netconn_recv(conn_server, &buf_server);
//      
//        if (recv_err == ERR_OK) 
//        {
//          addr_client = netbuf_fromaddr(buf_server);
//          port_client = netbuf_fromport(buf_server);
//          netconn_connect(conn_server, addr_client, port_client);
//          buf_server->addr.addr = 0;
//          
//          void *data;
//          u16_t len;
//          netbuf_data(buf_server, &data, &len);

//          // Allocate space for string (+1 for null terminator)
//          char *str = malloc(len + 1);
//          if (str) 
//          {
//              memcpy(str, data, len);
//              str[len] = '\0'; // Null-terminate
//              printf("Received: %s\n", str);
//            
//            if(strstr(str,"LEDON")!=0)
//            {            
//              //udp_send_string(conn_server,"SET LED ON");
//              HAL_GPIO_WritePin(LED_BUS2_GPIO_Port,LED_BUS2_Pin,1);
//            }
//            else if(strstr(str,"LEDOFF")!=0)
//            {            
//              //udp_send_string(conn_server,"SET LED OFF");
//              HAL_GPIO_WritePin(LED_BUS2_GPIO_Port,LED_BUS2_Pin,0);
//            }
//            else
//            {
//              netconn_send(conn_server,buf_server);
//            }           
//            
//             free(str);
//          }
//        
//          
//          netbuf_delete(buf_server);
//        }
//      }
//    }
//    else
//    {
//      netconn_delete(conn_server);
//    }
//  }
//}
///*-----------------------------------------------------------------------------------*/

static void proccess_cmd(char* cmd)
{
  if(strstr(cmd,"LEDON")!=0)
  {            
    HAL_GPIO_WritePin(LED_BUS2_GPIO_Port,LED_BUS2_Pin,1);
  }
  else if(strstr(cmd,"LEDOFF")!=0)
  {            
    HAL_GPIO_WritePin(LED_BUS2_GPIO_Port,LED_BUS2_Pin,0);
  }
  else
  {
  }
  
}
/*-----------------------------------------------------------------------------------*/
/**** Send RESPONSE every time the client sends some data ******/
static void udp_server_thread(void *arg)
{
	err_t err, recv_err;
	struct pbuf *txBuf;

	/* Create a new connection identifier */
	conn_server = netconn_new(NETCONN_UDP);

	if (conn_server!= NULL)
	{
		/* Bind connection to the port 7 */
		err = netconn_bind(conn_server, IP_ADDR_ANY, 50001);

		if (err == ERR_OK)
		{
			/* The while loop will run everytime this Task is executed */
			while (1)
			{
				/* Receive the data from the connection */
				recv_err = netconn_recv(conn_server, &buf_server);

				if (recv_err == ERR_OK) // if the data is received
				{
					addr_client = netbuf_fromaddr(buf_server);  // get the address of the client
					port_client = netbuf_fromport(buf_server);  // get the Port of the client
					strcpy (msg_client, buf_server->p->payload);   // get the message from the client
          
          proccess_cmd(msg_client);

					// Or modify the message received, so that we can send it back to the client
					int len = sprintf (msg_server, "\"%s\" was sent by the Client, Port:%d Address:%s\n", (char *) msg_client,port_client,ip_ntoa(addr_client));

					/* allocate pbuf from RAM*/
					txBuf = pbuf_alloc(PBUF_TRANSPORT,len, PBUF_RAM);

					/* copy the data into the buffer  */
					pbuf_take(txBuf, msg_server, len);

					// refer the nebuf->pbuf to our pbuf
					buf_server->p = txBuf;

					netconn_connect(conn_server, addr_client, port_client);  // connect to the destination address and port

					netconn_send(conn_server,buf_server);  // send the netbuf to the client

					buf_server->addr.addr = 0;  // clear the address
					pbuf_free(txBuf);   // clear the pbuf
					netbuf_delete(buf_server);  // delete the netbuf
				}
        else
        {
          printf("recv_err: %d\n", recv_err);
        }
			}
		}
		else
		{
			netconn_delete(conn_server);
		}
	}
}

void init_task_udp_server(void)
{		 
  Log("init_task_udp_server Start\r\n");
  
  if(xTaskCreate(udp_server_thread,"udp_server_thread",1024,(void*) 0,0,&xHandle_udp_server) != pdPASS )
  {         
    Log("udp_server_thread Create Err\r\n");
  }
  else
  {
    Log("udp_server_thread Created\r\n");
  }
     
}

