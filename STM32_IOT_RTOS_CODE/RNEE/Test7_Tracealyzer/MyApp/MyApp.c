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
#include "queue.h"

#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "modbus_rtu.h"

#include <stdio.h>

//#include "SEGGER_RTT.h"

volatile int _Cnt;



TaskHandle_t xHandle1;
TaskHandle_t xHandle2;
TaskHandle_t xHandleRTTMenu;

TaskHandle_t xHandleModbus;
TaskHandle_t xHandleConsumeData;

QueueHandle_t xQueue1;


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
  //SEGGER_RTT_Write(0, (const char*)&ch, 1);

  return ch;
}  
  

void Log(char* log)
{
   HAL_UART_Transmit(&huart3,(uint8_t*)log,strlen(log),100);	
}


//void rtt_start(void)
//{
//   SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
//}


//void rtt_test(void) 
//{

//  SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n\r\n");
//  SEGGER_RTT_WriteString(0, "###### Testing SEGGER_printf() ######\r\n");

//  SEGGER_RTT_printf(0, "printf Test: %%c,         'S' : %c.\r\n", 'S');
//  SEGGER_RTT_printf(0, "printf Test: %%5c,        'E' : %5c.\r\n", 'E');
//  SEGGER_RTT_printf(0, "printf Test: %%-5c,       'G' : %-5c.\r\n", 'G');
//  SEGGER_RTT_printf(0, "printf Test: %%5.3c,      'G' : %-5c.\r\n", 'G');
//  SEGGER_RTT_printf(0, "printf Test: %%.3c,       'E' : %-5c.\r\n", 'E');
//  SEGGER_RTT_printf(0, "printf Test: %%c,         'R' : %c.\r\n", 'R');

//  SEGGER_RTT_printf(0, "printf Test: %%s,      \"RTT\" : %s.\r\n", "RTT");
//  SEGGER_RTT_printf(0, "printf Test: %%s, \"RTT\\r\\nRocks.\" : %s.\r\n", "RTT\r\nRocks.");

//  SEGGER_RTT_printf(0, "printf Test: %%u,       12345 : %u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%+u,      12345 : %+u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%.3u,     12345 : %.3u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%.6u,     12345 : %.6u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%6.3u,    12345 : %6.3u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%8.6u,    12345 : %8.6u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%08u,     12345 : %08u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%08.6u,   12345 : %08.6u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%0u,      12345 : %0u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-.6u,    12345 : %-.6u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-6.3u,   12345 : %-6.3u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-8.6u,   12345 : %-8.6u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-08u,    12345 : %-08u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-08.6u,  12345 : %-08.6u.\r\n", 12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-0u,     12345 : %-0u.\r\n", 12345);

//  SEGGER_RTT_printf(0, "printf Test: %%u,      -12345 : %u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%+u,     -12345 : %+u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%.3u,    -12345 : %.3u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%.6u,    -12345 : %.6u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%6.3u,   -12345 : %6.3u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%8.6u,   -12345 : %8.6u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%08u,    -12345 : %08u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%08.6u,  -12345 : %08.6u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%0u,     -12345 : %0u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-.6u,   -12345 : %-.6u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-6.3u,  -12345 : %-6.3u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-8.6u,  -12345 : %-8.6u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-08u,   -12345 : %-08u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-08.6u, -12345 : %-08.6u.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-0u,    -12345 : %-0u.\r\n", -12345);

//  SEGGER_RTT_printf(0, "printf Test: %%d,      -12345 : %d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%+d,     -12345 : %+d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%.3d,    -12345 : %.3d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%.6d,    -12345 : %.6d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%6.3d,   -12345 : %6.3d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%8.6d,   -12345 : %8.6d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%08d,    -12345 : %08d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%08.6d,  -12345 : %08.6d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%0d,     -12345 : %0d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-.6d,   -12345 : %-.6d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-6.3d,  -12345 : %-6.3d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-8.6d,  -12345 : %-8.6d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-08d,   -12345 : %-08d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-08.6d, -12345 : %-08.6d.\r\n", -12345);
//  SEGGER_RTT_printf(0, "printf Test: %%-0d,    -12345 : %-0d.\r\n", -12345);

