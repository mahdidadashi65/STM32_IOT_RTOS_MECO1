/* TM1638 LED & Key Driver for STM32 using GPIO
 * No SPI hardware required - bit-banged implementation
 */

#ifndef TM1638_H
#define TM1638_H

#include "main.h"  // Change to your STM32 family (f1xx, f4xx, etc.)
#include <stdint.h>
#include <stdbool.h>
#include "LED_KEY_TM1638.h"


// 7-segment digit encoding (0-9, A-F)
static const uint8_t TM1638_DIGIT_TABLE[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F, // 9
    0x77, // A
    0x7C, // b
    0x39, // C
    0x5E, // d
    0x79, // E
    0x71  // F
};

// Pin control macros
#define TM1638_CLK_HIGH()   HAL_GPIO_WritePin(TM1638_CLK_PORT, TM1638_CLK_PIN, GPIO_PIN_SET)
#define TM1638_CLK_LOW()    HAL_GPIO_WritePin(TM1638_CLK_PORT, TM1638_CLK_PIN, GPIO_PIN_RESET)
#define TM1638_DIO_HIGH()   HAL_GPIO_WritePin(TM1638_DIO_PORT, TM1638_DIO_PIN, GPIO_PIN_SET)
#define TM1638_DIO_LOW()    HAL_GPIO_WritePin(TM1638_DIO_PORT, TM1638_DIO_PIN, GPIO_PIN_RESET)
#define TM1638_STB_HIGH()   HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_SET)
#define TM1638_STB_LOW()    HAL_GPIO_WritePin(TM1638_STB_PORT, TM1638_STB_PIN, GPIO_PIN_RESET)
#define TM1638_DIO_READ()   HAL_GPIO_ReadPin(TM1638_DIO_PORT, TM1638_DIO_PIN)

// Small delay for timing (adjust based on your clock speed)
static inline void TM1638_Delay(void) {
    for(volatile int i = 0; i < 10; i++);
}

// Initialize GPIO pins
void TM1638_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // Enable GPIO clocks (modify according to your MCU)
   // __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // Configure CLK and STB as output
    GPIO_InitStruct.Pin = TM1638_CLK_PIN | TM1638_STB_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TM1638_CLK_PORT, &GPIO_InitStruct);
    
    // Configure DIO as output initially
    GPIO_InitStruct.Pin = TM1638_DIO_PIN;
    HAL_GPIO_Init(TM1638_DIO_PORT, &GPIO_InitStruct);
    
    // Set initial states
    TM1638_STB_HIGH();
    TM1638_CLK_HIGH();
    TM1638_DIO_HIGH();
    
    // Initialize display
    TM1638_SetBrightness(3);
    TM1638_Clear();
}

// Set DIO pin as input for reading
static void TM1638_DIO_Input(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TM1638_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(TM1638_DIO_PORT, &GPIO_InitStruct);
}

// Set DIO pin as output for writing
static void TM1638_DIO_Output(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TM1638_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TM1638_DIO_PORT, &GPIO_InitStruct);
}

// Write a byte to TM1638
static void TM1638_WriteByte(uint8_t data) {
    TM1638_DIO_Output();
    
    for(uint8_t i = 0; i < 8; i++) {
        TM1638_CLK_LOW();
        TM1638_Delay();
        
        if(data & 0x01) {
            TM1638_DIO_HIGH();
        } else {
            TM1638_DIO_LOW();
        }
        
        data >>= 1;
        TM1638_Delay();
        TM1638_CLK_HIGH();
        TM1638_Delay();
    }
}

// Read a byte from TM1638
static uint8_t TM1638_ReadByte(void) {
    uint8_t data = 0;
    
    TM1638_DIO_Input();
    
    for(uint8_t i = 0; i < 8; i++) {
        TM1638_CLK_LOW();
        TM1638_Delay();
        
        data >>= 1;
        if(TM1638_DIO_READ() == GPIO_PIN_SET) {
            data |= 0x80;
        }
        
        TM1638_CLK_HIGH();
        TM1638_Delay();
    }
    
    TM1638_DIO_Output();
    return data;
}

// Send command to TM1638
static void TM1638_SendCommand(uint8_t cmd) {
    TM1638_STB_LOW();
    TM1638_WriteByte(cmd);
    TM1638_STB_HIGH();
}

