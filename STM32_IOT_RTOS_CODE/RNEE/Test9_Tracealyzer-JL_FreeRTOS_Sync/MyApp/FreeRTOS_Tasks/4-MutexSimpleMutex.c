#include "MyApp.h"
#ifdef __TEST04
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"


SemaphoreHandle_t SimpleMutex;

TaskHandle_t Task1Handle, Task2Handle;

void Task01(void* argument);
void Task02(void* argument);

void GPIO_TOGGLE_01(void)
{
	xSemaphoreTake(SimpleMutex, portMAX_DELAY); // acquire the mutex

	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
	vTaskDelay(pdMS_TO_TICKS(500));

	xSemaphoreGive(SimpleMutex); // release the mutex
}

void GPIO_TOGGLE_02(void)
{
	xSemaphoreTake(SimpleMutex, portMAX_DELAY); // acquire the mutex

	HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
	vTaskDelay(pdMS_TO_TICKS(500));

	xSemaphoreGive(SimpleMutex); // release the mutex
}


void Task01(void* argument)
{
	while(1)
  {
		GPIO_TOGGLE_01();
		// vTaskDelay(pdMS_TO_TICKS(500)); // Add a delay to allow Task02 to run
	}
}

void Task02(void *argument)  
{
	while(1)
  {
		GPIO_TOGGLE_02();
	}
}


void init_MutexSimpleMutex(void)
{

  SimpleMutex = xSemaphoreCreateMutex();
  if (SimpleMutex == NULL) 
  {
		HAL_UART_Transmit(&huart1, (uint8_t *)"Mutex Creation Failed\r\n", 23, HAL_MAX_DELAY);
  }

   xTaskCreate(Task01, "Task01", 128, NULL, 2, &Task1Handle);
   xTaskCreate(Task02, "Task02", 128, NULL, 1, &Task2Handle);
}

#endif

