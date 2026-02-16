#include "MyApp.h"
#ifdef __TEST09
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"


uint32_t txValue = 100;

/* *************************** Task Handles ******************************* */
TaskHandle_t Task01_Handle;

/* *************************** Task Functions ***************************** */
void Task01(void* pvParameters)
{
	uint32_t notificationValue = 0;
	for(;;)
	{

		// Block here until at least one notification is pending
		xTaskNotifyWait(pdFALSE, pdFALSE, &notificationValue, portMAX_DELAY);

		// notificationValue tells how many times ISR triggered since last check
		char str[50];
		sprintf(str, "Task01: Received %lu notifications from ISR\n", notificationValue);
		HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);


		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_0)
  {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

	//xTaskNotifyFromISR(Task01_Handle, 0, eIncrement, &xHigherPriorityTaskWoken); // Increment notification value by 1

	xTaskNotifyFromISR(Task01_Handle, txValue, eSetValueWithOverwrite, &xHigherPriorityTaskWoken); // Set notification value to txValue
	txValue++;

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void init_Direct_to_Task_Notifications_Event_Counter_Task_Notification(void)
{
  xTaskCreate(Task01, "Task01", 128, NULL, 1, &Task01_Handle);
}
#endif
