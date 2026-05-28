#ifndef APP_FREERTOS_TASK_H
#define APP_FREERTOS_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"
#include "int_IP5305T.h"
#include "int_motor.h"
#include "int_LED.h"
#include "Com_config.h"
#include "int_SI24R1.h"
#include "APP_receive_Data.h"

#define FreeRTOStest 0 // 1：启用实时操作系统测试任务，0：禁用

extern Aircraft_State aircraft_state; // 声明一个全局变量来存储飞机状态，定义在APP_FreeRTOS_Task.c中
/*
启动FreeRTOS操作系统
*/

void APP_FreeRTOS_Task_Start(void);

#endif // APP_FREERTOS_TASK_H