//  SEGGER_RTT_printf(0, "printf Test: %%x,      0x1234ABC : %x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%+x,     0x1234ABC : %+x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%.3x,    0x1234ABC : %.3x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%.6x,    0x1234ABC : %.6x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%6.3x,   0x1234ABC : %6.3x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%8.6x,   0x1234ABC : %8.6x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%08x,    0x1234ABC : %08x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%08.6x,  0x1234ABC : %08.6x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%0x,     0x1234ABC : %0x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%-.6x,   0x1234ABC : %-.6x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%-6.3x,  0x1234ABC : %-6.3x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%-8.6x,  0x1234ABC : %-8.6x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%-08x,   0x1234ABC : %-08x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%-08.6x, 0x1234ABC : %-08.6x.\r\n", 0x1234ABC);
//  SEGGER_RTT_printf(0, "printf Test: %%-0x,    0x1234ABC : %-0x.\r\n", 0x1234ABC);

//  SEGGER_RTT_printf(0, "printf Test: %%p,      &_Cnt      : %p.\r\n", &_Cnt);

//  SEGGER_RTT_WriteString(0, "###### SEGGER_printf() Tests done. ######\r\n");
//  
//  _Cnt++;
// 
//}

void test_send_uart_modbus(void)
{
  uint8_t data[10]={1,2,3};
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,1);
  HAL_UART_Transmit(&huart2,(uint8_t*)data,10,100);	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,0);
}

//********************


//**********************

void vTaskBlink1( void * pvParameters )
{
  Log("Task Blink1 Started\r\n");
  /* Enter an infinite loop to perform the task processing. */
  
  int counter=0;
  for( ;; )
  {
    HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,1);
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
    HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,0);
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
    
    printf("counter:%d\r\n",counter++);
    
   // rtt_test();
  }
}

void vTaskBlink2( void * pvParameters )
{
  Log("Task Blink2 Started\r\n");
  /* Enter an infinite loop to perform the task processing. */
  for( ;; )
  {
    HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,1);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,0);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
  }
}


void vTaskRTTMenu( void * pvParameters )
{
  Log("Task RTTMenu Started\r\n");
  int r;
  int CancelOp;
  
  
    //vTaskSuspend(xHandle1);
    //vTaskSuspend(xHandle2);
  
  //vTaskDelay( pdMS_TO_TICKS( 1000 ) );


  do {
 //       HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,1);
 //       SEGGER_RTT_WriteString(0, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
 //       HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,0);
    
//        HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,1);
//        HAL_UART_Transmit(&huart3,(uint8_t*)"1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n",strlen("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n"),1000);        
//        HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,0);
    
  
    
        HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,1);
        printf("1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
        HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,0);
    
    vTaskDelay( pdMS_TO_TICKS( 100 ) );
  } while (1);
}



//void vTaskRTTMenu1( void * pvParameters )
//{
//  Log("Task RTTMenu Started\r\n");
//  int r;
//  int CancelOp;

//  do {
//    _Cnt = 0;

//    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
//    SEGGER_RTT_WriteString(0, "Press <1> start test1\r\n");
//    SEGGER_RTT_WriteString(0, "Press <2> start test2\r\n");
//    do 
//    {
//      r = SEGGER_RTT_WaitKey();
//    } while ((r != '1') && (r != '2'));
//    
//    if (r == '1') 
//    {
//      SEGGER_RTT_WriteString(0, "\r\nSelected <1>\r\n");
//    } 
//    else if (r == '2')  
//    {
//      SEGGER_RTT_WriteString(0, "\r\nSelected <2>\r\n");
//    }
//    
//  } while (1);
//}

//void vTaskRTTMenu2( void * pvParameters )
//{
//  Log("Task RTTMenu Started\r\n");
//  int r;
//  int CancelOp;