// Set display brightness (0-7)
void TM1638_SetBrightness(uint8_t brightness) {
    if(brightness > 7) brightness = 7;
    TM1638_SendCommand(TM1638_CMD_DISPLAY | brightness);
}

// Clear display
void TM1638_Clear(void) {
    TM1638_SendCommand(TM1638_CMD_DATA_AUTO);
    
    TM1638_STB_LOW();
    TM1638_WriteByte(TM1638_CMD_ADDRESS);
    for(uint8_t i = 0; i < 16; i++) {
        TM1638_WriteByte(0x00);
    }
    TM1638_STB_HIGH();
}

// Write data to specific address
void TM1638_WriteData(uint8_t addr, uint8_t data) {
    TM1638_SendCommand(TM1638_CMD_DATA_FIXED);
    
    TM1638_STB_LOW();
    TM1638_WriteByte(TM1638_CMD_ADDRESS | (addr & 0x0F));
    TM1638_WriteByte(data);
    TM1638_STB_HIGH();
}

// Display a digit at position (0-7)
void TM1638_DisplayDigit(uint8_t pos, uint8_t digit, bool dp) {
    if(pos > 7) return;
    
    uint8_t segments = 0;
    if(digit < 16) {
        segments = TM1638_DIGIT_TABLE[digit];
    }
    
    if(dp) segments |= 0x80;  // Add decimal point
    
    TM1638_WriteData(pos * 2, segments);
}

// Display a raw segment pattern at position
void TM1638_DisplayRaw(uint8_t pos, uint8_t segments) {
    if(pos > 7) return;
    TM1638_WriteData(pos * 2, segments);
}

// Display an 8-digit number
void TM1638_DisplayNumber(uint32_t number) {
    for(int8_t i = 7; i >= 0; i--) {
        TM1638_DisplayDigit(i, number % 10, false);
        number /= 10;
    }
}

// Display hex number
void TM1638_DisplayHex(uint32_t number) {
    for(int8_t i = 7; i >= 0; i--) {
        TM1638_DisplayDigit(i, number & 0x0F, false);
        number >>= 4;
    }
}

// Control LED at position (0-7)
void TM1638_SetLED(uint8_t pos, bool state) {
    if(pos > 7) return;
    TM1638_WriteData(pos * 2 + 1, state ? 0x01 : 0x00);
}

// Read button states (returns 8-bit value, 1 bit per button)
uint8_t TM1638_ReadButtons(void) {
    uint8_t buttons = 0;
    
    TM1638_STB_LOW();
    TM1638_WriteByte(0x42);  // Read key command
    
//    for(uint8_t i = 0; i < 4; i++) 
//    {
//        uint8_t data = TM1638_ReadByte();
//        buttons |= ((data & 0x01) << i);
//        buttons |= ((data & 0x10) << (i + 1));
//    }
    uint32_t key=0;
    uint8_t data[4];
    for(uint8_t i = 0; i < 4; i++) 
    {
      data[i] = TM1638_ReadByte();
    }
    key = data[0]+(data[1]*0x100)+(data[2]*0x10000)+(data[3]*0x1000000);

    buttons = buttons | ( key & 0x00000001 );
    buttons = buttons | ( (key >> 7 ) & 0x00000002 );
    buttons = buttons | ( (key >> 14 ) & 0x00000004 );
    buttons = buttons | ( (key >> 21 ) & 0x00000008 );
    buttons = buttons | ( (key >> 0 ) & 0x00000010 );
    buttons = buttons | ( (key >> 7 ) & 0x00000020 );
    buttons = buttons | ( (key >> 14 ) & 0x00000040 );
    buttons = buttons | ( (key >> 21 ) & 0x00000080 );
    
    
    TM1638_STB_HIGH();
    return buttons;
}

// Display text (up to 8 characters, only 0-9 and A-F supported)
void TM1638_DisplayText(const char* text) {
    uint8_t pos = 0;
    while(*text && pos < 8) {
        uint8_t digit = 0;
        if(*text >= '0' && *text <= '9') {
            digit = *text - '0';
        } else if(*text >= 'A' && *text <= 'F') {
            digit = *text - 'A' + 10;
        } else if(*text >= 'a' && *text <= 'f') {
            digit = *text - 'a' + 10;
        }
        TM1638_DisplayDigit(pos++, digit, false);
        text++;
    }
}

#endif // TM1638_H
