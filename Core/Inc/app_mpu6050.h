#ifndef __APP_MPU6050_H__
#define __APP_MPU6050_H__

#include "main.h"

#define MPU6050_I2C_ADDR       (0x68 << 1)
#define MPU6050_SMPLRT_DIV     0x19
#define MPU6050_CONFIG         0x1A
#define MPU6050_GYRO_CONFIG    0x1B
#define MPU6050_ACCEL_CONFIG   0x1C
#define MPU6050_ACCEL_XOUT_H   0x3B
#define MPU6050_GYRO_XOUT_H    0x43
#define MPU6050_PWR_MGMT_1     0x6B
#define MPU6050_PWR_MGMT_2     0x6C
#define MPU6050_WHO_AM_I       0x75
#define MPU6050_I2C_TIMEOUT    1000
#define STEP_THRESHOLD         3000

HAL_StatusTypeDef AppMpu6050_Init(void);
HAL_StatusTypeDef AppMpu6050_ReadGyro(int16_t *gx, int16_t *gy, int16_t *gz);
HAL_StatusTypeDef AppMpu6050_ReadAccel(int16_t *ax, int16_t *ay, int16_t *az);
HAL_StatusTypeDef AppMpu6050_WriteReg(uint8_t reg, uint8_t value);
HAL_StatusTypeDef AppMpu6050_ReadReg(uint8_t reg, uint8_t *value);

#endif