//  do {
//    _Cnt = 0;

//    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n");
//    SEGGER_RTT_WriteString(0, "Press <1> to continue in blocking mode (Application waits if necessary, no data lost)\r\n");
//    SEGGER_RTT_WriteString(0, "Press <2> to continue in non-blocking mode (Application does not wait, data lost if fifo full)\r\n");
//    do {
//      r = SEGGER_RTT_WaitKey();
//    } while ((r != '1') && (r != '2'));
//    if (r == '1') {
//      SEGGER_RTT_WriteString(0, "\r\nSelected <1>. Configuring RTT and starting...\r\n");
//      SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
//    } else {
//      SEGGER_RTT_WriteString(0, "\r\nSelected <2>. Configuring RTT and starting...\r\n");
//      SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
//    }
//    CancelOp = 0;
//    do {
//      //for (_Delay = 0; _Delay < 10000; _Delay++);
//      SEGGER_RTT_printf(0, "Count: %d. Press <Space> to get back to menu.\r\n", _Cnt++);
//      r = SEGGER_RTT_HasKey();
//      if (r) {
//        CancelOp = (SEGGER_RTT_GetKey() == ' ') ? 1 : 0;
//      }
//      //
//      // Check if user selected to cancel the current operation
//      //
//      if (CancelOp) {
//        SEGGER_RTT_WriteString(0, "Operation cancelled, going back to menu...\r\n");
//        break;
//      }
//    } while (1);
//    SEGGER_RTT_GetKey();
//    SEGGER_RTT_WriteString(0, "\r\n");
//  } while (1);
//}

//#define RX_BUFFER_SIZE 10

//uint8_t aRXBufferUser[RX_BUFFER_SIZE];

///**
//  * @brief Data buffers used to manage received data in interrupt routine
//  */
//uint8_t aRXBufferA[RX_BUFFER_SIZE];
//uint8_t aRXBufferB[RX_BUFFER_SIZE];

//__IO uint32_t     uwNbReceivedChars;
//uint8_t *pBufferReadyForUser;
//uint8_t *pBufferReadyForReception;

///**
//  * @brief  This function prints user info on PC com port and initiates RX transfer
//  * @retval None
//  */
//void StartReception(void)
//{
//  /* Initializes Buffer swap mechanism (used in User callback) :
//     - 2 physical buffers aRXBufferA and aRXBufferB (RX_BUFFER_SIZE length)
//  */
//  pBufferReadyForReception = aRXBufferA;
//  pBufferReadyForUser      = aRXBufferB;
//  uwNbReceivedChars        = 0;

//  /* Print user info on PC com port */
// // PrintInfo(&huart3, aTextInfoStart, COUNTOF(aTextInfoStart));

//  /* Initializes Rx sequence using Reception To Idle event API.
//     As DMA channel associated to UART Rx is configured as Circular,
//     reception is endless.
//     If reception has to be stopped, call to HAL_UART_AbortReceive() could be used.

//     Use of HAL_UARTEx_ReceiveToIdle_DMA service, will generate calls to
//     user defined HAL_UARTEx_RxEventCallback callback for each occurrence of
//     following events :
//     - DMA RX Half Transfer event (HT)
//     - DMA RX Transfer Complete event (TC)
//     - IDLE event on UART Rx line (indicating a pause is UART reception flow)
//  */
//  if (HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(&huart3, aRXBufferUser, RX_BUFFER_SIZE))
//  {
//    Error_Handler();
//  }
//}

///**
//  * @brief  This function handles buffer containing received data on PC com port
//  * @note   In this example, received data are sent back on UART Tx (loopback)
//  *         Any other processing such as copying received data in a larger buffer to make it
//  *         available for application, could be implemented here.
//  * @note   This routine is executed in Interrupt context.
//  * @param  huart UART handle.
//  * @param  pData Pointer on received data buffer to be processed
//  * @retval Size  Nb of received characters available in buffer
//  */
//void UserDataTreatment(UART_HandleTypeDef *huart, uint8_t* pData, uint16_t Size)
//{
//  /*
//   * This function might be called in any of the following interrupt contexts :
//   *  - DMA TC and HT events
//   *  - UART IDLE line event
//   *
//   * pData and Size defines the buffer where received data have been copied, in order to be processed.
//   * During this processing of already received data, reception is still ongoing.
//   *
//   */
//  uint8_t* pBuff = pData;
//  uint8_t  i;

