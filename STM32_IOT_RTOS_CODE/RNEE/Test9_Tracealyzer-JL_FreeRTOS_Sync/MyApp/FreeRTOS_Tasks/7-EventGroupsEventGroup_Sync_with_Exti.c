#include "MyApp.h"
#ifdef __TEST07
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"


/* ***************************** Group Event Handle ************************** */
EventGroupHandle_t GroupEventHandle;

/* ***************************** Event Bits ********************************** */
const EventBits_t Task01_Bit = (1 << 0);
const EventBits_t Task02_Bit = (1 << 1);

const EventBits_t all_sync_bits = (Task01_Bit | Task02_Bit);

/* ***************************** Task Handle ********************************* */
TaskHandle_t Task01_Handle;
TaskHandle_t Task02_Handle;

/* ***************************** Task Functions ****************************** */
void Task01(void* pvParameters){

	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

		EventBits_t uxBits = xEventGroupWaitBits(
				GroupEventHandle,         // Event group handle
				all_sync_bits,            // Bits to wait for
				pdTRUE,	                  // Clear bits on exit
				pdTRUE,                   // Wait for all bits
				pdMS_TO_TICKS(2000)       // Wait time
		);

		if ((uxBits & all_sync_bits) == all_sync_bits) {
			HAL_UART_Transmit(&huart1, (uint8_t*) "All tasks are running\n", 23, HAL_MAX_DELAY);
		}else{
			if ((uxBits & Task01_Bit) != Task01_Bit) {
				HAL_UART_Transmit(&huart1, (uint8_t*) "Task01 is not running. Task01 failure. \n", 40, HAL_MAX_DELAY);
			}
			if ((uxBits & Task02_Bit) != Task02_Bit) {
				HAL_UART_Transmit(&huart1, (uint8_t*) "Task02 is not running. Task02 failure. \n", 40, HAL_MAX_DELAY);
			}
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}

void Task02(void *pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		xEventGroupSetBits(GroupEventHandle, Task02_Bit);

		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_0)
  {
	  HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);

	  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	  xEventGroupSetBitsFromISR(GroupEventHandle, Task01_Bit, &xHigherPriorityTaskWoken);

	  portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
  }
}


void init_EventGroupsEventGroup_Sync_with_Exti(void)
{
  /* ***************************** Create Event Group ************************** */
  GroupEventHandle = xEventGroupCreate();
  if(GroupEventHandle == NULL)
  {
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Failed to create Event Group\n", 30, HAL_MAX_DELAY);
  }
  else
  {
	  HAL_UART_Transmit(&huart1, (uint8_t *)"Event Group Created Successfully\n", 36, HAL_MAX_DELAY);
  }

  /* **************************** Create Tasks ******************************* */
  xTaskCreate(Task01, "T1", 128, NULL, 1, &Task01_Handle);
  xTaskCreate(Task02, "T2", 128, NULL, 1, &Task02_Handle);
}
#endif
