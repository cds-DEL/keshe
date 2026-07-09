#include "app_input.h"

/* 读取按钮并做简要消抖 */
GPIO_PinState AppInput_ReadButtonStable(GPIO_TypeDef *port, uint16_t pin)
{
  GPIO_PinState state = HAL_GPIO_ReadPin(port, pin);
  if (state == BUTTON_PRESSED_STATE)
  {
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(port, pin) == BUTTON_PRESSED_STATE)
    {
      return BUTTON_PRESSED_STATE;
    }
  }
  return GPIO_PIN_SET;
}

/* 读取编码器增量，使用简单的四相状态机 */
int8_t AppInput_ReadEncoderDelta(int8_t *lastState)
{
  uint8_t a = (HAL_GPIO_ReadPin(ENCODER_A_GPIO_PORT, ENCODER_A_GPIO_PIN) == GPIO_PIN_RESET) ? 1U : 0U;
  uint8_t b = (HAL_GPIO_ReadPin(ENCODER_B_GPIO_PORT, ENCODER_B_GPIO_PIN) == GPIO_PIN_RESET) ? 1U : 0U;
  uint8_t raw = (uint8_t)((a << 1) | b);

  static uint8_t lastRaw = 0xFF;
  static uint32_t lastChangeTick = 0;
  const uint32_t debounce_ms = 5;

  if (lastRaw != raw)
  {
    lastRaw = raw;
    lastChangeTick = HAL_GetTick();
    return 0;
  }

  if ((HAL_GetTick() - lastChangeTick) < debounce_ms)
  {
    return 0;
  }

  uint8_t state = raw;
  if (*lastState < 0)
  {
    *lastState = (int8_t)state;
    return 0;
  }

  static const int8_t encoderTable[16] = {
      0,  1, -1,  0,
     -1,  0,  0,  1,
      1,  0,  0, -1,
      0, -1,  1,  0
  };

  int8_t delta = encoderTable[((*lastState) << 2) | state];
  *lastState = (int8_t)state;
  return delta;
}
