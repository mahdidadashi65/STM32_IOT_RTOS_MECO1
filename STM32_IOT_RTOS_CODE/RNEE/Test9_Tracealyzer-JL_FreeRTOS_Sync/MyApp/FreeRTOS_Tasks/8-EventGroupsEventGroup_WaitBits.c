#include "MyApp.h"
#ifdef __TEST08
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "usart.h"


/* ********************* Task Handles *************************************** */
TaskHandle_t Task01Handle = NULL;
TaskHandle_t Task02Handle = NULL;
TaskHandle_t Task03Handle = NULL;

TaskHandle_t Tasks_WatchDogHandle = NULL;

/* ********************* Event Group Handle ********************************** */
EventGroupHandle_t xEventBits;

const uint32_t task01_id = ( 1 << 0 ); // 0x01 bit 0
const uint32_t task02_id = ( 1 << 1 ); // 0x02 bit 1
const uint32_t task03_id = ( 1 << 2 ); // 0x04 bit 2

const uint32_t all_sync_bits = ( task01_id | task02_id | task03_id ); // 0x07 bits 0, 1, 2

void Task01(void* pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
		xEventGroupSetBits(xEventBits, task01_id);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void Task02(void *pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
		xEventGroupSetBits(xEventBits, task02_id);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void Task03(void *pvParameters)
{
	int count = 0;
	for(;;)
	{
		if (count >= 4 && count <= 9) // every 4th to 9th iteration the task will not set its bit
		{
			// do nothing, simulating a task failure
		} else
		{
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);
			xEventGroupSetBits(xEventBits, task03_id);
		}
		count++;
		if(count > 11) count = 0;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void Tasks_WatchDog(void *pvParameters)
{
	for(;;)
	{
		uint32_t result = xEventGroupWaitBits(
				xEventBits,         // Event group handle
				all_sync_bits,      // Bits to wait for
				pdTRUE,	            // Clear bits on exit
				pdTRUE,             // Wait for all bits
				pdMS_TO_TICKS(2000) // Wait time
		);

		if ((result & all_sync_bits) == all_sync_bits) {
			HAL_UART_Transmit(&huart1, (uint8_t*) "All tasks are running\n", 23, HAL_MAX_DELAY);
		} else {

			if ((result & task01_id) != task01_id) {
				HAL_UART_Transmit(&huart1, (uint8_t*) "Task01 is not running. Task01 failure. \n", 40 , HAL_MAX_DELAY);
			}
			if ((result & task02_id) != task02_id) {
				HAL_UART_Transmit(&huart1, (uint8_t*) "Task02 is not running. Task02 failure. \n", 40 , HAL_MAX_DELAY);
			}
			if ((result & task03_id) != task03_id) {
				HAL_UART_Transmit(&huart1, (uint8_t*) "Task03 is not running. Task03 failure. \n", 40 , HAL_MAX_DELAY);
			}
		}
	}
}

void init_EventGroupsEventGroup_WaitBits(void)
{

  /* *********************** Create Event Group ************************** */
  xEventBits = xEventGroupCreate();

  if (xEventBits == NULL) {
	HAL_UART_Transmit(&huart1, (uint8_t *)"Event Group was not created\n", 29, HAL_MAX_DELAY);
  } else {
	HAL_UART_Transmit(&huart1, (uint8_t *)"Event Group was created\n", 26, HAL_MAX_DELAY);
  }

  /* *********************** Create Tasks ******************************** */
  xTaskCreate(Task01, "T1", 128, NULL, 1, &Task01Handle);
  xTaskCreate(Task02, "T2", 128, NULL, 1, &Task02Handle);
  xTaskCreate(Task03, "T3", 128, NULL, 1, &Task03Handle);

  xTaskCreate(Tasks_WatchDog, "WD", 128, NULL, 2, &Tasks_WatchDogHandle);
}
#endif
