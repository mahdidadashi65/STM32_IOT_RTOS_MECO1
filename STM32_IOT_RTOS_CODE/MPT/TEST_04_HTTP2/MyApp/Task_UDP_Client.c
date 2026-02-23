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
#include "Task_UDP_Client.h"
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"




/*-----------------------------------------------------------------------------------*/

TaskHandle_t xHandle_udp_client;

static struct netconn *conn_client;
static struct netbuf *buf_client;
static int indx = 0;
char msg_from_client[200];

/*-----------------------------------------------------------------------------------*/

static void udp_send_string(struct netconn * conn ,char *data)
{
	buf_client = netbuf_new();   // Create a new netbuf
	netbuf_ref(buf_client, data, strlen(data));  // refer the netbuf to the data to be sent 
	netconn_send(conn,buf_client);  // send the netbuf to the client
	netbuf_delete(buf_client);  // delete the netbuf
}


static void udp_client_thread(void *arg)
{
	err_t err, recv_err;
	ip_addr_t dest_addr;
	/* Create a new connection identifier */
	conn_client = netconn_new(NETCONN_UDP);

	if (conn_client!= NULL)
	{
			/* The desination IP adress of the computer */
			IP_ADDR4(&dest_addr, 192, 168, 1, 20);
			/* connect to the destination (server) at port 50002 */
			err = netconn_connect(conn_client, &dest_addr, 50002);
			if (err == ERR_OK)
			{      
          for (;;)
          {
            sprintf(msg_from_client, "index value = %d\n", indx++);
            udp_send_string(conn_client,msg_from_client);
            vTaskDelay( pdMS_TO_TICKS( 1000 ) );
          }
		  }
	}
	else
	{
		netconn_delete(conn_client);
	}
}
/*-----------------------------------------------------------------------------------*/


void init_task_udp_client(void)
{		 
  Log("init_task_udp Start\r\n");
 
  if( xTaskCreate(udp_client_thread,"udp_client_thread",1024,(void*) 0,0,&xHandle_udp_client) != pdPASS )
  {         
    Log("udp_client_thread Create Err\r\n");
  }
  else
  {
    Log("udp_client_thread Created\r\n");
  }    
}

