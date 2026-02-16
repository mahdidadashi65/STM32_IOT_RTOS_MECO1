#include "MyApp.h"
#ifdef __TEST11
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"


/* *************************** Task Handles ******************************* */
TaskHandle_t Task01_Handle;
TaskHandle_t Task02_Handle;
TaskHandle_t Task03_Handle;

/* *************************** Task Functions ***************************** */
void Task01(void* pvParameters)
{
	for(;;)
	{
		xTaskNotifyGive(Task02_Handle);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
		HAL_UART_Transmit(&huart1, (uint8_t *)"Task 1: Notification Sent\n", 27, HAL_MAX_DELAY);

		vTaskDelay(1000);
	}
}

void Task02(void* pvParameters)
{
	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		xTaskNotifyGive(Task03_Handle);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
		HAL_UART_Transmit(&huart1, (uint8_t*) "Task 2: Notification Received\n", 30, HAL_MAX_DELAY);
	}
}

void Task03(void* pvParameters)
{
	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);
		HAL_UART_Transmit(&huart1, (uint8_t*) "Task 3: Notification Received\n", 30, HAL_MAX_DELAY);
	}
}


void init_Direct_to_Task_Notifications_Task_to_Task_Notification(void)
{
  /* ********************** Create Tasks ************************ */
  xTaskCreate(Task01, "T1", 128, NULL, 2, &Task01_Handle);
  xTaskCreate(Task02, "T2", 128, NULL, 1, &Task02_Handle);
  xTaskCreate(Task03, "T3", 128, NULL, 0, &Task03_Handle);
}
#endif