//  /* Implementation of loopback is on purpose implemented in direct register access,
//     in order to be able to echo received characters as fast as they are received.
//     Wait for TC flag to be raised at end of transmit is then removed, only TXE is checked */
////  for (i = 0; i < Size; i++)
////  {
////    while (!(__HAL_UART_GET_FLAG(huart, UART_FLAG_TXE))) {}
////    huart->Instance->DR = *pBuff;
////    pBuff++;
////  }

//   //HAL_UART_Transmit(&huart3,(uint8_t*)pBuff,Size,100);	
//   
//   // send to queue

//    char cIn;
//    BaseType_t xHigherPriorityTaskWoken;
//    /* No tasks have yet been unblocked. */
//    xHigherPriorityTaskWoken = pdFALSE;
//    /* Loop until the buffer is empty. */
//    for(int i=0;i<Size;i++)
//    {
//      /* Obtain a byte from the buffer. */
//      cIn = pBuff[i];
//      /* Write the byte to the queue. xHigherPriorityTaskWoken will get set to
//      pdTRUE if writing to the queue causes a task to leave the Blocked state,
//      and the task leaving the Blocked state has a priority higher than the
//      currently executing task (the task that was interrupted). */
//      xQueueSendFromISR( xQueue1, &cIn, &xHigherPriorityTaskWoken );
//    }
//    /* Clear the interrupt source here. */
//    /* Now the buffer is empty, and the interrupt source has been cleared, a context
//    switch should be performed if xHigherPriorityTaskWoken is equal to pdTRUE.
//    NOTE: The syntax required to perform a context switch from an ISR varies from
//    port to port, and from compiler to compiler. Check the web documentation and
//    examples for the port being used to find the syntax required for your
//    application. */
//    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

//}

///**
//  * @brief  User implementation of the Reception Event Callback
//  *         (Rx event notification called after use of advanced reception service).
//  * @param  huart UART handle
//  * @param  Size  Number of data available in application reception buffer (indicates a position in
//  *               reception buffer until which, data are available)
//  * @retval None
//  */
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//  static uint8_t old_pos = 0;
//  uint8_t *ptemp;
//  uint8_t i;

//  /* Check if number of received data in recpetion buffer has changed */
//  if (Size != old_pos)
//  {
//    /* Check if position of index in reception buffer has simply be increased
//       of if end of buffer has been reached */
//    if (Size > old_pos)
//    {
//      /* Current position is higher than previous one */
//      uwNbReceivedChars = Size - old_pos;
//      /* Copy received data in "User" buffer for evacuation */
//      for (i = 0; i < uwNbReceivedChars; i++)
//      {
//        pBufferReadyForUser[i] = aRXBufferUser[old_pos + i];
//      }
//    }
//    else
//    {
//      /* Current position is lower than previous one : end of buffer has been reached */
//      /* First copy data from current position till end of buffer */
//      uwNbReceivedChars = RX_BUFFER_SIZE - old_pos;
//      /* Copy received data in "User" buffer for evacuation */
//      for (i = 0; i < uwNbReceivedChars; i++)
//      {
//        pBufferReadyForUser[i] = aRXBufferUser[old_pos + i];
//      }
//      /* Check and continue with beginning of buffer */
//      if (Size > 0)
//      {
//        for (i = 0; i < Size; i++)
//        {
//          pBufferReadyForUser[uwNbReceivedChars + i] = aRXBufferUser[i];
//        }
//        uwNbReceivedChars += Size;
//      }
//    }
//    /* Process received data that has been extracted from Rx User buffer */
//    UserDataTreatment(huart, pBufferReadyForUser, uwNbReceivedChars);

