/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ssd1306.h
  * @brief   SSD1306 OLED display driver header
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef __SSD1306_H__
#define __SSD1306_H__

#include "main.h"

#define SSD1306_I2C_ADDR        (0x3C << 1)
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

typedef enum {
  SSD1306_COLOR_BLACK = 0x00,
  SSD1306_COLOR_WHITE = 0x01
} SSD1306_COLOR_t;

void ssd1306_Init(void);
void ssd1306_UpdateScreen(void);
void ssd1306_Fill(uint8_t color);
void ssd1306_Clear(void);
void ssd1306_GotoXY(uint8_t x, uint8_t y);
void ssd1306_PutChar(char ch);
void ssd1306_PutString(const char* str);
void ssd1306_PutBigString2x(const char* str, uint8_t x, uint8_t y);
void ssd1306_PutBigString4x(const char* str, uint8_t x, uint8_t y);
void ssd1306_PutBigStringScaled(const char* str, uint8_t x, uint8_t y, uint8_t scale, uint8_t spacing);

#endif /* __SSD1306_H__ */
