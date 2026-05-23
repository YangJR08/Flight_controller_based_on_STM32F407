#ifndef APP_FREERTOS_TASK_H
#define APP_FREERTOS_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"

/*
启动FreeRTOS操作系统
*/

void APP_FreeRTOS_Task_Start(void);

#endif // APP_FREERTOS_TASK_H