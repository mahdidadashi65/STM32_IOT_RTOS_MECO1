#include "MyApp.h"
#ifdef __TEST17
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "usart.h"
#include "string.h"

uint8_t rx_data = 0; // Variable to store UART received data

/* *********************** Stream Buffer Configurations ************************ */
#define STREAM_BUFFER_SIZE 64
#define TRIGGER_LEVEL	   1

/* ****************************** Stream Buffer Handle ************************* */
StreamBufferHandle_t StreamBuffer_Handle;

void ConsumerTask(void *pvParameters)
{
	char rxBuffer[32];
	for(;;)
	{
		size_t bytesReceived = xStreamBufferReceive( //  Receive data from the stream buffer
				StreamBuffer_Handle,
				(void*)rxBuffer,
				sizeof(rxBuffer),
				portMAX_DELAY );

		if (bytesReceived > 0) {
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13); // Toggle LED on successful Receive (Consumer)
			HAL_UART_Transmit(&huart1, (uint8_t*)rxBuffer, bytesReceived, HAL_MAX_DELAY);
		}
	}
}

// This callback is called by the HAL_UART_IRQHandler when the UART receive interrupt occurs
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{

	const char *msg = "\nUART Received\n";

	if(rx_data == 'r')
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		// Send data to the stream buffer from ISR
		xStreamBufferSendFromISR(
				StreamBuffer_Handle,
				(void*) msg,
				strlen(msg),
				&xHigherPriorityTaskWoken);

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}

    // Restart UART Reception in Interrupt mode
	HAL_UART_Receive_IT(huart, &rx_data, 1);
}


void init_Stream_BufferISR_Task_Communication(void)
{
  /* ************************** Create Stream Buffer ************************** */
  StreamBuffer_Handle = xStreamBufferCreate(STREAM_BUFFER_SIZE, TRIGGER_LEVEL);
  if(StreamBuffer_Handle == NULL)
  {
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Stream Buffer Creation Failed\r\n", 30, HAL_MAX_DELAY);
  }else{
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Stream Buffer Created Successfully\r\n", 37, HAL_MAX_DELAY);
  }

  /* ************************** Create Consumer Task ************************** */
  xTaskCreate(ConsumerTask, "ConsumerTask", 256, NULL, 2, NULL);

  /* Start UART Reception in Interrupt mode */
  HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}
#endif
