#ifndef APP_MUTEX_H
#define APP_MUTEX_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//初始化互斥锁（在任务启动前调用一次）
void APP_Mutex_Init(void);
//加锁保护 remote_data，timeout_ticks 为等待时间（单位：tick）
BaseType_t APP_RemoteData_Lock(TickType_t timeout_ticks);
//释放 remote_data 的互斥锁
void APP_RemoteData_Unlock(void);

#endif // APP_MUTEX_H
