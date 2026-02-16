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

#include "mb.h"
#include "mbport.h"


#define RXBUFFERSIZE 1
/* Buffer used for reception */
uint8_t aRxBuffer[RXBUFFERSIZE];


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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  printf("RX:%X %c\r\n",aRxBuffer[0],aRxBuffer[0]);
  if(HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
  {
    Error_Handler();
  }
}



void test_send_uart_modbus(void)
{
  uint8_t data[10]={1,2,3};
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,1);
  HAL_UART_Transmit(&huart2,(uint8_t*)data,10,100);	
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port,RS485_DIR_Pin,0);
}

//********************




#define REG_INPUT_START 0
#define REG_INPUT_NREGS 10

static USHORT usRegInputStart = REG_INPUT_START;
static USHORT usRegInputBuf[REG_INPUT_NREGS];

void Test_ModbusRTU(void)
{ 
  /* ABCDEF */
  usRegInputBuf[0] = 11;
  usRegInputBuf[1] = 22;
  usRegInputBuf[2] = 33;
  usRegInputBuf[3] = 44;
  usRegInputBuf[4] = 55;
  usRegInputBuf[5] = 66;
  usRegInputBuf[6] = 77;
  usRegInputBuf[7] = 88;
  usRegInputBuf[8] = 99;
  usRegInputBuf[9] = 100;  
  
  eMBErrorCode eStatus = eMBInit( MB_RTU, 5, 3, 19200, MB_PAR_NONE );
  eStatus = eMBEnable();
  
  while(1) 
  {
    eMBPoll(); 
    //HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin); 
    //HAL_Delay(100);    
  }
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
				
				//HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
    }
    else
    {
			  //HAL_GPIO_TogglePin(LD5_GPIO_Port, LD5_Pin);
        eStatus = MB_ENOREG;			
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    return MB_ENOREG;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}


__attribute__((weak,noreturn))
void __aeabi_assert (const char *expr, const char *file, int line) {
  char str[12], *p;

  fputs("*** assertion failed: ", stderr);
  fputs(expr, stderr);
  fputs(", file ", stderr);
  fputs(file, stderr);
  fputs(", line ", stderr);

  p = str + sizeof(str);
  *--p = '\0';
  *--p = '\n';
  while (line > 0) {
    *--p = '0' + (line % 10);
    line /= 10;
  }
  fputs(p, stderr);

  abort();
}

__attribute__((weak))
void abort(void) {
  for (;;);
}
//**********************

void MyApp(void)
{		
  
  Log("Start\r\n");
  
//  if(HAL_UART_Receive_IT(&huart2, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
//  {
//    Error_Handler();
//  }
  
  Test_ModbusRTU();
  
	while(1)
	{
    HAL_GPIO_TogglePin(LED_G_GPIO_Port,LED_G_Pin);
    Log("Toggle\r\n");
    //test_send_uart_modbus();
    HAL_Delay(1000);
	}	
}
