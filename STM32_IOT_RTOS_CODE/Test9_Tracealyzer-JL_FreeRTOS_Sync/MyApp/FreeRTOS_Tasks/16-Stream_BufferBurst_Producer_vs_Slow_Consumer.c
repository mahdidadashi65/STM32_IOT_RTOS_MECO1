#include "MyApp.h"
#ifdef __TEST16
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "usart.h"

#include "string.h"

/* ****************************** Stream Buffer Handle ************************* */
StreamBufferHandle_t StreamBuffer_Handle;

/* ****************************** Task Handles ********************************* */
TaskHandle_t ProducerHandle;
TaskHandle_t ConsumerHandle;

#define STREAM_BUFFER_SIZE 64
#define TRIGGER_LEVEL	   1

/* ****************************** BurstProducer Task ******************************** */
void BurstProducer(void *pvParameters)
{
	uint8_t txData[5];
	uint8_t i = 0;
	char setChars[] = {'A','B','C','D','E'};

	xStreamBufferReset(StreamBuffer_Handle); // Reset Stream Buffer before using

	for(;;)
	{
		memset(txData, setChars[i] , sizeof(txData));
		// Send data to Stream Buffer
		size_t sent = xStreamBufferSend(StreamBuffer_Handle,
		                                txData,
		                                sizeof(txData),
		                                pdMS_TO_TICKS(100));

		if(sent < sizeof(txData)) {
			HAL_UART_Transmit(&huart1, (uint8_t*) "Buffer Overflow - Not all sent\n", 32, HAL_MAX_DELAY);
		}

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

		i++;
		if(i > 4) i = 0;

		vTaskDelay(pdMS_TO_TICKS(100)); // Very fast producer
	}
}

/* ****************************** SlowConsumer Task ******************************** */
void SlowConsumer(void *pvParameters)
{
	uint8_t rxData[20];
	memset(rxData, 0, sizeof(rxData));
	for(;;)
	{
		size_t recvd = xStreamBufferReceive(StreamBuffer_Handle,
		                                    rxData,
		                                    sizeof(rxData),
		                                    portMAX_DELAY);

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		HAL_UART_Transmit(&huart1, rxData, recvd, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1,(uint8_t*)"\n",1,HAL_MAX_DELAY);

		vTaskDelay(pdMS_TO_TICKS(1000));  // Slow consumer
	}
}


void init_Stream_BufferBurst_Producer_vs_Slow_Consumer(void)
{
  /* ************************** Create Stream Buffer ************************** */
  StreamBuffer_Handle = xStreamBufferCreate(STREAM_BUFFER_SIZE, TRIGGER_LEVEL);
  if(StreamBuffer_Handle == NULL)
  {
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Stream Buffer Creation Failed\n", 30, HAL_MAX_DELAY);
  }else{
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Stream Buffer Created Successfully\n", 37, HAL_MAX_DELAY);
  }

  /* **************************** Create Tasks ********************************** */
  xTaskCreate(BurstProducer, "Producer", 256, NULL, 2, &ProducerHandle);
  xTaskCreate(SlowConsumer,  "Consumer", 256, NULL, 2, &ConsumerHandle);
}
#endif
