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
#include "MyTask_Mongoose.h"


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



//**********************

void vTaskBlink1( void * pvParameters )
{
  Log("Task Blink1 Started\r\n");
  /* Enter an infinite loop to perform the task processing. */
  for( ;; )
  {
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port,LED_ACTIVE_Pin,1);
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
    HAL_GPIO_WritePin(LED_ACTIVE_GPIO_Port,LED_ACTIVE_Pin,0);
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
    //SEGGER_RTT_WriteString(0, "Hello from STM32 -> Segger_RTT\r\n");
  }
}

//void vTaskBlink2( void * pvParameters )
//{
//  Log("Task Blink2 Started\r\n");
//  /* Enter an infinite loop to perform the task processing. */
//  for( ;; )
//  {
//    HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,1);
//    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
//    HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,0);
//    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
//  }
//}


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
    /* The task could not be created as there was insufficient heap memory remaining. If
    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
    amount of FreeRTOS heap memory that remains unallocated can be queried using the
    xPortGetFreeHeapSize() API function.*/
           
      Log("Task1 Blink Create Err\r\n");
    }
    else
    {
      Log("Task1 Blink Created\r\n");
    }
//    
//    
//        /* Create the task. */
//    if( xTaskCreate(
//    vTaskBlink2, /* Pointer to the function that implements the task. */
//    "task2", /* Text name given to the task. */
//    1024, /* The size of the stack that should be created for the task.
//    This is defined in words, not bytes. */
//    (void*) 0,/* A reference to xParameters is used as the task parameter.
//    This is cast to a void * to prevent compiler warnings. */
//    0, /* The priority to assign to the newly created task. */
//    &xHandle2 /* The handle to the task being created will be placed in
//    xHandle. */
//    ) != pdPASS )
//    {
//    /* The task could not be created as there was insufficient heap memory remaining. If
//    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
//    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
//    amount of FreeRTOS heap memory that remains unallocated can be queried using the
//    xPortGetFreeHeapSize() API function.*/
//           
//      Log("Task2 Blink Create Err\r\n");
//    }
//    else
//    {
//      Log("Task2 Blink Created\r\n");
//    }
       
  start_task_mongoose();
    
  vTaskDelete(NULL);

}







