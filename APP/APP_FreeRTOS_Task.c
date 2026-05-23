#include "APP_FreeRTOS_Task.h"

void task1(void *pvParameters);
void task2(void *pvParameters);

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);


//任务栈空间大小，最小推荐128，等于128*4=512字节
#define TASK1_STACK_SIZE 128
//任务优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
#define TASK1_PRIORITY 1
//任务句柄，可以用来操作任务，如删除、挂起等
TaskHandle_t task1Handle = NULL;

//任务栈空间大小，最小推荐128，等于128*4=512字节
#define TASK2_STACK_SIZE 128
//任务优先级，数值越大优先级越高，范围从0到configMAX_PRIORITIES-1
#define TASK2_PRIORITY 1
//任务句柄，可以用来操作任务，如删除、挂起等
TaskHandle_t task2Handle = NULL;

/*
启动FreeRTOS操作系统
*/

void APP_FreeRTOS_Task_Start(void)
{
    //1、创建任务
    xTaskCreate(task1, "Task1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, &task1Handle);
    xTaskCreate(task2, "Task2", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, &task2Handle);

    //2、启动调度器
    vTaskStartScheduler();
}

void task1(void *pvParameters)
{
    while (1)
    {
        debug_printf("This is Task 1\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1000ms
    }
}

void task2(void *pvParameters)
{
    while (1)
    {
        debug_printf("This is Task 2\r\n");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时 1000ms
    }
}


/* FreeRTOS 任务栈溢出钩子函数，当任务发生栈溢出时会调用此函数 
需要在 FreeRTOSConfig.h 中将 configCHECK_FOR_STACK_OVERFLOW 设置为 1 或 2 来启用栈溢出检查，并提供此钩子函数的实现。
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}
*/
