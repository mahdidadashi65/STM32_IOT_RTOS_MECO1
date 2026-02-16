//           ****************************************************
//          **   Processor      : STM32F107VCT6                   **
//         ***   Frequency      : 8MHz ExtClock 72MHZ CPU Clock   ***
//        ****   AUTHOR         : Mahdi Dadashi                   ****
//       *****   STM32CubeMX    : V5.40                           *****
//      ******   STM32CubeF1    : V1.8.0                          ******
//       *****   Compiler       : KEIL uVision V5.26              *****
//        ****   Instagram      : instagram.com/mahdidadashi65/   ****
//         ***   Telegram       : t.me/mahdidadashi65/            ***
//          **   Github         : github.com/mahdidadashi65/      **
//           ****************************************************
//.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''                                 ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''      www.mahdidadashi.ir        ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''                                 ''-._.-''-._.-''-._.-''-._
//.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._.-''-._

#include "main.h"
#include "MyApp.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include <math.h>

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF); 

  return ch;
}  
  

void Log(char* log)
{
   HAL_UART_Transmit(&huart3,(uint8_t*)log,strlen(log),100);	
}

typedef enum {
    STATE_LOCKED,
    STATE_UNLOCKING,
    STATE_UNLOCKED,
    STATE_LOCKING,
    STATE_ERROR,
    STATE_COUNT
} MyState_t;

char trcStateName[STATE_COUNT][20] = {"LOCKED", "UNLOCKING", "UNLOCKED", "LOCKING", "ERROR"};

static TraceStateMachineHandle_t trcStateMachine;
static TraceStateMachineStateHandle_t trcStateHandle[STATE_COUNT];

static MyState_t currentState;



void SetState(MyState_t new_state)
{
    /* Updates the state */
    currentState = new_state;
    
    /* Logs the state change as an event in TraceRecorder, using the state handle
       provided by xTraceStateMachineStateCreate(). */
    xTraceStateMachineSetState(trcStateMachine, trcStateHandle[currentState]);
    
    /* Print to the console to show something is happening... */
    //printf("State is now: %s\r\n", trcStateName[currentState]);
}

//**********************

void vTaskBlink1( void * pvParameters )
{
  Log("Task Blink1 Started\r\n");
  /* Enter an infinite loop to perform the task processing. */
  
  /* Creates the StateMachine object in TraceRecorder and obtain the trcStateMachine handle. */
    xTraceStateMachineCreate("Lock", &trcStateMachine);

    /* Registers the state names and obtain the state handles stored in the trcStateHandle array. */
    xTraceStateMachineStateCreate(trcStateMachine, trcStateName[STATE_LOCKED],    &trcStateHandle[STATE_LOCKED]);
    xTraceStateMachineStateCreate(trcStateMachine, trcStateName[STATE_UNLOCKING], &trcStateHandle[STATE_UNLOCKING]);
    xTraceStateMachineStateCreate(trcStateMachine, trcStateName[STATE_UNLOCKED],  &trcStateHandle[STATE_UNLOCKED]);
    xTraceStateMachineStateCreate(trcStateMachine, trcStateName[STATE_LOCKING],   &trcStateHandle[STATE_LOCKING]);
    xTraceStateMachineStateCreate(trcStateMachine, trcStateName[STATE_ERROR],     &trcStateHandle[STATE_ERROR]);
    
 
    
    SetState(STATE_LOCKING);   
    vTaskDelay(10);
    SetState(STATE_LOCKED);    
    vTaskDelay(20);
    SetState(STATE_UNLOCKING);    
    vTaskDelay(10);
    SetState(STATE_UNLOCKED);    
    vTaskDelay(20);
    SetState(STATE_LOCKING);    
    vTaskDelay(10);
    SetState(STATE_ERROR);    
    vTaskDelay(20);
    
  for( ;; )
  {
    HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,1);
    SetState(STATE_LOCKING);    
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    HAL_GPIO_WritePin(LED_B_GPIO_Port,LED_B_Pin,0);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    SetState(STATE_UNLOCKED); 
  }
}

void vTaskBlink2( void * pvParameters )
{
  Log("Task Blink2 Started\r\n");
  /* Enter an infinite loop to perform the task processing. */
  for( ;; )
  {
    HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,1);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
    HAL_GPIO_WritePin(LED_R_GPIO_Port,LED_R_Pin,0);
    vTaskDelay( pdMS_TO_TICKS( 1000 ) );
  }
}


    TaskHandle_t xHandle1;
    TaskHandle_t xHandle2;
    TaskHandle_t xHandle3;



