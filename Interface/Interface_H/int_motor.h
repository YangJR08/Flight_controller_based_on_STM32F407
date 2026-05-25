#ifndef __INT_MOTOR__
#define __INT_MOTOR__

#include "tim.h"
#include "Com_debug.h"



//创建电机控制结构体
typedef struct
{
    TIM_HandleTypeDef *htim; // 定时器句柄
    uint16_t channel;        // 定时器通道
    uint16_t duty_cycle;       // 占空比，范围0-1000
} Motor_Struct;


//电机编号
typedef enum
{
    MOTOR_LEFT_UP = 0,
    MOTOR_LEFT_DOWN,
    MOTOR_RIGHT_UP,
    MOTOR_RIGHT_DOWN,
    MOTOR_NUM // 电机数量
} Motor_ID;

extern Motor_Struct motor_con[MOTOR_NUM]; // 电机结构体数组，包含四个电机的定时器句柄、通道和占空比
//电机启动函数
void Int_motor_start(Motor_Struct *motor);

//控制电机转速函数
void Int_motor_set_speed(Motor_Struct *motor);


#endif