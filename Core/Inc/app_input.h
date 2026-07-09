#ifndef __APP_INPUT_H__
#define __APP_INPUT_H__

#include "main.h"

/* 旋钮和按键对应的引脚定义 */
#define ENCODER_A_GPIO_PORT GPIOB
#define ENCODER_A_GPIO_PIN  GPIO_PIN_12
#define ENCODER_B_GPIO_PORT GPIOB
#define ENCODER_B_GPIO_PIN  GPIO_PIN_13
#define ENCODER_KEY_GPIO_PORT GPIOB
#define ENCODER_KEY_GPIO_PIN  GPIO_PIN_14
#define RETURN_MENU_GPIO_PORT GPIOA
#define RETURN_MENU_GPIO_PIN  GPIO_PIN_7
#define BUTTON_PRESSED_STATE GPIO_PIN_RESET

/* 时间调整按钮 */
#define TIME_HOUR_DEC_GPIO_PORT GPIOA
#define TIME_HOUR_DEC_GPIO_PIN  GPIO_PIN_1
#define TIME_HOUR_INC_GPIO_PORT GPIOA
#define TIME_HOUR_INC_GPIO_PIN  GPIO_PIN_3
#define TIME_MIN_INC_GPIO_PORT  GPIOA
#define TIME_MIN_INC_GPIO_PIN   GPIO_PIN_5

/* 模式切换按钮 (PB1) */
#define MODE_SWITCH_GPIO_PORT   GPIOB
#define MODE_SWITCH_GPIO_PIN    GPIO_PIN_1

/* 按键消抖读取 */
GPIO_PinState AppInput_ReadButtonStable(GPIO_TypeDef *port, uint16_t pin);

/* 编码器状态机读取，返回增量 */
int8_t AppInput_ReadEncoderDelta(int8_t *lastState);

#endif
