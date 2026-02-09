#include "MyApp.h"
#ifdef __TEST01
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "string.h"
#include "stdio.h"
#include "usart.h"

uint8_t rx_data = 0;

typedef struct {
    char* pStr;
    int value;
}QMsg;

/* ******************* TASK HANDLERS ******************* */
xTaskHandle Task01_Handle;
xTaskHandle Task02_Handle;
xTaskHandle Task03_Handle;

/* ******************* QUEUE HANDLER ******************* */
xQueueHandle Queue_Handle;

/* ******************* TASK FUNCTIONS ******************* */
void Task01_Producer(void* argument)
{
	QMsg t1Msg;

	uint32_t TickDelay = pdMS_TO_TICKS(4000); // convert ms to ticks
	while(1)
  {
		char* str = (char*) pvPortMalloc(100 * sizeof(char)); // Allocate memory from the heap

		t1Msg.pStr = "Message from T1";
		t1Msg.value = 101;

		if(xQueueSend(Queue_Handle, &t1Msg, portMAX_DELAY) == pdPASS)
    {
			//HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

			sprintf(str, "T1 Successfully sent message to the queue value:%d  Msg:%s\n", t1Msg.value, t1Msg.pStr);
			HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
		}

		vPortFree(str); // Free the allocated memory

		vTaskDelay(TickDelay);
	}
}

void Task02_Producer(void* argument)
{
	QMsg t2Msg;

	uint32_t TickDelay = pdMS_TO_TICKS(2000); // convert ms to ticks
	while(1){

		char* str = (char*) pvPortMalloc(100 * sizeof(char)); // Allocate memory from the heap

		t2Msg.pStr = "Message from T2";
		t2Msg.value = 202;

		if(xQueueSend(Queue_Handle, &t2Msg, portMAX_DELAY) == pdPASS)
    {
			//HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);

			sprintf(str, "T2 Successfully sent message to the queue value:%d  Msg:%s\n", t2Msg.value, t2Msg.pStr);
			HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

		}

		vPortFree(str); // Free the allocated memory

		vTaskDelay(TickDelay);
	}
}

void Task03_Consumer(void* argument)
{
	QMsg received; // Allocate memory for the message
	uint32_t TickDelay = pdMS_TO_TICKS(5000); // convert ms to ticks
	while(1)
  {
		char* str = (char*) pvPortMalloc(200 * sizeof(char)); // Allocate memory from the heap

		if(xQueueReceive(Queue_Handle, &received, portMAX_DELAY) != pdPASS) // Wait indefinitely until something arrives
		{
			HAL_UART_Transmit(&huart3, (uint8_t*) "Error in receiving from Queue\n", 31, HAL_MAX_DELAY);
		}
    else 
    {
			//HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_11);
			sprintf(str, "Successfully received QMsg from the queue: value:%d  Msg:%s\n\n", received.value, received.pStr);
			HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
		}

		vPortFree(str); // Free the allocated memory

		vTaskDelay(TickDelay);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	QMsg ISRMsg;

	if(rx_data == 'r')
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		ISRMsg.pStr = "Message from ISR";
		ISRMsg.value = 999;

		if (xQueueSendToFrontFromISR(Queue_Handle, &ISRMsg, &xHigherPriorityTaskWoken) == pdPASS) // if queue is full, it will block.
		{
			HAL_UART_Transmit(huart, (uint8_t*) "\nSent from ISR\n\n", 17, HAL_MAX_DELAY);
		}
    else 
    {
			HAL_UART_Transmit(huart, (uint8_t*) "\nCould not send from ISR Queue Full\n\n", 38, HAL_MAX_DELAY); // queue full
		}

		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}

    // Restart UART Reception in Interrupt mode
	HAL_UART_Receive_IT(huart, &rx_data, 1);
}



void init_QueueSimpleQueue(void)
{
  /* ********************* Create integer QUEUE ********************* */
  Queue_Handle = xQueueCreate(5, sizeof(QMsg));
  if (Queue_Handle == NULL) 
  {
    // Queue was not created and must not be used.
    HAL_UART_Transmit(&huart3, (uint8_t *)"Queue was not created and must not be used.\n", 43, HAL_MAX_DELAY);
  }
  else
  {
    HAL_UART_Transmit(&huart3, (uint8_t*) "Queue created successfully.\n", 30, HAL_MAX_DELAY);
  }

  /* ********************* Create Tasks ********************* */
  xTaskCreate(Task01_Producer, "T1", 256, NULL, 3, &Task01_Handle);
  xTaskCreate(Task02_Producer, "T2", 256, NULL, 2, &Task02_Handle);
  xTaskCreate(Task03_Consumer, "T3", 256, NULL, 1, &Task03_Handle);

  /* Start UART Reception in Interrupt mode */
  HAL_UART_Receive_IT(&huart3, &rx_data, 1);

}
#endif
