#include "APP_mutex.h"

//互斥锁句柄，用于保护 remote_data
static SemaphoreHandle_t remote_data_mutex = NULL;

void APP_Mutex_Init(void)
{
    //只创建一次，避免重复创建互斥锁
    if (remote_data_mutex == NULL)
    {
        remote_data_mutex = xSemaphoreCreateMutex();
    }
}

BaseType_t APP_RemoteData_Lock(TickType_t timeout_ticks)
{
    //首次使用时确保互斥锁已创建
    if (remote_data_mutex == NULL)
    {
        APP_Mutex_Init();
    }
    if (remote_data_mutex == NULL)
    {
        //创建失败，无法加锁
        return pdFALSE;
    }

    //获取互斥锁，超时由调用者指定
    return xSemaphoreTake(remote_data_mutex, timeout_ticks);
}

void APP_RemoteData_Unlock(void)
{
    //释放互斥锁
    if (remote_data_mutex != NULL)
    {
        xSemaphoreGive(remote_data_mutex);
    }
}
