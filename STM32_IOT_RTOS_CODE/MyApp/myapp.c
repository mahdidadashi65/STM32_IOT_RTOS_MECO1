#include "myapp.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "httpserver-socket.h"



#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

//V2
//osThreadId_t myappTaskHandle;
//const osThreadAttr_t myappTask_attributes = {
//  .name = "myappTask",
//  .stack_size = 128 * 4,
//  .priority = (osPriority_t) osPriorityNormal,
//};

//V1
osThreadId myappTaskHandle;


void StartMyAppTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
    http_server_socket_init();

  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_ACTIVE_GPIO_Port,LED_ACTIVE_Pin);
    HAL_GPIO_TogglePin(LED_LAN_GPIO_Port,LED_LAN_Pin);
    HAL_GPIO_TogglePin(LED_FAULT_GPIO_Port,LED_FAULT_Pin);
    HAL_GPIO_TogglePin(LED_BUS1_GPIO_Port,LED_BUS1_Pin);
    HAL_GPIO_TogglePin(LED_BUS2_GPIO_Port,LED_BUS2_Pin);
     printf("Status LED Toggle\r\n");
    osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}





//void myappV2(void)
//{
//  HAL_UART_Transmit(&huart3,"Start MyApp\r\n",strlen("Start MyApp\r\n"), 100);
//  printf("Start MyApp\r\n");
//  
//  myappTaskHandle = osThreadNew(StartMyAppTask, NULL, &myappTask_attributes);

//}

void myapp(void)
{
  HAL_UART_Transmit(&huart3,"Start MyApp\r\n",strlen("Start MyApp\r\n"), 100);
  printf("Start MyApp\r\n");
  
  osThreadDef(myappTask, StartMyAppTask, osPriorityNormal, 0, 2048);
  myappTaskHandle = osThreadCreate(osThread(myappTask), NULL);
}

