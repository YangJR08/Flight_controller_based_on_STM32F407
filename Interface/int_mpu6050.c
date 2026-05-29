#include "int_mpu6050.h"
#include "Com_config.h"

//定义偏移量结构体，原来结构体16位可能不够用，改成32位
typedef struct{
    int32_t accel_x_offset; // 加速度X轴偏移量
    int32_t accel_y_offset; // 加速度Y轴偏移量
    int32_t accel_z_offset; // 加速度Z轴偏移量
    int32_t gyro_x_offset;  // 角速度X轴偏移量
    int32_t gyro_y_offset;  // 角速度Y轴偏移量
    int32_t gyro_z_offset;  // 角速度Z轴偏移量
} MPU6050_Offset_struct;

MPU6050_Offset_struct mpu6050_offset; // 定义一个全局变量来存储MPU6050的偏移量

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

//零偏校准函数，读取陀螺仪和加速度计的零偏值，并存储在全局变量中，后续读取数据时会自动减去零偏值，得到更准确的测量值
//初始化MPU6050之后调用
void Int_MPU6050_Calibrate(void)
{   //等待飞机停放平稳，前后两次加速度的值小于两百，连续100次，才认为飞机已经停放平稳了，可以进行零偏校准了
    Accel_struct current_accel = {0};
    Accel_struct last_accel = {0};
    uint8_t stable_count = 0; // 稳定计数器
    Int_MPU6050_Get_Accel(&last_accel);
    while (stable_count < 100) 
    {
        Int_MPU6050_Get_Accel(&current_accel);
        if (abs(current_accel.accel_x - last_accel.accel_x) < 200 &&
            abs(current_accel.accel_y - last_accel.accel_y) < 200 &&
            abs(current_accel.accel_z - last_accel.accel_z) < 200) 
        {
            stable_count++;
        } 
        else 
        {
            stable_count = 0; // 如果有一次不稳定，就重置计数器
        }
        last_accel = current_accel; // 更新上一次的加速度值
        HAL_Delay(6); // 每10ms读取一次数据
    }
    //这里我们可以读取陀螺仪和加速度计的零偏值，并存储在全局变量中，后续读取数据时会自动减去零偏值，得到更准确的测量值
    //具体实现可以根据需要进行调整，比如读取多次取平均值，或者使用滤波算法来计算零偏值，这样可以提高零偏校准的准确性
    Gyro_Accel_struct zero_bias = {0};
    for (int i = 0; i < 100; i++)
    {
        Int_MPU6050_Get_Data(&zero_bias);
        mpu6050_offset.accel_x_offset += zero_bias.accel.accel_x;
        mpu6050_offset.accel_y_offset += zero_bias.accel.accel_y;
        mpu6050_offset.accel_z_offset += (zero_bias.accel.accel_z-16384);
        mpu6050_offset.gyro_x_offset += zero_bias.gyro.gyro_x;
        mpu6050_offset.gyro_y_offset += zero_bias.gyro.gyro_y;
        mpu6050_offset.gyro_z_offset += zero_bias.gyro.gyro_z;
        HAL_Delay(6); // 每10ms读取一次数据
    }
    // 计算平均零偏值
    mpu6050_offset.accel_x_offset /= 100;
    mpu6050_offset.accel_y_offset /= 100;
    mpu6050_offset.accel_z_offset /= 100;
    mpu6050_offset.gyro_x_offset /= 100;
    mpu6050_offset.gyro_y_offset /= 100;
    mpu6050_offset.gyro_z_offset /= 100;
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
    //配置低通滤波器，加速度配置184陀螺仪配置188，DLPF_CFG值为1
    Int_MPU6050_Write_Reg(MPU6050_CONFIG, 0x01); // 低通滤波器配置，设置为184Hz
    //配置系统时钟源
    //在电源管理寄存器PWR_MGMT_1中，系统时钟源由第2-0位控制
    //默认值是0x00，表示使用内部8MHz振荡器作为时钟源，这个时钟源的稳定性和精度较差
    //可能会导致陀螺仪和加速度计的数据不稳定，所以我们可以选择一个更稳定的时钟源
    //比如陀螺仪X轴的输出作为时钟源，对应的值是0x01，所以我们可以写1（0b001）来选择陀螺仪X轴的输出作为时钟源
    //注意这里之前对这个寄存器是写零唤醒的，如果之前对高位有配置的话，这里需要把高位的配置保留，所以我们需要先读取这个寄存器的值
    // 然后把高位的配置保留，最后再写入新的值
    uint8_t pwr_mgmt_1_value = Int_MPU6050_Read_Reg(PWR_MGMT_1); // 读取当前寄存器值
    pwr_mgmt_1_value &= 0xF8; // 保留高位的配置，清除低3位
    pwr_mgmt_1_value |= 0x01; // 设置低3位为0x01，选择陀螺仪X轴的输出作为时钟源
    Int_MPU6050_Write_Reg(PWR_MGMT_1, pwr_mgmt_1_value); // 写入新的寄存器值
    //Int_MPU6050_Write_Reg(PWR_MGMT_1, 0x01); // 选择陀螺仪X轴的输出作为时钟源
    //去电源管理寄存器2，使能加速度和角速度传感器
    //其实默认0可以不写
    Int_MPU6050_Write_Reg(PWR_MGMT_2, 0x00); // 0x6C是电源管理寄存器2，设置为0，表示所有传感器都使能

    //进行测量值零偏校准，读取陀螺仪和加速度计的初始值，作为零偏值，在后续的测量中减去这个零偏值，可以提高测量的准确性
    Int_MPU6050_Calibrate();

}

