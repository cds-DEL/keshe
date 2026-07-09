#include "app_mpu6050.h"
#include "i2c.h"

HAL_StatusTypeDef AppMpu6050_WriteReg(uint8_t reg, uint8_t value)
{
  return HAL_I2C_Mem_Write(&hi2c2, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, MPU6050_I2C_TIMEOUT);
}

HAL_StatusTypeDef AppMpu6050_ReadReg(uint8_t reg, uint8_t *value)
{
  return HAL_I2C_Mem_Read(&hi2c2, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, MPU6050_I2C_TIMEOUT);
}

HAL_StatusTypeDef AppMpu6050_Init(void)
{
  uint8_t val;

  HAL_Delay(50);
  if (HAL_I2C_IsDeviceReady(&hi2c2, MPU6050_I2C_ADDR, 5, MPU6050_I2C_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (AppMpu6050_WriteReg(MPU6050_PWR_MGMT_1, 0x80) != HAL_OK) return HAL_ERROR;
  HAL_Delay(100);

  if (AppMpu6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00) != HAL_OK) return HAL_ERROR;
  HAL_Delay(10);

  if (AppMpu6050_ReadReg(MPU6050_PWR_MGMT_1, &val) != HAL_OK) return HAL_ERROR;
  if (val != 0x00)
  {
    if (AppMpu6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00) != HAL_OK) return HAL_ERROR;
    HAL_Delay(10);
    if (AppMpu6050_ReadReg(MPU6050_PWR_MGMT_1, &val) != HAL_OK) return HAL_ERROR;
    if (val != 0x00) return HAL_ERROR;
  }

  if (AppMpu6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00) != HAL_OK) return HAL_ERROR;
  if (AppMpu6050_WriteReg(0x38, 0x00) != HAL_OK) return HAL_ERROR;
  if (AppMpu6050_WriteReg(0x1F, 0x00) != HAL_OK) return HAL_ERROR;
  if (AppMpu6050_WriteReg(0x20, 0x00) != HAL_OK) return HAL_ERROR;
  if (AppMpu6050_WriteReg(0x6A, 0x00) != HAL_OK) return HAL_ERROR;
  if (AppMpu6050_WriteReg(MPU6050_GYRO_CONFIG, 0x00) != HAL_OK) return HAL_ERROR;
  if (AppMpu6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00) != HAL_OK) return HAL_ERROR;

  HAL_Delay(50);
  return HAL_OK;
}

HAL_StatusTypeDef AppMpu6050_ReadGyro(int16_t *gx, int16_t *gy, int16_t *gz)
{
  uint8_t data[6];
  if (HAL_I2C_Mem_Read(&hi2c2, MPU6050_I2C_ADDR, MPU6050_GYRO_XOUT_H, I2C_MEMADD_SIZE_8BIT, data, 6, MPU6050_I2C_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }

  *gx = (int16_t)((data[0] << 8) | data[1]);
  *gy = (int16_t)((data[2] << 8) | data[3]);
  *gz = (int16_t)((data[4] << 8) | data[5]);
  return HAL_OK;
}

HAL_StatusTypeDef AppMpu6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az)
{
  uint8_t data[6];
  if (HAL_I2C_Mem_Read(&hi2c2, MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, data, 6, MPU6050_I2C_TIMEOUT) != HAL_OK)
  {
    return HAL_ERROR;
  }

  *ax = (int16_t)((data[0] << 8) | data[1]);
  *ay = (int16_t)((data[2] << 8) | data[3]);
  *az = (int16_t)((data[4] << 8) | data[5]);
  return HAL_OK;
}