//***********************










int ReadSensor(void)
{
    static double r = 0;

    r = r + 3.1415/16;
    
    return (int)(100 + 100*sin(r));
}

void vTaskAccelerometer(void *pvParameters)
{
    (void) pvParameters;
    TraceStringHandle_t sensor_chn, log_chn, counter_chn, format_string;
    int counter = 1;
    
    /* All "user events" (application log messages) are connected to a User
       Event Channel. This is specified in first argument of the xTracePrint...
       logging calls. Here we register the channel names and get the handles
       in return, used in the later logging calls. */
    xTraceStringRegister("Sensor channel", &sensor_chn);
    xTraceStringRegister("Log channel", &log_chn);
    xTraceStringRegister("Counter channel", &counter_chn);
    
    /* Registers the format string for xTracePrintF1 (see below). */ 
    xTraceStringRegister("%d", &format_string);
    
    for (;;)
    {
        
        /* xTracePrintF allows for storing multiple data arguments with
        a printf-like interface, supporting integers and strings.
        This is a lot faster than printf calls, since not doing string
        formatting in runtime and not limited by slow UART baud rates. */
        xTracePrintF(sensor_chn, "%d", ReadSensor());
        
        if (counter % 3 == 0)
        {
            /* xTracePrintF1 is an even faster logging function suitable for data logging,
               using pre-registration of the format string and a known number of data arguments.
               There are five variants, xTracePrintF0 - xTracePrintF4, for 0-4 data arguments. */           
            xTracePrintF1(counter_chn, format_string, counter);
        }
        
        if (counter % 7 == 0)
        {
            /* xTracePrint is suitable when you only need to log a string,
               without any data arguments and registering the string first.
               This is suitable for debug events and for strings created in
               runtime, for example incoming data from a network connection. 
               For very frequent events, consider using the faster option
               xTracePrintF0 instead. */
            xTracePrint(log_chn, "Some event happened");
        }
        
        counter++;
                
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}



void MyApp(void)
{		 
  
  Log("MyApp Start\r\n");
  

    /* Create the task. */
    if( xTaskCreate(
    vTaskBlink1, /* Pointer to the function that implements the task. */
    "task1", /* Text name given to the task. */
    1024, /* The size of the stack that should be created for the task.
    This is defined in words, not bytes. */
    (void*) 0,/* A reference to xParameters is used as the task parameter.
    This is cast to a void * to prevent compiler warnings. */
    0, /* The priority to assign to the newly created task. */
    &xHandle1 /* The handle to the task being created will be placed in
    xHandle. */
    ) != pdPASS )
    {
    /* The task could not be created as there was insufficient heap memory remaining. If
    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
    amount of FreeRTOS heap memory that remains unallocated can be queried using the
    xPortGetFreeHeapSize() API function.*/
           
      Log("Task1 Blink Create Err\r\n");
    }
    else
    {
      Log("Task1 Blink Created\r\n");
    }
    
    
//        /* Create the task. */
//    if( xTaskCreate(
//    vTaskBlink2, /* Pointer to the function that implements the task. */
//    "task2", /* Text name given to the task. */
//    1024, /* The size of the stack that should be created for the task.
//    This is defined in words, not bytes. */
//    (void*) 0,/* A reference to xParameters is used as the task parameter.
//    This is cast to a void * to prevent compiler warnings. */
//    0, /* The priority to assign to the newly created task. */
//    &xHandle2 /* The handle to the task being created will be placed in
//    xHandle. */
//    ) != pdPASS )
//    {
//    /* The task could not be created as there was insufficient heap memory remaining. If
//    heap_1.c, heap_2.c or heap_4.c are included in the project then this situation can be
//    trapped using the vApplicationMallocFailedHook() callback (or ‘hook’) function, and the
//    amount of FreeRTOS heap memory that remains unallocated can be queried using the
//    xPortGetFreeHeapSize() API function.*/
//           
//      Log("Task2 Blink Create Err\r\n");
//    }
//    else
//    {
//      Log("Task2 Blink Created\r\n");
//    }
      
  // vTaskDelete(NULL);
  
    xTaskCreate(
      vTaskAccelerometer,
      "vTaskAccelerometer",
      configMINIMAL_STACK_SIZE*4,
      NULL,
      tskIDLE_PRIORITY + 2,
      &xHandle3
  );
    
}







