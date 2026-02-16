#include "MyApp.h"
#ifdef __TEST05
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"
/* ************************* Mutex Handle ************************* */
QueueHandle_t RecursiveMutexHandle;

/* ************************* Task Handles ************************* */
TaskHandle_t Task01_Handle, Task02_Handle;

/* ************************ Some functions ************************ */
void Function_B(void)
{
	// Try to lock the mutex again inside Function_B
	if(xSemaphoreTakeRecursive(RecursiveMutexHandle, portMAX_DELAY) == pdPASS) // This will succeed since it's a recursive mutex, incrementing the lock count
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13); // indicate Function_B
		HAL_UART_Transmit(&huart1, (uint8_t*)"Task01: Enter Function_B\n", 26, HAL_MAX_DELAY);

		vTaskDelay(pdMS_TO_TICKS(500));

		HAL_UART_Transmit(&huart1, (uint8_t*)"Task01: Exit Function_B Releasing Mutex\n", 42, HAL_MAX_DELAY);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

		// Release mutex (decrement lock count)
		xSemaphoreGiveRecursive(RecursiveMutexHandle);
	}
}

void Function_A(void)
{
	// First lock in Function_A
	if(xSemaphoreTakeRecursive(RecursiveMutexHandle, portMAX_DELAY) == pdPASS)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14); // indicate Function_A
		HAL_UART_Transmit(&huart1, (uint8_t*)"Task01: Enter Function_A\n", 26, HAL_MAX_DELAY);

		vTaskDelay(pdMS_TO_TICKS(500));

		// Call nested function that also uses the same mutex
		Function_B();

		HAL_UART_Transmit(&huart1, (uint8_t*)"Task01: Exit Function_A Releasing Mutex\n", 42, HAL_MAX_DELAY);
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		// Release mutex once for Function_A
		xSemaphoreGiveRecursive(RecursiveMutexHandle);
	}
}

/* ************************* Task Functions *********************** */
void Task01(void *pvParameters)
{
	for(;;)
	{
		Function_A(); // Top-level call
		vTaskDelay(pdMS_TO_TICKS(2000)); // Run every 2s
	}
}

void Task02(void *pvParameters)
{
	for(;;)
	{
		// Task02 also wants to use the same resource
		if (xSemaphoreTakeRecursive(RecursiveMutexHandle, portMAX_DELAY) == pdPASS) 
    {
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11); // indicate Task02
			HAL_UART_Transmit(&huart1, (uint8_t*) "Task02: Acquired Mutex\n", 23, HAL_MAX_DELAY);

			vTaskDelay(pdMS_TO_TICKS(1000));

			HAL_UART_Transmit(&huart1, (uint8_t*) "Task02: Releasing Mutex\n", 25, HAL_MAX_DELAY);
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);

			xSemaphoreGiveRecursive(RecursiveMutexHandle);
		}

		vTaskDelay(pdMS_TO_TICKS(3000)); // Run every 3s
	}
}


void init_MutexRecursiveMutex(void)
{
  /* ************************ Create Tasks ************************** */
  RecursiveMutexHandle = xSemaphoreCreateRecursiveMutex();
  if (RecursiveMutexHandle == NULL) 
  {
	HAL_UART_Transmit(&huart1, (uint8_t *)"Mutex creation failed\n", 23, HAL_MAX_DELAY);
  }
  else
  {
	HAL_UART_Transmit(&huart1, (uint8_t *)"Mutex created\n", 16, HAL_MAX_DELAY);
  }

  xTaskCreate(Task01, "T1", 256, NULL, 2, &Task01_Handle);
  xTaskCreate(Task02, "T2", 256, NULL, 1, &Task02_Handle);
}
#endif
