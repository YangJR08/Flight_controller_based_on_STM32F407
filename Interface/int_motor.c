#include "int_motor.h"

//电机结构体数组，包含四个电机的定时器句柄、通道和占空比
Motor_Struct motor_con[MOTOR_NUM] ={
    {&htim1, TIM_CHANNEL_4, 200}, //左上电机
    {&htim1, TIM_CHANNEL_2, 200}, //左下电机
    {&htim1, TIM_CHANNEL_1, 200}, //右上电机
    {&htim1, TIM_CHANNEL_3, 200}  //右下电机
};


//电机启动函数
void Int_motor_start(Motor_Struct *motor)
{
    if(motor == NULL)
    {
        debug_printf("无电机结构体参数");
        return;
    }
    HAL_TIM_PWM_Start(motor->htim, motor->channel);
}

void Int_motor_stop(Motor_Struct *motor)
{
    if(motor == NULL)
    {
        debug_printf("无电机结构体参数");
        return;
    }
    for(int i=0;i<MOTOR_NUM;i++)
    {
        HAL_TIM_PWM_Stop(motor_con[i].htim, motor_con[i].channel);
    }
}


//控制电机转速函数
void Int_motor_set_speed(Motor_Struct *motor)
{
    if(motor == NULL)
    {
        debug_printf("无电机结构体参数");
        return;
    }
    if(motor->duty_cycle > 600)
    {
        motor->duty_cycle = 600; // 限制占空比最大值为60%
    }
    //参数定时器句柄，通道，占空比
    __HAL_TIM_SetCompare(motor->htim,motor->channel,motor->duty_cycle);
    debug_printf("电机转速已设置");
}