//读取三轴角速度
//抖动得做滤波和初始值不为0要做零偏校准
void Int_MPU6050_Get_Gyro(Gyro_struct *gyro)
{
    gyro->gyro_x = (Int_MPU6050_Read_Reg(GYRO_XOUT_H) << 8) | Int_MPU6050_Read_Reg(GYRO_XOUT_L);
    gyro->gyro_x /= 16.4; // 16.4是±2000°/s量程的敏感度，单位是LSB/(°/s)，所以我们需要把读取到的值除以16.4来得到实际的角速度值，单位是°/s
    gyro->gyro_y = (Int_MPU6050_Read_Reg(GYRO_YOUT_H) << 8) | Int_MPU6050_Read_Reg(GYRO_YOUT_L);
    gyro->gyro_y /= 16.4; // 16.4是±2000°/s量程的敏感度，单位是LSB/(°/s)，所以我们需要把读取到的值除以16.4来得到实际的角速度值，单位是°/s
    gyro->gyro_z = (Int_MPU6050_Read_Reg(GYRO_ZOUT_H) << 8) | Int_MPU6050_Read_Reg(GYRO_ZOUT_L);
    gyro->gyro_z /= 16.4; // 16.4是±2000°/s量程的敏感度，单位是LSB/(°/s)，所以我们需要把读取到的值除以16.4来得到实际的角速度值，单位是°/s
}
//读取三轴加速度
//抖动得做滤波和初始值不为0要做零偏校准
//z轴值不为0
void Int_MPU6050_Get_Accel(Accel_struct *accel)
{
    accel->accel_x = (Int_MPU6050_Read_Reg(ACCEL_XOUT_H) << 8) | Int_MPU6050_Read_Reg(ACCEL_XOUT_L);
    accel->accel_y = (Int_MPU6050_Read_Reg(ACCEL_YOUT_H) << 8) | Int_MPU6050_Read_Reg(ACCEL_YOUT_L);
    accel->accel_z = (Int_MPU6050_Read_Reg(ACCEL_ZOUT_H) << 8) | Int_MPU6050_Read_Reg(ACCEL_ZOUT_L);
    accel->accel_x /= 16384.0; // 16384.0是±2g量程的敏感度，单位是LSB/g，所以我们需要把读取到的值除以16384.0来得到实际的加速度值，单位是g
    accel->accel_y /= 16384.0; // 16384.0是±2g量程的敏感度，单位是LSB/g，所以我们需要把读取到的值除以16384.0来得到实际的加速度值，单位是g
    accel->accel_z /= 16384.0; // 16384.0是±2g量程的敏感度，单位是LSB/g，所以我们需要把读取到的值除以16384.0来得到实际的加速度值，单位是g
}


//读取三轴角速度和三轴加速度
void Int_MPU6050_Get_Data(Gyro_Accel_struct *data)
{
    Int_MPU6050_Get_Gyro(&data->gyro);
    Int_MPU6050_Get_Accel(&data->accel);
}