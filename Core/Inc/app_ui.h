#ifndef __APP_UI_H__
#define __APP_UI_H__

#include "main.h"

/* OLED 界面显示接口 */
void AppUi_ShowMainMenu(uint8_t selectedIndex);
void AppUi_ShowTime(uint8_t hours, uint8_t minutes, uint8_t seconds, uint16_t year, uint8_t month, uint8_t day, uint8_t dateAdjustMode);
void AppUi_ShowGyro(int16_t gx, int16_t gy, int16_t gz, uint8_t mpu6050_ok);
void AppUi_ShowSteps(uint32_t steps);
void AppUi_Clear(void);

#endif
