#ifndef INT_MPU6050_H
#define INT_MPU6050_H

#include "i2c.h"
#include "stm32f4xx_hal_i2c.h"
#include "main.h"
#include "com_debug.h"

// MPU6050 I2C地址
#define MPU6050_ADDR 0x68  
//读写地址
#define MPU6050_WRITE_ADDR (MPU6050_ADDR << 1) // 写地址，最低位为0
#define MPU6050_READ_ADDR (MPU6050_ADDR << 1 | 0x01) // 读地址，最低位为1

// MPU6050寄存器地址
#define PWR_MGMT_1 0x6B
//角速度量程寄存器
#define GYRO_CONFIG 0x1B
//加速度量程寄存器 
#define ACCEL_CONFIG 0x1C
//陀螺仪数据寄存器地址 
#define GYRO_XOUT_H 0x43
//中断状态寄存器地址
#define INT_STATUS 0x3A
//中断使能寄存器地址
#define INT_ENABLE 0x38
//用户控制寄存器地址
#define USER_CTRL 0x6A
//采样频率分频器寄存器地址
#define SMPLRT_DIV 0x19


// Function declarations for MPU6050 interface
// 初始化MPU6050传感器
//也就是对寄存器进行详细得配置，见寄存器及其描述
void Int_MPU6050_Init(void);
void Int_MPU6050_Read_Data(float *accel, float *gyro);
void Int_MPU6050_Update(void);

#endif // INT_MPU6050_H