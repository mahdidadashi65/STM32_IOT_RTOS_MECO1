#include "MyApp.h"
#ifdef __TEST15
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "string.h"
#include "usart.h"


/* ****************************** Stream Buffer Handle ************************* */
StreamBufferHandle_t StreamBuffer_Handle;

/* ****************************** Task Handles ********************************* */
TaskHandle_t ProducerHandle;
TaskHandle_t ConsumerHandle;

#define STREAM_BUFFER_SIZE 64
#define TRIGGER_LEVEL	   1

/* ***************************** Task Functions ******************************** */
void ProducerTask(void* pvParameters)
{
	char* txData = "Hello from Producer.\n"; // 20 bytes

	for(;;)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Producing Data...\r\n", 20, HAL_MAX_DELAY);
		size_t bytesSent = xStreamBufferSend(
				StreamBuffer_Handle,
				(void *)txData,
				strlen(txData),
				pdMS_TO_TICKS(100)
		);

		if(bytesSent > 0){
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13); // Toggle LED on successful send (Producer)
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void ConsumerTask(void *pvParameters)
{
	char rxBuffer[32];
	for(;;)
	{
		size_t bytesReceived = xStreamBufferReceive( // waiting for data from Producer
				StreamBuffer_Handle,
				(void*)rxBuffer,
				sizeof(rxBuffer),
				portMAX_DELAY );

		if (bytesReceived > 0) {
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14); // Toggle LED on successful Receive (Consumer)
			HAL_UART_Transmit(&huart1, (uint8_t*)rxBuffer, bytesReceived, HAL_MAX_DELAY);
		}
	}
}


void init_Stream_BufferBasic_ProducerConsume(void)
{

  /* ************************** Create Stream Buffer ************************** */
  StreamBuffer_Handle = xStreamBufferCreate(STREAM_BUFFER_SIZE, TRIGGER_LEVEL);
  if(StreamBuffer_Handle == NULL)
  {
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Stream Buffer Creation Failed\r\n", 30, HAL_MAX_DELAY);
  }else{
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Stream Buffer Created Successfully\r\n", 37, HAL_MAX_DELAY);
  }


  /* **************************** Create Tasks ********************************** */
  xTaskCreate(ProducerTask, "Producer", 256, NULL, 2, &ProducerHandle);
  xTaskCreate(ConsumerTask, "Consumer", 256, NULL, 2, &ConsumerHandle);

}
#endif