//    /* Swap buffers for next bytes to be processed */
//    ptemp = pBufferReadyForUser;
//    pBufferReadyForUser = pBufferReadyForReception;
//    pBufferReadyForReception = ptemp;
//  }
//  /* Update old_pos as new reference of position in User Rx buffer that
//     indicates position to which data have been processed */
//  old_pos = Size;

//}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
//{
////     HAL_UART_Transmit(&huart3,(uint8_t*)aRxBuffer,RXBUFFERSIZE,100);	
////    if(HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
////  {
////         Log("HAL_UARTEx_ReceiveToIdle_DMA Err\r\n");
////    Error_Handler();
////  }
//}

//void vTaskModbus( void * pvParameters )
//{
//  Log("Task vTaskModbus Started\r\n");
//  
//  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,0);

//  StartReception();
//  
//  /* Enter an infinite loop to perform the task processing. */
//  uint8_t data[10]={1,2,3};
//  for( ;; )
//  {  
////    HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,1);
////    HAL_UART_Transmit(&huart2,(uint8_t*)data,10,100);	
////    HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,0);
//    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
//  }
//}


//void vTaskConsumeData( void * pvParameters )
//{
//  char xMessage;
//    for( ;; )
//  {  
//    //get from queue
//    // get data from task rx and print
//    // HAL_UART_Transmit(&huart3,(uint8_t*)pBuff,Size,100);	
//    
//    /* Wait for the maximum period for data to become available on the queue.
//    The period will be indefinite if INCLUDE_vTaskSuspend is set to 1 in
//    FreeRTOSConfig.h. */
//    if( xQueueReceive( xQueue1, &xMessage, portMAX_DELAY ) != pdPASS )
//    {
//    /* Nothing was received from the queue – even after blocking to wait
//    for data to arrive. */
//      
//    }
//    else
//    {
//      HAL_UART_Transmit(&huart3,(uint8_t*)&xMessage,1,100);	
//    }
//    //vTaskDelay( pdMS_TO_TICKS( 1000 ) );
//  }
//}




void MyApp(void)
{		
  printf("MyApp Start\r\n");
  //vTaskDelete(xHandle1);
  //vTaskSuspend(xHandle1);
  //vTaskResume(xHandle1);

    //SEGGER_SYSVIEW_Conf();
  //SEGGER_SYSVIEW_Start();
  
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,1);
  char buff[100];
  
  BaseType_t Err;
  
  Log("MyApp Start\r\n");
  
    Err = xTaskCreate(
    vTaskBlink1, /* Pointer to the function that implements the task. */
    "blinktask1", /* Text name given to the task. */
    1024, /* The size of the stack that should be created for the task.
    This is defined in words, not bytes. */
    (void*) 0,/* A reference to xParameters is used as the task parameter.
    This is cast to a void * to prevent compiler warnings. */
    0, /* The priority to assign to the newly created task. */
    &xHandle1 /* The handle to the task being created will be placed in
    xHandle. */
    );
    /* Create the task. */
    if( Err != pdPASS )
    {
    /* The task could not be created as there was insufficient heap memory remaining. If
    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
    amount of FreeRTOS heap memory that remains unallocated can be queried using the
    xPortGetFreeHeapSize() API function.*/
      sprintf(buff,"Task1 Blink Create Err:%d\r\n",(int)Err);  
      Log(buff);
    }
    else
    {
      Log("Task1 Blink Created\r\n");
    }
    
    
        /* Create the task. */
    if( xTaskCreate(
    vTaskBlink2, /* Pointer to the function that implements the task. */
    "blinktask2", /* Text name given to the task. */
    1024, /* The size of the stack that should be created for the task.
    This is defined in words, not bytes. */
    (void*) 0,/* A reference to xParameters is used as the task parameter.
    This is cast to a void * to prevent compiler warnings. */
    0, /* The priority to assign to the newly created task. */
    &xHandle2 /* The handle to the task being created will be placed in
    xHandle. */
    ) != pdPASS )
    {
    /* The task could not be created as there was insufficient heap memory remaining. If
    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
    amount of FreeRTOS heap memory that remains unallocated can be queried using the
    xPortGetFreeHeapSize() API function.*/
           
     sprintf(buff,"Task2 Blink Create Err:%d\r\n",(int)Err);  
      Log(buff);
    }
    else
    {
      Log("Task2 Blink Created\r\n");
    }
    
