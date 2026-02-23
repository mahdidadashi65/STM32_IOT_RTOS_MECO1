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
#include "Task_TCP_Server.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"



/*-----------------------------------------------------------------------------------*/

TaskHandle_t xHandle_tcp_server;

/*-----------------------------------------------------------------------------------*/
//static void tcpecho_thread(void *arg)
//{
//  struct netconn *conn, *newconn;
//  err_t err, accept_err;
//  struct netbuf *buf;
//  void *data;
//  u16_t len;
//      
//  LWIP_UNUSED_ARG(arg);

//  /* Create a new connection identifier. */
//  conn = netconn_new(NETCONN_TCP);
//  
//  if (conn!=NULL)
//  {  
//    /* Bind connection to well known port number 8080. */
//    err = netconn_bind(conn, NULL, 8080);
//    
//    if (err == ERR_OK)
//    {
//      /* Tell connection to go into listening mode. */
//      netconn_listen(conn);
//    
//      while (1) 
//      {
//        /* Grab new connection. */
//         accept_err = netconn_accept(conn, &newconn);
//    
//        /* Process the new connection. */
//        if (accept_err == ERR_OK) 
//        {

//          while (netconn_recv(newconn, &buf) == ERR_OK) 
//          {
//            do 
//            {
//              netbuf_data(buf, &data, &len);
//              netconn_write(newconn, data, len, NETCONN_COPY);
//          
//            } 
//            while (netbuf_next(buf) >= 0);
//          
//            netbuf_delete(buf);
//          }
//        
//          /* Close connection and discard connection identifier. */
//          netconn_close(newconn);
//          netconn_delete(newconn);
//        }
//      }
//    }
//    else
//    {
//      netconn_delete(newconn);
//    }
//  }
//}

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


static struct netconn *conn_tcp_server, *newconn_tcp_server;
static struct netbuf *buf_tcp_server;
static ip_addr_t *addr_tcp_client;
static unsigned short port_tcp_client;
char msg_from_tcp_client[200];
char msg_to_tcp_client[200];


static void tcp_server_thread(void *arg)
{
	err_t err, accept_err, recv_error;

	/* Create a new connection identifier. */
	conn_tcp_server = netconn_new(NETCONN_TCP);

	if (conn_tcp_server!=NULL)
	{
		/* Bind connection to the port number 7. */
		err = netconn_bind(conn_tcp_server, IP_ADDR_ANY, 8080);

		if (err == ERR_OK)
		{
			/* Tell connection to go into listening mode. */
			netconn_listen(conn_tcp_server);

			while (1)
			{
				/* Grab new connection. */
				accept_err = netconn_accept(conn_tcp_server, &newconn_tcp_server);

				/* Process the new connection. */
				if (accept_err == ERR_OK)
				{

					/* receive the data from the client */
					while (netconn_recv(newconn_tcp_server, &buf_tcp_server) == ERR_OK)
					{
						/* Extrct the address and port in case they are required */
						addr_tcp_client = netbuf_fromaddr(buf_tcp_server);  // get the address of the client
						port_tcp_client = netbuf_fromport(buf_tcp_server);  // get the Port of the client

						/* If there is some data remaining to be sent, the following process will continue */
						do
						{

							strncpy (msg_from_tcp_client, buf_tcp_server->p->payload, buf_tcp_server->p->len);   // get the message from the client
              
              proccess_cmd(msg_from_tcp_client);

							// Or modify the message received, so that we can send it back to the client
              int len = sprintf (msg_to_tcp_client, "\"%s\" was sent by the TCP Client, Port:%d Address:%s\n", (char *) msg_from_tcp_client,port_tcp_client,ip_ntoa(addr_tcp_client));

							netconn_write(newconn_tcp_server, msg_to_tcp_client, len, NETCONN_COPY);  // send the message back to the client
							memset (msg_from_tcp_client, '\0', 200);  // clear the buffer
						}
						while (netbuf_next(buf_tcp_server) >0);

						netbuf_delete(buf_tcp_server);
					}

					/* Close connection and discard connection identifier. */
					netconn_close(newconn_tcp_server);
					netconn_delete(newconn_tcp_server);
				}
			}
		}
		else
		{
			netconn_delete(conn_tcp_server);
		}
	}
}



void init_task_tcp_server(void)
{		 
  Log("init_task_tcp_server Start\r\n");
  
  if(xTaskCreate(tcp_server_thread,"tcp_server_thread",1024,(void*) 0,0,&xHandle_tcp_server) != pdPASS )
  {         
    Log("tcp_server_thread Create Err\r\n");
  }
  else
  {
    Log("tcp_server_thread Created\r\n");
  }
     
}

