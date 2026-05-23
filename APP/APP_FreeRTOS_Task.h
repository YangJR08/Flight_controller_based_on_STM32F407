#ifndef APP_FREERTOS_TASK_H
#define APP_FREERTOS_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"
#include "int_IP5305T.h"
#include "int_motor.h"

#define FreeRTOStest 0 // 1：启用实时操作系统测试任务，0：禁用


/*
启动FreeRTOS操作系统
*/

void APP_FreeRTOS_Task_Start(void);

#endif // APP_FREERTOS_TASK_H