//    Err= xTaskCreate(
//    vTaskModbus, /* Pointer to the function that implements the task. */
//    "modbustask", /* Text name given to the task. */
//    1024, /* The size of the stack that should be created for the task.
//    This is defined in words, not bytes. */
//    (void*) 0,/* A reference to xParameters is used as the task parameter.
//    This is cast to a void * to prevent compiler warnings. */
//    0, /* The priority to assign to the newly created task. */
//    &xHandleModbus /* The handle to the task being created will be placed in
//    xHandle. */
//    );
//            /* Create the task. */
//    if( Err != pdPASS )
//    {
//    /* The task could not be created as there was insufficient heap memory remaining. If
//    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
//    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
//    amount of FreeRTOS heap memory that remains unallocated can be queried using the
//    xPortGetFreeHeapSize() API function.*/
//           
//            sprintf(buff,"modbustaskCreate Err:%d\r\n",(int)Err);  
//      Log(buff);
//    }
//    else
//    {
//      Log("modbustask Created\r\n");
//    }
//    
//    Err=xTaskCreate(
//    vTaskConsumeData, /* Pointer to the function that implements the task. */
//    "ConsumeData", /* Text name given to the task. */
//    1024, /* The size of the stack that should be created for the task.
//    This is defined in words, not bytes. */
//    (void*) 0,/* A reference to xParameters is used as the task parameter.
//    This is cast to a void * to prevent compiler warnings. */
//    1, /* The priority to assign to the newly created task. */
//    &xHandleConsumeData /* The handle to the task being created will be placed in
//    xHandle. */
//    );
//                /* Create the task. */
//    if( Err != pdPASS )
//    {
//    /* The task could not be created as there was insufficient heap memory remaining. If
//    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
//    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
//    amount of FreeRTOS heap memory that remains unallocated can be queried using the
//    xPortGetFreeHeapSize() API function.*/
//           
//            sprintf(buff,"ConsumeData Err:%d\r\n",(int)Err);  
//      Log(buff);
//    }
//    else
//    {
//      Log("ConsumeData Created\r\n");
//    }
//    
//    
//    
//    /* Create the queue, storing the returned handle in the xQueue variable. */
//    xQueue1 = xQueueCreate( 100, 1 );
//    if( xQueue1 == NULL )
//    {
//      Log("xQueue1 Create Err\r\n");
//    }



//     /* Create the task. */
//    if( xTaskCreate(
//    vTaskRTTMenu, /* Pointer to the function that implements the task. */
//    "RTTMenu", /* Text name given to the task. */
//    1024, /* The size of the stack that should be created for the task.
//    This is defined in words, not bytes. */
//    (void*) 0,/* A reference to xParameters is used as the task parameter.
//    This is cast to a void * to prevent compiler warnings. */
//    0, /* The priority to assign to the newly created task. */
//    &xHandleRTTMenu /* The handle to the task being created will be placed in
//    xHandle. */
//    ) != pdPASS )
//    {
//    /* The task could not be created as there was insufficient heap memory remaining. If
//    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
//    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
//    amount of FreeRTOS heap memory that remains unallocated can be queried using the
//    xPortGetFreeHeapSize() API function.*/
//           
//     sprintf(buff,"Task2 Blink Create Err:%d\r\n",(int)Err);  
//      Log(buff);
//    }
//    else
//    {
//      Log("Task2 Blink Created\r\n");
//    }
    
    //init_nanoModbus();
    Log("Init End\r\n");

}
