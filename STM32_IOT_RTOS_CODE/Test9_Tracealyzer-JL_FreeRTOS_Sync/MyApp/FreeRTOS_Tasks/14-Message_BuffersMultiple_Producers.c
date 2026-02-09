#include "MyApp.h"
#ifdef __TEST14
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

/* *************************** Message Buffer Handle *********************** */
MessageBufferHandle_t MessageBuffer_Handle;

/* *************************** Tasks Handles ****************************** */
TaskHandle_t Producer01_Handle;
TaskHandle_t Producer02_Handle;

TaskHandle_t Consumer_Handle;

/* *************************** Task Functions ******************************* */
void Producer01(void* pv)
{
	uint8_t msg[8];
	memset(msg, 'a', sizeof(msg));

	for(;;)
	{
		if(xMessageBufferSend(MessageBuffer_Handle, msg, sizeof(msg), pdMS_TO_TICKS(100)) != pdPASS)
		{
			HAL_UART_Transmit(&huart1, (uint8_t*)"P1 Message Send Failed\n", 24, HAL_MAX_DELAY);
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
		}

		vTaskDelay(pdMS_TO_TICKS(400));
	}
}

void Producer02(void* pv)
{
	uint8_t msg[24];
	memset(msg, 'B', sizeof(msg));

	for(;;)
	{
		if(xMessageBufferSend(MessageBuffer_Handle, msg, sizeof(msg), pdMS_TO_TICKS(100)) != pdPASS)
		{
			HAL_UART_Transmit(&huart1, (uint8_t*) "P2 Message Send Failed\n", 24, HAL_MAX_DELAY);
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
		}

		vTaskDelay(pdMS_TO_TICKS(900));
	}
}

void Consumer(void* pv)
{
	uint8_t rxBuffer[64];
	size_t receivedBytes;
	size_t freeSpaceBytes;

	for(;;)
	{
		// Block until a message arrives
		receivedBytes = xMessageBufferReceive(MessageBuffer_Handle, rxBuffer, sizeof(rxBuffer), portMAX_DELAY);

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);

		// Print received message
		char log[80];
		sprintf(log,"Consumer: Received msg (len=%u): %.*s\n",(unsigned)receivedBytes ,(unsigned)receivedBytes, rxBuffer);
		HAL_UART_Transmit(&huart1, (uint8_t*)log, strlen(log), HAL_MAX_DELAY);

		// Show buffer usage
		freeSpaceBytes = xMessageBufferSpacesAvailable(MessageBuffer_Handle);
		sprintf(log,"Consumer: Free space = %u bytes\n", (unsigned)freeSpaceBytes);
		HAL_UART_Transmit(&huart1, (uint8_t*)log, strlen(log), HAL_MAX_DELAY);

		vTaskDelay(pdMS_TO_TICKS(100));  //
	}
}


void init_Message_BuffersMultiple_Producers(void)
{
  /* ************************* Create Message Buffer ****************************** */
  MessageBuffer_Handle = xMessageBufferCreate(256);
	if (MessageBuffer_Handle == NULL) {
		HAL_UART_Transmit(&huart1, (uint8_t*)"Message Buffer Creation Failed\n", 32, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart1, (uint8_t*)"Message Buffer Created Successfully\n", 36, HAL_MAX_DELAY);
	}

    /* ************************** Create Tasks ********************************** */
	xTaskCreate(Producer01, "Producer01", 256, NULL, 1, &Producer01_Handle);
	xTaskCreate(Producer02, "Producer02", 256, NULL, 1, &Producer02_Handle);

	xTaskCreate(Consumer, "Consumer", 256, NULL, 2, &Consumer_Handle);

}
#endif
