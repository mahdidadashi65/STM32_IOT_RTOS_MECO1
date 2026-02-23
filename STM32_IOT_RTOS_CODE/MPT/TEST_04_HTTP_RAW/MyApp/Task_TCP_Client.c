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
#include "Task_TCP_Client.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "lwip/opt.h"
#include "lwip/api.h"
#include "lwip/sys.h"

/*-----------------------------------------------------------------------------------*/

TaskHandle_t xHandle_tcp_client;
TaskHandle_t xHandle_tcp_client_sender;


static struct netconn *conn_tcp_client;
static struct netbuf *buf_tcp_client;
static ip_addr_t *addr, dest_addr;
static unsigned short port, dest_port;
char msg_from_tcp_server[200];
char msg_to_tcp_server[200];
static int indx = 0;

// Function to send the data to the server
void tcpsend (struct netconn * conn ,char *data);

// tcpsem is the binary semaphore to prevent the access to tcpsend
sys_sem_t tcpsem;


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


static void tcp_client_thread(void *arg)
{
	err_t err, connect_error;

	/* Create a new connection identifier. */
	conn_tcp_client = netconn_new(NETCONN_TCP);
  
  //netconn_set_nonblocking(conn_tcp_client, true);

	if (conn_tcp_client!=NULL)
	{
		/* Bind connection to the port number 7 (port of the Client). */
		err = netconn_bind(conn_tcp_client, IP_ADDR_ANY, 8081);

		if (err == ERR_OK)
		{
			/* The desination IP adress of the computer */
			IP_ADDR4(&dest_addr, 192, 168, 10, 20);
			dest_port = 8080;  // server port

			/* Connect to the TCP Server */
			connect_error = netconn_connect(conn_tcp_client, &dest_addr, dest_port);

			// If the connection to the server is established, the following will continue, else delete the connection
			if (connect_error == ERR_OK)
			{
				// Release the semaphore once the connection is successful
				sys_sem_signal(&tcpsem);
				while (1)
				{
					/* wait until the data is sent by the server */
					if (netconn_recv(conn_tcp_client, &buf_tcp_client) == ERR_OK)
					{
						/* Extract the address and port in case they are required */
						addr = netbuf_fromaddr(buf_tcp_client);  // get the address of the client
						port = netbuf_fromport(buf_tcp_client);  // get the Port of the client

						/* If there is some data remaining to be sent, the following process will continue */
						do
						{

							strncpy (msg_from_tcp_server, buf_tcp_client->p->payload, buf_tcp_client->p->len);   // get the message from the server
             
              proccess_cmd(msg_from_tcp_server);

							// Or modify the message received, so that we can send it back to the client
              int len = sprintf (msg_to_tcp_server, "\"%s\" was sent by the TCP Server, Port:%d Address:%s\n", (char *) msg_from_tcp_server,port,ip_ntoa(addr));
              

							// semaphore must be taken before accessing the tcpsend function
							sys_arch_sem_wait(&tcpsem, 500);

							// send the data to the TCP Server
							tcpsend (conn_tcp_client,msg_to_tcp_server);

							memset (msg_from_tcp_server, '\0', 100);  // clear the buffer
						}
						while (netbuf_next(buf_tcp_client) >0);

						netbuf_delete(buf_tcp_client);
					}
				}
			}

			else
			{
				/* Close connection and discard connection identifier. */
				netconn_close(conn_tcp_client);
				netconn_delete(conn_tcp_client);
			}
		}
		else
		{
			// if the binding wasn't successful, delete the netconn connection
			netconn_delete(conn_tcp_client);
		}
	}
}

void tcpsend (struct netconn * conn ,char *data)
{
	// send the data to the connected connection
	netconn_write(conn, data, strlen(data), NETCONN_COPY);
	// relaese the semaphore
	sys_sem_signal(&tcpsem);
}


static void tcp_client_sender_thread (void *arg)
{
	for (;;)
	{
		sprintf (msg_to_tcp_server, "index value = %d\n", indx++);
		// semaphore must be taken before accessing the tcpsend function
		sys_arch_sem_wait(&tcpsem, 500);
		// send the data to the server
		tcpsend(conn_tcp_client,msg_to_tcp_server);
		osDelay(500);
	}
}



void init_task_tcp_client(void)
{		 
  Log("init_task_tcp_client Start\r\n");
  
  sys_sem_new(&tcpsem, 0);  // the semaphore would prevent simultaneous access to tcpsend
  
  if(xTaskCreate(tcp_client_thread,"tcp_client_thread",1024,(void*) 0,0,&xHandle_tcp_client) != pdPASS )
  {         
    Log("tcp_client_thread Create Err\r\n");
  }
  else
  {
    Log("tcp_client_thread Created\r\n");
  }
  
  
   if(xTaskCreate(tcp_client_sender_thread,"tcp_client_sender_thread",1024,(void*) 0,0,&xHandle_tcp_client_sender) != pdPASS )
  {         
    Log("tcp_client_sender_thread Create Err\r\n");
  }
  else
  {
    Log("tcp_client_sender_thread Created\r\n");
  }
     
}

