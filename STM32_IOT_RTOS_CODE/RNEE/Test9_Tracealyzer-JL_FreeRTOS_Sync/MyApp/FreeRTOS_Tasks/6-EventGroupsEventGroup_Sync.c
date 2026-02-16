#include "MyApp.h"
#ifdef __TEST06
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "usart.h"

/* ************************ Event Group Handle ***************************** */
EventGroupHandle_t EventGroup_Handle;

/* ************************ Task Handlers ***************************** */
TaskHandle_t Task01_Handler = NULL;
TaskHandle_t Task02_Handler = NULL;
TaskHandle_t Task03_Handler = NULL;

/* ************************ Event Bits ***************************** */
const EventBits_t task01_bit = (1 << 0); // 0x01 // Bit 0
const EventBits_t task02_bit = (1 << 1); // 0x02 // Bit 1
const EventBits_t task03_bit = (1 << 2); // 0x04 // Bit 2

const EventBits_t all_sync_bits = (task01_bit | task02_bit | task03_bit); // 0x07 // Bit 0, Bit 1, Bit 2)

/* ************************ Task Functions ************************* */
void Task01(void* pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

		EventBits_t uxReturn = xEventGroupSync(
				EventGroup_Handle,  // The event group
				task01_bit, 		// The bits being set.
				all_sync_bits,		// Wait for all bits
				pdMS_TO_TICKS(2000) // Wait for 2000 ms
		);

		if((uxReturn & all_sync_bits) == all_sync_bits){
			// All three tasks reached the synchronisation point before the call
			// to xEventGroupSync() timed out.
			HAL_UART_Transmit(&huart1, (uint8_t *)"Task01: All tasks synchronized.\n", 32, HAL_MAX_DELAY);
		}else{
			// Timeout occurred before all bits were set.
			HAL_UART_Transmit(&huart1, (uint8_t*) "Task01: Synchronization timeout.\n", 33, HAL_MAX_DELAY);
		}

		vTaskDelay(pdMS_TO_TICKS(500));

	}
}

void Task02(void* pvParameters)
{
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

		EventBits_t uxReturn = xEventGroupSync(
				EventGroup_Handle,  // The event group
				task02_bit, 		// The bits being set.
				all_sync_bits,		// Wait for all bits
				pdMS_TO_TICKS(2000) // Wait for 2000 ms
		);


		if((uxReturn & all_sync_bits) == all_sync_bits){
			// All three tasks reached the synchronisation point before the call
			// to xEventGroupSync() timed out.
			HAL_UART_Transmit(&huart1, (uint8_t *)"Task02: All tasks synchronized.\n", 32, HAL_MAX_DELAY);
		}else{
			// Timeout occurred before all bits were set.
			HAL_UART_Transmit(&huart1, (uint8_t*) "Task02: Synchronization timeout.\n", 33, HAL_MAX_DELAY);
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void Task03(void* pvParameters)
{
	static int count = 0;
	for(;;)
	{
		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);

		 if (count >= 3 && count <= 6) {
			 // Simulate a failure (not calling sync)
			 HAL_UART_Transmit(&huart1, (uint8_t *)"Task03: Missed sync (failure simulation).\n", 43, HAL_MAX_DELAY);
		 }
		 else {

			EventBits_t uxReturn = xEventGroupSync(
					EventGroup_Handle,  // The event group
					task03_bit, 		// The bits being set.
					all_sync_bits,		// Wait for all bits
					pdMS_TO_TICKS(2000) // Wait for 2000 ms
			);


			if((uxReturn & all_sync_bits) == all_sync_bits){
				// All three tasks reached the synchronisation point before the call
				// to xEventGroupSync() timed out.
				HAL_UART_Transmit(&huart1, (uint8_t *)"Task03: All tasks synchronized.\n", 32, HAL_MAX_DELAY);
			}else{
				// Timeout occurred before all bits were set.
				HAL_UART_Transmit(&huart1, (uint8_t*) "Task03: Synchronization timeout.\n", 33, HAL_MAX_DELAY);
			}
		 }

		 count++;
		 if (count > 8) count = 0;

		 vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void init_EventGroupsEventGroup_Sync(void)
{
  // Create Event Group
  EventGroup_Handle = xEventGroupCreate();
	if (EventGroup_Handle == NULL) {
		// Event Group creation failed
		HAL_UART_Transmit(&huart1, (uint8_t*) "Event Group Creation Failed!\n", 28, HAL_MAX_DELAY);
	}else {
		HAL_UART_Transmit(&huart1, (uint8_t*) "Event Group Created Successfully.\n", 35, HAL_MAX_DELAY);
	}

	// Create Tasks
	xTaskCreate(Task01, "Task01", 128, NULL, 1, &Task01_Handler);
	xTaskCreate(Task02, "Task02", 128, NULL, 1, &Task02_Handler);
	xTaskCreate(Task03, "Task03", 128, NULL, 1, &Task03_Handler);
}
#endif
