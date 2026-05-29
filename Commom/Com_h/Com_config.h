#ifndef COM_CONFIG_H
#define COM_CONFIG_H

#include "main.h"
#include <stdint.h>
// 系统配置参数


//表示当前连接状态
typedef enum{
    REMOTE_CONNECTED = 0, // 遥控器已连接
    REMOTE_DISCONNECTED,  // 遥控器已断开
} Remote_State;

//飞行状态
typedef enum{
    FLIGHT_IDLE = 0, // 飞行器空闲
    FLIGHT_NORMAL,      // 正常飞行
    FLIGHT_HEIGHT,     // 定高中飞行
    FLIGHT_FALLING,    // 故障
} Flight_State;

//油门解锁状态
typedef enum{
    FREE = 0, 
    MAX,
    LEAV_MAX,
    MIN,
    UNLOCK,
} Throttle_State;


//封装飞机状态
typedef struct{
    Remote_State remote_state; // 当前连接状态
    Flight_State flight_state; // 当前飞行状态
    Throttle_State throttle_state; // 当前油门解锁状态
} Aircraft_State;

//陀螺仪数据,实测得到方向
typedef struct{
    int16_t accel_x; // 加速度X轴数据，往前飞行时为正，往后飞行时为负
    int16_t accel_y; // 加速度Y轴数据，往左飞行时为正，往右飞行时为负
    int16_t accel_z; //朝上为正，朝下为负
} Accel_struct;

typedef struct{
    int16_t gyro_x; // 角速度X轴数据，往右飞行时为正，往左飞行时为负
    int16_t gyro_y; // 角速度Y轴数据，往前飞行时为正，往后飞行时为负
    int16_t gyro_z; // 角速度Z轴数据，顺时针为正，逆时针为负
} Gyro_struct;

typedef struct{
    Accel_struct accel; // 加速度数据
    Gyro_struct gyro;   // 角速度数据
} Gyro_Accel_struct;

//解算得到的欧拉角
typedef struct{
    int16_t yaw;   // 偏航角
    int16_t pitch; // 俯仰角
    int16_t roll;  // 横滚角
} Euler_Angle_struct;

#endif // COM_CONFIG_H