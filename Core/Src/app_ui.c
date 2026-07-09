#include "app_ui.h"
#include "ssd1306.h"
#include <stdio.h>
#include <string.h>

/* 清屏并刷新显示 */
void AppUi_Clear(void)
{
  ssd1306_Clear();
  ssd1306_UpdateScreen();
}

/* 显示主菜单 */
void AppUi_ShowMainMenu(uint8_t selectedIndex)
{
  ssd1306_Clear();
  ssd1306_GotoXY(0, 0);
  ssd1306_PutString("MAIN MENU");
  ssd1306_GotoXY(0, 16);
  ssd1306_PutString(selectedIndex == 0 ? "> CLOCK" : "  CLOCK");
  ssd1306_GotoXY(0, 32);
  ssd1306_PutString(selectedIndex == 1 ? "> MPU6050" : "  MPU6050");
  ssd1306_GotoXY(0, 48);
  ssd1306_PutString(selectedIndex == 2 ? "> STEP" : "  STEP");
  ssd1306_UpdateScreen();
}

/* 显示时钟界面 */
void AppUi_ShowTime(uint8_t hours, uint8_t minutes, uint8_t seconds, uint16_t year, uint8_t month, uint8_t day, uint8_t dateAdjustMode)
{
  char dateBuf[16];
  char timeBuf[16];
  ssd1306_Clear();
  snprintf(dateBuf, sizeof(dateBuf), "%04u.%02u.%02u", year, month, day);
  snprintf(timeBuf, sizeof(timeBuf), "%02u:%02u:%02u", hours, minutes, seconds);

  ssd1306_GotoXY(0, 0);
  ssd1306_PutString(dateBuf);
  if (dateAdjustMode)
  {
    ssd1306_GotoXY(96, 0);
    ssd1306_PutString("SET");
  }

  uint8_t scale = 1;
  uint8_t spacing = 4;
  size_t len = strlen(timeBuf);
  uint16_t textWidth = (uint16_t)len * (8 * scale) + (uint16_t)(len > 0 ? (len - 1) * spacing : 0);
  uint8_t x = (SSD1306_WIDTH - textWidth) / 2;
  uint8_t y = (SSD1306_HEIGHT - (16 * scale)) / 2;
  ssd1306_PutBigStringScaled(timeBuf, x, y, scale, spacing);
  ssd1306_UpdateScreen();
}

/* 显示陀螺仪数据 */
void AppUi_ShowGyro(int16_t gx, int16_t gy, int16_t gz, uint8_t mpu6050_ok)
{
  char buf1[24];
  char buf2[24];
  char buf3[24];

  ssd1306_Clear();
  snprintf(buf1, sizeof(buf1), "GX:%d", gx);
  snprintf(buf2, sizeof(buf2), "GY:%d", gy);
  snprintf(buf3, sizeof(buf3), "GZ:%d", gz);
  ssd1306_GotoXY(0, 0);
  ssd1306_PutString(mpu6050_ok ? "GYRO [OK]" : "GYRO [FAIL]");
  ssd1306_GotoXY(0, 16);
  ssd1306_PutString(buf1);
  ssd1306_GotoXY(0, 32);
  ssd1306_PutString(buf2);
  ssd1306_GotoXY(0, 48);
  ssd1306_PutString(buf3);
  ssd1306_UpdateScreen();
}

/* 显示步数界面 */
void AppUi_ShowSteps(uint32_t steps)
{
  char buf[24];
  ssd1306_Clear();
  snprintf(buf, sizeof(buf), "STEPS:%lu", (unsigned long)steps);
  ssd1306_GotoXY(0, 0);
  ssd1306_PutString("STEP COUNT");
  ssd1306_GotoXY(0, 24);
  ssd1306_PutString(buf);
  ssd1306_GotoXY(0, 48);
  ssd1306_PutString("PRESS MODE");
  ssd1306_UpdateScreen();
}
