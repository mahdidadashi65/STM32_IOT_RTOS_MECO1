/*
	
  ******************************************************************************
  * @file 			( фаил ):   LED_KEY_TM1638.h
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):	author: Golinskiy Konstantin	e-mail: golinskiy.konstantin@gmail.com
  ******************************************************************************
  
*/

#ifndef _LED_KEY_TM1638_H
#define _LED_KEY_TM1638_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// Pin definitions - modify according to your connections
#define TM1638_DIO_PORT     GPIOE
#define TM1638_DIO_PIN      GPIO_PIN_10
#define TM1638_CLK_PORT     GPIOE
#define TM1638_CLK_PIN      GPIO_PIN_11
#define TM1638_STB_PORT     GPIOE
#define TM1638_STB_PIN      GPIO_PIN_12

// TM1638 Commands
#define TM1638_CMD_DATA_AUTO    0x40  // Auto increment address
#define TM1638_CMD_DATA_FIXED   0x44  // Fixed address
#define TM1638_CMD_DISPLAY      0x88  // Display ON, brightness bits 0-7
#define TM1638_CMD_ADDRESS      0xC0  // Starting address

// Brightness levels (0-7)
#define TM1638_BRIGHTNESS_MIN   0
#define TM1638_BRIGHTNESS_MAX   7


void TM1638_Init(void);
void TM1638_SetBrightness(uint8_t brightness);
void TM1638_Clear(void);
void TM1638_DisplayDigit(uint8_t pos, uint8_t digit, bool dp);
void TM1638_DisplayRaw(uint8_t pos, uint8_t segments);
void TM1638_DisplayNumber(uint32_t number);
void TM1638_DisplayHex(uint32_t number);
void TM1638_SetLED(uint8_t pos, bool state);
uint8_t TM1638_ReadButtons(void);
void TM1638_DisplayText(const char* text);


#ifdef __cplusplus
}
#endif

#endif	/*	_LED_KEY_TM1638_H */
