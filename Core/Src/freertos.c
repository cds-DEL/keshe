/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "app_input.h"
#include "app_ui.h"
#include "app_mpu6050.h"
#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint8_t mpu6050_ok;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    static uint32_t tick = 0;
    static uint8_t hours = 0;
    static uint8_t minutes = 0;
    static uint8_t seconds = 0;
    static uint16_t year = 2026;
    static uint8_t month = 7;
    static uint8_t day = 9;
    static uint8_t dateAdjustMode = 0;
    static uint8_t currentPage = 0; /* 0=main menu, 1=clock, 2=gyro, 3=step */
    static uint8_t menuSelection = 0; /* 0=clock, 1=mpu6050, 2=step */
    static int8_t encoderLastState = -1;
    static uint32_t lastEncoderActionTick = 0;
    static GPIO_PinState prev_return_btn = GPIO_PIN_SET;
    static GPIO_PinState prev_encoder_key_btn = GPIO_PIN_SET;
    static GPIO_PinState prev_hour_dec_btn = GPIO_PIN_SET;
    static GPIO_PinState prev_hour_inc_btn = GPIO_PIN_SET;
    static GPIO_PinState prev_min_inc_btn = GPIO_PIN_SET;
    static GPIO_PinState prev_mode_btn = GPIO_PIN_SET;
    static uint8_t oled_on = 1;
    static int16_t gyro_x = 0;
    static int16_t gyro_y = 0;
    static int16_t gyro_z = 0;
    static int16_t accel_x = 0;
    static int16_t accel_y = 0;
    static int16_t accel_z = 0;
    static uint32_t gyroTick = 0;
    static uint32_t stepCount = 0;
    static uint32_t lastSentStepCount = 0;
    static uint8_t gait_state = 0;
    static float peak_candidate = 0.0f;
    static float valley_candidate = 1e6f;
    static float last_filtered = 0.0f;
    static uint32_t last_step_tick = 0;

    uint8_t rxByte = 0;
    if (HAL_UART_Receive(&huart1, &rxByte, 1, 0) == HAL_OK)
    {
      HAL_UART_Transmit(&huart1, &rxByte, 1, 100);
    }

    GPIO_PinState curr_return_btn = AppInput_ReadButtonStable(RETURN_MENU_GPIO_PORT, RETURN_MENU_GPIO_PIN);
    GPIO_PinState curr_encoder_key_btn = AppInput_ReadButtonStable(ENCODER_KEY_GPIO_PORT, ENCODER_KEY_GPIO_PIN);
    GPIO_PinState curr_hour_dec_btn = AppInput_ReadButtonStable(TIME_HOUR_DEC_GPIO_PORT, TIME_HOUR_DEC_GPIO_PIN);
    GPIO_PinState curr_hour_inc_btn = AppInput_ReadButtonStable(TIME_HOUR_INC_GPIO_PORT, TIME_HOUR_INC_GPIO_PIN);
    GPIO_PinState curr_min_inc_btn  = AppInput_ReadButtonStable(TIME_MIN_INC_GPIO_PORT,  TIME_MIN_INC_GPIO_PIN);
    GPIO_PinState curr_mode_btn     = AppInput_ReadButtonStable(MODE_SWITCH_GPIO_PORT,   MODE_SWITCH_GPIO_PIN);
    int8_t encoderDelta = AppInput_ReadEncoderDelta(&encoderLastState);

    if (currentPage == 0)
    {
      const uint32_t encoder_inhibit_ms = 30;
      if (encoderDelta != 0)
      {
        if ((HAL_GetTick() - lastEncoderActionTick) >= encoder_inhibit_ms)
        {
          if (encoderDelta > 0)
          {
            menuSelection = (menuSelection + 1) % 3;
            if (oled_on) AppUi_ShowMainMenu(menuSelection);
          }
          else if (encoderDelta < 0)
          {
            menuSelection = (menuSelection + 2) % 3;
            if (oled_on) AppUi_ShowMainMenu(menuSelection);
          }
          lastEncoderActionTick = HAL_GetTick();
        }
      }
    }

    if (prev_encoder_key_btn == GPIO_PIN_SET && curr_encoder_key_btn == BUTTON_PRESSED_STATE)
    {
      if (currentPage == 0)
      {
        if (menuSelection == 0)
        {
          currentPage = 1;
          if (oled_on) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
        }
        else if (menuSelection == 1)
        {
          currentPage = 2;
          if (oled_on) AppUi_ShowGyro(gyro_x, gyro_y, gyro_z, mpu6050_ok);
        }
        else
        {
          currentPage = 3;
          if (oled_on) AppUi_ShowSteps(stepCount);
        }
      }
      else if (currentPage == 1)
      {
        hours = 0;
        minutes = 0;
        seconds = 0;
        if (oled_on) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
      }
      else if (currentPage == 3)
      {
        stepCount = 0;
        lastSentStepCount = stepCount;
        if (oled_on) AppUi_ShowSteps(stepCount);
      }
    }

    if (prev_return_btn == GPIO_PIN_SET && curr_return_btn == BUTTON_PRESSED_STATE)
    {
      if (currentPage != 0)
      {
        currentPage = 0;
        if (oled_on) AppUi_ShowMainMenu(menuSelection);
      }
    }
    /* PB1: toggle date adjustment mode (only when viewing clock page) */
    if (prev_mode_btn == GPIO_PIN_SET && curr_mode_btn == BUTTON_PRESSED_STATE)
    {
      if (currentPage == 1)
      {
        dateAdjustMode = dateAdjustMode ? 0 : 1;
        if (oled_on) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
      }
    }
    /* PA1: toggle OLED on/off (global, works on any page) */
    if (prev_hour_dec_btn == GPIO_PIN_SET && curr_hour_dec_btn == BUTTON_PRESSED_STATE)
    {
      oled_on = oled_on ? 0 : 1;
      if (!oled_on)
      {
        AppUi_Clear();
      }
      else
      {
        if (currentPage == 0) AppUi_ShowMainMenu(menuSelection);
        else if (currentPage == 1) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
        else if (currentPage == 2) AppUi_ShowGyro(gyro_x, gyro_y, gyro_z, mpu6050_ok);
        else if (currentPage == 3) AppUi_ShowSteps(stepCount);
      }
    }

    /* Time adjustment buttons (only when viewing clock page) */
    if (currentPage == 1)
    {
      if (dateAdjustMode)
      {
        /* PA3: month +1 */
        if (prev_hour_inc_btn == GPIO_PIN_SET && curr_hour_inc_btn == BUTTON_PRESSED_STATE)
        {
          month++;
          if (month > 12)
          {
            month = 1;
          }
          if (day > 28)
          {
            uint8_t maxDay = 31;
            if (month == 2)
            {
              maxDay = 28;
            }
            else if (month == 4 || month == 6 || month == 9 || month == 11)
            {
              maxDay = 30;
            }
            if (day > maxDay)
            {
              day = maxDay;
            }
          }
          if (oled_on) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
        }
        /* PA5: day +1 */
        if (prev_min_inc_btn == GPIO_PIN_SET && curr_min_inc_btn == BUTTON_PRESSED_STATE)
        {
          uint8_t maxDay = 31;
          if (month == 2)
          {
            maxDay = 28;
          }
          else if (month == 4 || month == 6 || month == 9 || month == 11)
          {
            maxDay = 30;
          }
          if (day < maxDay)
          {
            day++;
          }
          else
          {
            day = 1;
            month++;
            if (month > 12)
            {
              month = 1;
            }
          }
          if (oled_on) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
        }
      }
      else
      {
        /* PA3: hour increment */
        if (prev_hour_inc_btn == GPIO_PIN_SET && curr_hour_inc_btn == BUTTON_PRESSED_STATE)
        {
          hours = (hours + 1) % 24;
          if (oled_on) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
        }
        /* PA5: minute increment */
        if (prev_min_inc_btn == GPIO_PIN_SET && curr_min_inc_btn == BUTTON_PRESSED_STATE)
        {
          minutes = (minutes + 1) % 60;
          if (oled_on) AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
        }
      }
    }

    prev_return_btn = curr_return_btn;
    prev_encoder_key_btn = curr_encoder_key_btn;
    prev_hour_dec_btn = curr_hour_dec_btn;
    prev_hour_inc_btn = curr_hour_inc_btn;
    prev_min_inc_btn  = curr_min_inc_btn;
    prev_mode_btn     = curr_mode_btn;

    if (HAL_GetTick() - tick >= 1000)
    {
      tick = HAL_GetTick();
      seconds++;
      if (seconds >= 60)
      {
        seconds = 0;
        minutes++;
      }
      if (minutes >= 60)
      {
        minutes = 0;
        hours++;
      }
      if (hours >= 24)
      {
        hours = 0;
        /* 跨天，日期+1 */
        uint8_t maxDay = 31;
        if (month == 2)
        {
          maxDay = 28;
        }
        else if (month == 4 || month == 6 || month == 9 || month == 11)
        {
          maxDay = 30;
        }
        if (day < maxDay)
        {
          day++;
        }
        else
        {
          day = 1;
          month++;
          if (month > 12)
          {
            month = 1;
          }
        }
      }
      if (currentPage == 1 && oled_on)
      {
        AppUi_ShowTime(hours, minutes, seconds, year, month, day, dateAdjustMode);
      }

      if (stepCount != lastSentStepCount)
      {
        char btBuf[32];
        int btLen = snprintf(btBuf, sizeof(btBuf), "step:%lu\r\n", (unsigned long)stepCount);
        if (btLen > 0)
        {
          HAL_UART_Transmit(&huart1, (uint8_t *)btBuf, (uint16_t)btLen, 100);
        }
        lastSentStepCount = stepCount;
      }
    }

    if (HAL_GetTick() - gyroTick >= 200)
    {
      gyroTick = HAL_GetTick();

      /* Read and wake MPU6050 if it went to sleep (e.g. after power glitch from shaking) */
      uint8_t pwr_val = 0;
      AppMpu6050_ReadReg(MPU6050_PWR_MGMT_1, &pwr_val);
      if (pwr_val != 0x00)
      {
        /* Device is in sleep/standby - wake it up immediately */
        AppMpu6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);
        HAL_Delay(5);
        AppMpu6050_ReadReg(MPU6050_PWR_MGMT_1, &pwr_val);
      }

      /* Also ensure PWR_MGMT_2 has gyro axes enabled (bit0=0), not in standby */
      AppMpu6050_ReadReg(MPU6050_PWR_MGMT_2, &pwr_val);
      if (pwr_val != 0x00)
      {
        AppMpu6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
      }

      HAL_StatusTypeDef gyroStatus = AppMpu6050_ReadGyro(&gyro_x, &gyro_y, &gyro_z);

      if (gyroStatus == HAL_OK)
      {
        mpu6050_ok = 1;
        AppMpu6050_ReadAccel(&accel_x, &accel_y, &accel_z);

        /* Convert raw accel to g (assumes FS = +-2g, 16384 LSB/g) */
        float ax = (float)accel_x / 16384.0f;
        float ay = (float)accel_y / 16384.0f;
        float az = (float)accel_z / 16384.0f;
        float acc_mag = sqrtf(ax * ax + ay * ay + az * az);

        /* 高通滤波：长时间常数低通作为基线，得到高通加速度用于步态检测 */
        static float acc_baseline = 0.0f;
        static uint8_t acc_baseline_init = 0;
        const float baseline_alpha = 0.01f; /* 越小基线越稳定，建议 0.005-0.02 之间 */
        if (!acc_baseline_init)
        {
          acc_baseline = acc_mag;
          acc_baseline_init = 1;
        }
        else
        {
          acc_baseline += baseline_alpha * (acc_mag - acc_baseline);
        }

        float acc_hp = acc_mag - acc_baseline; /* 高通分量，步态信号 */

        /* 使用峰-谷自适应阈值的状态机进行计步，避免阈值随长时间行走漂移 */
        static float peak_ema = 0.0f;
        static uint8_t peak_ema_init = 0;
        const float min_amp = 0.12f; /* 最小峰谷幅度，经验值 */
        const uint32_t min_step_ms = 300; /* 最小步间隔 */
        uint32_t now = HAL_GetTick();

        switch (gait_state)
        {
          case 0: /* WAIT_LOW */
            if (acc_hp < 0.0f)
            {
              if (valley_candidate == 1e6f || acc_hp < valley_candidate) valley_candidate = acc_hp;
            }
            if (acc_hp > 0.0f && acc_hp > last_filtered)
            {
              gait_state = 1; /* RISING */
              peak_candidate = acc_hp;
            }
            break;
          case 1: /* RISING */
            if (acc_hp > peak_candidate) peak_candidate = acc_hp;
            /* detect start of falling when value decreases from peak */
            if (acc_hp < peak_candidate - 0.02f)
            {
              gait_state = 3; /* FALLING */
              valley_candidate = acc_hp;
            }
            break;
          case 3: /* FALLING */
            if (acc_hp < valley_candidate) valley_candidate = acc_hp;
            /* finalize when crossing back to baseline (<=0) */
            if (acc_hp <= 0.0f)
            {
              float peak_val = peak_candidate;
              float valley_val = valley_candidate;
              float amplitude = peak_val - valley_val;

              float amp_thr = min_amp;
              if (peak_ema_init)
              {
                float rel = peak_ema * 0.5f;
                if (rel > amp_thr) amp_thr = rel;
              }

              if (amplitude > amp_thr && (now - last_step_tick) >= min_step_ms)
              {
                stepCount++;
                last_step_tick = now;
                /* 立即发送步数（保持原有行为） */
                char btBuf[32];
                int btLen = snprintf(btBuf, sizeof(btBuf), "step:%lu\r\n", (unsigned long)stepCount);
                if (btLen > 0)
                {
                  if (HAL_UART_Transmit(&huart1, (uint8_t *)btBuf, (uint16_t)btLen, 100) == HAL_OK)
                  {
                    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
                    HAL_Delay(60);
                    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
                  }
                }
                lastSentStepCount = stepCount;

                /* 更新峰值 EMA (用于自适应阈值) */
                if (!peak_ema_init)
                {
                  peak_ema = amplitude;
                  peak_ema_init = 1;
                }
                else
                {
                  peak_ema = peak_ema * 0.9f + amplitude * 0.1f;
                }
              }

              gait_state = 0; /* back to WAIT_LOW */
              valley_candidate = 1e6f;
              peak_candidate = 0.0f;
            }
            break;
        }

        /* 保存上一次高通值用于边缘/趋势检测 */
        last_filtered = acc_hp;
      }
      else
      {
        /* Read failed - clear gyro values to avoid displaying stale data */
        gyro_x = 0;
        gyro_y = 0;
        gyro_z = 0;
        mpu6050_ok = 0;
        /* Try to re-init on failure */
        HAL_I2C_DeInit(&hi2c2);
        MX_I2C2_Init();
        if (AppMpu6050_Init() == HAL_OK)
        {
          mpu6050_ok = 1;
        }
      }

      if (currentPage == 2 && oled_on)
      {
        AppUi_ShowGyro(gyro_x, gyro_y, gyro_z, mpu6050_ok);
      }
      else if (currentPage == 3 && oled_on)
      {
        AppUi_ShowSteps(stepCount);
      }
    }
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

