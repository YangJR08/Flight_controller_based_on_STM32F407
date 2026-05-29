#ifndef APP_FLIGHT_H
#define APP_FLIGHT_H

#include "int_mpu6050.h"
#include "Com_config.h"
#include "Com_debug.h"
#include "Com_filter.h"
#include "math.h"
#include "Com_imu.h"
#include "Com_pid.h"
#include "APP_FreeRTOS_Task.h"

//飞控任务初始化，包括mpu6050和电机
void APP_Flight_Init(void);

//根据陀螺仪计算出欧拉角，单位是度
void APP_Flight_Get_euler_angel(void);

//飞行控制pid处理函数
void APP_flight_pid_process(void);

//根据PID的输出来控制电机的速度
void APP_flight_control_motor(void);

#endif // APP_FLIGHT_H