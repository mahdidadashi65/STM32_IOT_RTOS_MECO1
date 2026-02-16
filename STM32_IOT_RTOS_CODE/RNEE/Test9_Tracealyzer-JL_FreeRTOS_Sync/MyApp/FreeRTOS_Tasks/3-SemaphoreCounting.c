#include "MyApp.h"
#ifdef __TEST03
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"

TaskHandle_t Task01_Handle;
TaskHandle_t Task02_Handle;
TaskHandle_t Task03_Handle;

SemaphoreHandle_t CountingSemaphore_Handle;

void Task01(void* arg)
{
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}

void Task02(void* arg)
{
	xSemaphoreGive(CountingSemaphore_Handle);
	xSemaphoreGive(CountingSemaphore_Handle);
	while(1)
	{
		xSemaphoreTake(CountingSemaphore_Handle, portMAX_DELAY);

		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);

		vTaskDelay(pdMS_TO_TICKS(1000));

		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
	}
}

void Task03(void* arg)
{
	while(1)
	{
		xSemaphoreTake(CountingSemaphore_Handle, portMAX_DELAY);

		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);

		vTaskDelay(pdMS_TO_TICKS(1000));

		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_0)
  {
	  // Release the binary semaphore
	  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	  xSemaphoreGiveFromISR(CountingSemaphore_Handle, &xHigherPriorityTaskWoken);

	  // Perform context switch if needed
	  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

  }
}


void init_SemaphoreCounting(void)
{
  CountingSemaphore_Handle = xSemaphoreCreateCounting(2,0);
  if(CountingSemaphore_Handle == NULL) HAL_UART_Transmit(&huart1,(uint8_t *) "Unable to create semaphore\n\n",29, 100);
  else HAL_UART_Transmit(&huart1,(uint8_t *) "Counting Semaphore created successfully\n\n", 42, 100);

  xTaskCreate(Task01, "T1", 128, NULL, 3, &Task01_Handle);
  xTaskCreate(Task02, "T2", 128, NULL, 2, &Task02_Handle);
  xTaskCreate(Task03, "T3", 128, NULL, 1, &Task03_Handle);

}
#endif
