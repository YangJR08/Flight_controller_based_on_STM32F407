#ifndef INT_MPU6050_H
#define INT_MPU6050_H

#include "i2c.h"
#include "stm32f4xx_hal_i2c.h"
#include "main.h"
#include "com_debug.h"
#include "com_config.h"
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "Com_delay.h"

// MPU6050 I2C地址
#define MPU6050_ADDR 0x68  
//读写地址
#define MPU6050_WRITE_ADDR (MPU6050_ADDR << 1) // 写地址，最低位为0
#define MPU6050_READ_ADDR (MPU6050_ADDR << 1 | 0x01) // 读地址，最低位为1

// MPU6050寄存器地址
#define PWR_MGMT_1 0x6B
//电源管理寄存器地址2
#define PWR_MGMT_2 0x6C
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
//低通滤波器配置寄存器地址
#define MPU6050_CONFIG 0x1A

//存储角速度的寄存器地址
//数据高八位在前，低八位在后，所以要先读取高八位，再读取低八位，最后把高八位和低八位合成一个16位的整数
//XYZ轴的寄存器地址是连续的，所以我们可以通过读取连续的寄存器来获取三轴的数据，这样可以减少I2C通信的次数，提高效率
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

//存储加速度的寄存器地址
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40


// Function declarations for MPU6050 interface
// 初始化MPU6050传感器
//也就是对寄存器进行详细得配置，见寄存器及其描述
void Int_MPU6050_Init(void);
//读取三轴角速度
void Int_MPU6050_Get_Gyro(Gyro_struct *gyro);
//读取三轴加速度
void Int_MPU6050_Get_Accel(Accel_struct *accel);
//读取三轴角速度和三轴加速度
void Int_MPU6050_Get_Data(Gyro_Accel_struct *data);


void Int_MPU6050_Read_Data(float *accel, float *gyro);
void Int_MPU6050_Update(void);

#endif // INT_MPU6050_H