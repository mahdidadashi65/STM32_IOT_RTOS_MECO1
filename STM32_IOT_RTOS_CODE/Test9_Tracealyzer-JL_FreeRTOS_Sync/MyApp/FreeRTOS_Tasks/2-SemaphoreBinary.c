#include "MyApp.h"
#ifdef __TEST02
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"

TaskHandle_t Task01Handle;
TaskHandle_t Task02Handle;

SemaphoreHandle_t BinarySemHandle;

void StartTask01(void *argument);
void StartTask02(void *argument);


void init_SemaphoreBinary(void)
{
  
//  /* EXTI interrupt init*/
//  /* Why 0 -> 6 */
//  /* Priority 0 is the highest priority */
//  // Set EXTI0 interrupt priority to 6 (safe for FreeRTOS).
//  // Priority 0 (highest) is unsafe as it can interrupt FreeRTOS kernel,
//  // causing crashes or task freezes (e.g., Task2 stopping).
//  // FreeRTOS requires interrupts using APIs like xSemaphoreGiveFromISR
//  // to have a priority >= configMAX_SYSCALL_INTERRUPT_PRIORITY (e.g., 5 or 6).
//  HAL_NVIC_SetPriority(EXTI0_IRQn, 6, 0);
//  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  
  /* Create the binary semaphore */
  BinarySemHandle = xSemaphoreCreateBinary();
	if (BinarySemHandle == NULL) 
  {
		/* Semaphore was not created successfully */
		HAL_UART_Transmit(&huart1, (uint8_t *)"Failed to create semaphore\r\n", 28, HAL_MAX_DELAY);
	}
  else
  {
		HAL_UART_Transmit(&huart1,(uint8_t*) "Semaphore created successfully\r\n", 32,HAL_MAX_DELAY);
	}

	/* Create the tasks */
	xTaskCreate(StartTask01, "TASK01", 128, NULL, 2, &Task01Handle);
	xTaskCreate(StartTask02, "TASK02", 128, NULL, 1, &Task02Handle);

}



void StartTask01(void* argument)
{
	for(;;)
  {
		// Wait for the semaphore (released by EXTI)
		if (xSemaphoreTake(BinarySemHandle, portMAX_DELAY) == pdTRUE)
    {
			// Semaphore acquired, toggle LED
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
		}
	}
}

void StartTask02(void *argument) 
{
	for(;;)
  {
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
		vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1000ms
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_0)
  {
	  // Release the binary semaphore
	  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	  xSemaphoreGiveFromISR(BinarySemHandle, &xHigherPriorityTaskWoken);

	  // Perform context switch if needed
	  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }
}
#endif


