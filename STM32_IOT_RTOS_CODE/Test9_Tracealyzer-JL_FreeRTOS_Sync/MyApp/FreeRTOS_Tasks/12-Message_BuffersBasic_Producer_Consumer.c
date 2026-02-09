#include "MyApp.h"
#ifdef __TEST12
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"


/* *********************** Create Message buffer handler ********************* */
MessageBufferHandle_t MessageBuffer_Handle;

/* *********************** Create Task Handles ******************************* */
TaskHandle_t Producer_Handle;
TaskHandle_t Consumer_Handle;

/* *********************** Task Functions ********************************** */
void Producer(void* pv)
{
	const char* msgs[] = {"CMD1", "HELLO MP CODING", "PAYLAOD:12345", "OK" };
	int i = 0;

	for(;;)
	{
		size_t len = strlen(msgs[i]);
		if (xMessageBufferSend(MessageBuffer_Handle, (void* )msgs[i], len, pdMS_TO_TICKS(100)) != len) {
			// Error
			HAL_UART_Transmit(&huart1, (uint8_t *)"Message Buffer Send Failed\r\n", 21, HAL_MAX_DELAY);
		}else {

			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

			char* msg = (char*) pvPortMalloc(100 * sizeof(char)); // Allocate memory from the heap
			sprintf(msg, "Message Sent: %s\r\n", msgs[i]);
			HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
			vPortFree(msg); // Free the allocated memory
		}

		i = (i+1) % (sizeof(msgs)/sizeof(msgs[0]));

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void Consumer(void* pv)
{
	uint8_t rxBuffer[100];
	for (;;)
	{
		size_t received = xMessageBufferReceive(MessageBuffer_Handle, (void* )rxBuffer, sizeof(rxBuffer), portMAX_DELAY);

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		char* msg = (char*) pvPortMalloc(100 * sizeof(char)); // Allocate memory from the heap

		sprintf(msg, "Message Received: %.*s\n", (int)received, rxBuffer);

		HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		vPortFree(msg); // Free the allocated memory

	}
}


void init_Message_BuffersBasic_Producer_Consumer(void)
{

  /* *********************** Create Message Buffer ************************** */
  MessageBuffer_Handle = xMessageBufferCreate(256); // 256 bytes buffer (Total buffer size bytes)

  if (MessageBuffer_Handle == NULL) {
	// Error
	HAL_UART_Transmit(&huart1, (uint8_t*) "Message Buffer Creation Failed\n", 31, HAL_MAX_DELAY);
  }
  else{
	HAL_UART_Transmit(&huart1, (uint8_t*) "Message Buffer Created Successfully\n", 37, HAL_MAX_DELAY);
  }

  /* *********************** Create Tasks ********************************** */
  xTaskCreate(Producer, "Producer", 256, NULL, 2, &Producer_Handle);
  xTaskCreate(Consumer, "Consumer", 256, NULL, 1, &Consumer_Handle);
}
#endif
