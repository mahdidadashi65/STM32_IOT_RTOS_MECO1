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
#include "FreeRTOS.h"
#include "task.h"

#include <string.h>
#include <stdio.h>
#include "usart.h"

#include "Task_UDP_Server.h"
#include "Task_UDP_Client.h"

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}  
  

void Log(char* log)
{
   HAL_UART_Transmit(&huart3,(uint8_t*)log,strlen(log),100);	
}



void vTaskBlink1( void * pvParameters )
{
  UBaseType_t uxHighWaterMark;
    
  Log("Task Blink1 Started\r\n");
  /* Enter an infinite loop to perform the task processing. */
  for( ;; )
  {
    HAL_GPIO_WritePin(LED_BUS1_GPIO_Port,LED_BUS1_Pin,1);
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
    HAL_GPIO_WritePin(LED_BUS1_GPIO_Port,LED_BUS1_Pin,0);
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
    
    uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    printf("vTaskBlink1 uxHighWaterMark:%d\r\n",(int)uxHighWaterMark);
    
  }
}

void vTaskBlink2( void * pvParameters )
{
  Log("Task Blink2 Started\r\n");
  /* Enter an infinite loop to perform the task processing. */
  for( ;; )
  {
    HAL_GPIO_WritePin(LED_BUS2_GPIO_Port,LED_BUS2_Pin,1);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    HAL_GPIO_WritePin(LED_BUS2_GPIO_Port,LED_BUS2_Pin,0);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
  }
}


    TaskHandle_t xHandle1;
    TaskHandle_t xHandle2;


void MyApp(void)
{		 
  Log("MyApp Start\r\n");
  
    /* Create the task. */
    if( xTaskCreate(
    vTaskBlink1, /* Pointer to the function that implements the task. */
    "task1", /* Text name given to the task. */
    1024, /* The size of the stack that should be created for the task.
    This is defined in words, not bytes. */
    (void*) 0,/* A reference to xParameters is used as the task parameter.
    This is cast to a void * to prevent compiler warnings. */
    0, /* The priority to assign to the newly created task. */
    &xHandle1 /* The handle to the task being created will be placed in
    xHandle. */
    ) != pdPASS )
    {           
      Log("Task1 Blink Create Err\r\n");
    }
    else
    {
      Log("Task1 Blink Created\r\n");
    }
    
    
//    if( xTaskCreate(vTaskBlink2,"task2",1024,(void*) 0,0,&xHandle2) != pdPASS )
//    {         
//      Log("Task2 Blink Create Err\r\n");
//    }
//    else
//    {
//      Log("Task2 Blink Created\r\n");
//    }
  
    //vTaskDelete(NULL);
    
    //init_task_udp_server();
    //init_task_udp_client();
    //init_task_tcp_server();
    //init_task_tcp_client();
    init_task_http_server();
   
}

