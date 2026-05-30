#ifndef COM_PID_H
#define COM_PID_H

//定义积分时间由任务时间决定
#include <stdint.h>
#include "main.h"
#define PID_TASK_TIME 0.006 // 6ms

// PID结构体
// kp: 比例增益，ki: 积分增益，kd: 微分增益,需要在使用前进行初始化
//目标值和测量值需要在每次计算前更新
typedef struct {
    float kp;   //值越大，系统响应越快，但可能会引起过度振荡
    float ki;   //解决系统的稳态误差，但过大的积分增益可能会导致系统不稳定
    float kd;   //值越大，系统对误差变化的响应越快，解决过调问题，但过大的微分增益可能会引起系统噪声放大
    float error;    //当前误差
    float desire;  //期望值
    float measure;  //测量值
    float last_error; //上一次的误差
    float integral; //积分累计
    float output;   //PID输出
} PID_TypeDef;

//单次PID计算
void Com_PID_Calculate(PID_TypeDef *pid);

//串级PID计算
void Com_PID_Calculate_Chain(PID_TypeDef *pid_outer, PID_TypeDef *pid_inner);

//限制电机速度在正常范围内
int16_t Com_Limit(int16_t speed, int16_t min, int16_t max);

#endif // COM_PID_H