#include "Com_pid.h"


//单次PID计算
void Com_PID_Calculate(PID_TypeDef *pid)
{
    //计算当前误差
    pid->error = pid->measure - pid->desire;
    //计算积分累计
    pid->integral += pid->error; // 积分项乘以积分时间，防止积分过大
    if (pid->last_error == 0) // 第一次计算，微分项为0
    {
        pid->last_error = pid->error;
    }
    //计算微分项
    float derivative = pid->error - pid->last_error;
    //计算PID输出
    pid->output = pid->kp * pid->error + pid->ki * pid->integral * PID_TASK_TIME + (pid->kd * derivative)/ PID_TASK_TIME; // 微分项除以积分时间，防止微分过大
    //更新上一次的误差
    pid->last_error = pid->error;
}

//串级PID计算
void Com_PID_Calculate_Chain(PID_TypeDef *pid_outer, PID_TypeDef *pid_inner)
{
    // 计算外环PID
    Com_PID_Calculate(pid_outer);
    
    // 将外环输出作为内环的期望值
    pid_inner->desire = pid_outer->output;
    
    // 计算内环PID
    Com_PID_Calculate(pid_inner);
}