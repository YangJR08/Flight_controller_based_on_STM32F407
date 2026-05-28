#include "int_mpu6050.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>

//封装一下iic通讯
//写寄存器函数，参数为reg寄存器地址和value要写入的值
void Int_MPU6050_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    //参数介绍：hi2c1：I2C句柄，MPU6050_WRITE_ADDR：设备地址，data：要发送的数据，2：数据长度，HAL_MAX_DELAY：超时时间
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_WRITE_ADDR, data, 2, HAL_MAX_DELAY);
}

//读寄存器函数，参数为reg寄存器地址，返回值为寄存器的值
uint8_t Int_MPU6050_Read_Reg(uint8_t reg)
{
    uint8_t value;
    //参数介绍：hi2c1：I2C句柄，MPU6050_WRITE_ADDR：设备地址，&reg：要发送的数据地址，1：数据长度，HAL_MAX_DELAY：超时时间
    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_WRITE_ADDR, &reg, 1, HAL_MAX_DELAY);
    //参数介绍：hi2c1：I2C句柄，MPU6050_READ_ADDR：设备地址，&value：接收数据的地址，1：数据长度，HAL_MAX_DELAY：超时时间
    HAL_I2C_Master_Receive(&hi2c1, MPU6050_READ_ADDR, &value, 1, HAL_MAX_DELAY);
    return value;
}

//读寄存器函数，参数为reg寄存器地址和data要接收数据的变量地址
void Int_MPU6050_Reg_Read(uint8_t reg, uint8_t *data)
{
    //参数介绍：hi2c1：I2C句柄，MPU6050_READ_ADDR：设备地址，reg：要读取的数据地址，1：数据长度，data：接收数据的地址，1：数据长度，HAL_MAX_DELAY：超时时间
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_READ_ADDR, reg, 1, data, 1, HAL_MAX_DELAY);
}


//也就是对寄存器进行详细得配置，见寄存器及其描述
void Int_MPU6050_Init(void)
{
    //重启芯片配置相关寄存器
    //写电源管理芯片DEVICE_RESET为1，重启芯片，第7位写1，其他位写0
    Int_MPU6050_Write_Reg(PWR_MGMT_1, 0x80);
    //手册说重启后电源管理寄存器默认值是0x40，所以我们读取这个寄存器，表示当前为低功耗模式
    //写个while循环来一直读取
    uint8_t timeout = 0; // 超时计数器
    while (Int_MPU6050_Read_Reg(PWR_MGMT_1) != 0x40) 
    {
        timeout++;
        //等待芯片重启完成，直到寄存器值变为0x40
        //避免卡死，可以添加一个超时机制，比如等待100ms后如果还没有重启完成就返回错误
        //这个函数调用的时候freertos还没有启动，所以不能使用vTaskDelay，可以使用HAL_Delay函数进行延时
        HAL_Delay(10); // 等待10ms后重试
        if (timeout > 10) // 超过100ms还没有重启完成
        {
            debug_printf("MPU6050 reset timeout!"); // 打印错误信息
            // 可以在这里添加错误处理代码，比如返回错误码或者打印错误信息
            return; // 直接返回，表示初始化失败
        }
    }
    //重启之后默认是低功耗模式，所以我们需要写0x00来退出低功耗模式
    Int_MPU6050_Write_Reg(PWR_MGMT_1, 0x00);
    //设置陀螺仪和加速度计的量程，采样率等参数，这些参数可以根据需要进行调整
    //量程范围不是越大与大越好，因为adc的分辨率是固定的，量程范围越大，分辨率就越低，所以需要根据实际需求来设置量程范围
    //所以量程在够用的范围内越小越好，这样可以提高分辨率，减少噪声的影响
    //填写角速度量程寄存器GYRO_CONFIG，设置为±2000°/s，对应的值是0x18，因为我们要设置的位是第4和第3位，所以我们可以写3（0b11）左移3位，得到0x18
    Int_MPU6050_Write_Reg(GYRO_CONFIG, 3 << 3); // 0x18 = 3 << 3
    //填写加速度量程寄存器ACCEL_CONFIG，设置为±2g，对应的值是0x00，因为我们要设置的位是第4和第3位，所以我们可以写0（0b00）左移3位，得到0x00
    Int_MPU6050_Write_Reg(ACCEL_CONFIG, 0 << 3); // 0x00 = 0 << 3
    //没有用到中断关闭中断使能
    Int_MPU6050_Write_Reg(INT_ENABLE, 0x00);
    //用户控制寄存器没有用到，不使用fifo和icc拓展
    Int_MPU6050_Write_Reg(USER_CTRL, 0x00);
    //采样频率分频器配置
    //陀螺仪监控三轴加速度和三轴角速度，默认频率1khz，1ms读取一次数据，如果不用这么快的频率，可以通过采样频率分频器来降低采样频率，分频器的值是0-255，采样频率=1khz/(分频器+1)，比如分频器设置为9，采样频率就是100hz
    //不能把频率设置的太低，因为陀螺仪和加速度计的数据会有一定的延迟，如果频率设置的太低，可能会错过一些重要的数据，导致数据不准确，所以需要根据实际需求来设置采样频率
    //飞控频率大约166hz，所以频率不能小于飞控频率，不然会数据失真
    //建议大于分控频率的倍，采样率大于使用频率的2倍，所以我们直接设置为500hz
    Int_MPU6050_Write_Reg(SMPLRT_DIV, 1); // 0x19是采样频率分频器寄存器，设置为1，采样频率就是500hz
}