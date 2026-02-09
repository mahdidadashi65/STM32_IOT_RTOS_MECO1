#include "MyApp.h"
#ifdef __TEST13
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

/* ***************************** Create Message Buffer handle *********************** */
MessageBufferHandle_t MessageBuffer_Handle;

/* ***************************** Task Handles *************************** */
TaskHandle_t Consumer_Handle;

/* ***************************** Task Functions *************************** */
void Consumer(void *pvParameters)
{
	uint8_t rx[100];
	for(;;) {
		size_t received = xMessageBufferReceive(MessageBuffer_Handle, rx, sizeof(rx), portMAX_DELAY);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13); // consume marker

		char* msg = (char*)pvPortMalloc(100 * sizeof(char));
		sprintf(msg, "Consumer recv %.*s\n", (int)received, rx);
		HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		vPortFree(msg); // Free the allocated memory
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_0)
	{
		const char alert[] = "ALERT";
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		xMessageBufferSendFromISR(MessageBuffer_Handle, (void *)alert, sizeof(alert), &xHigherPriorityTaskWoken);

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}
}

void init_Message_BuffersISR_to_Consumer(void)
{
  /* ********************** Create Message buffer ********************* */
  MessageBuffer_Handle = xMessageBufferCreate(200);
  if (MessageBuffer_Handle == NULL) {
	 HAL_UART_Transmit(&huart1, (uint8_t *)"Message Buffer Creation Failed\n", 32, HAL_MAX_DELAY);
  }
  else{
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Message Buffer Created\n", 23, HAL_MAX_DELAY);
  }

  /* ********************** Create Task ******************************** */
  xTaskCreate(Consumer, "Consumer", 128, NULL, 1, &Consumer_Handle);
}
#endif
