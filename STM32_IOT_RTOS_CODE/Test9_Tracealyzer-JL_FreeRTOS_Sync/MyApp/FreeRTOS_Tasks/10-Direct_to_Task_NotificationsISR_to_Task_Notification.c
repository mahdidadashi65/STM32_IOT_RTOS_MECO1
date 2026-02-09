#include "MyApp.h"
#ifdef __TEST10
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

/* *************************** Task Functions ***************************** */
void Task01(void* pvParameters)
{
	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
		HAL_UART_Transmit(&huart1, (uint8_t *)"Task 1: Notification Sent\n", 27, HAL_MAX_DELAY);

		vTaskDelay(1000);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_0)
  {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
	vTaskNotifyGiveFromISR(Task01_Handle, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void init_Direct_to_Task_NotificationsISR_to_Task_Notification(void)
{
  /* ********************** Create Tasks ************************ */
  xTaskCreate(Task01, "T1", 128, NULL, 1, &Task01_Handle);
}
#endif
