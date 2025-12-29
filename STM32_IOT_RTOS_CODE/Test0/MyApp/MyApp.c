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
#include <string.h>
#include <stdio.h>
#include "usart.h"

void Log(char* log)
{
   HAL_UART_Transmit(&huart3,(uint8_t*)log,strlen(log),100);	
}


void test_send_uart_modbus(void)
{
  uint8_t data[10]={1,2,3};
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,1);
  HAL_UART_Transmit(&huart2,(uint8_t*)data,10,100);	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,0);
}

void MyApp(void)
{		
  
  Log("Start\r\n");
  
	while(1)
	{
    HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
    Log("Toggle\r\n");
    test_send_uart_modbus();
    HAL_Delay(1000);
	}	
}
