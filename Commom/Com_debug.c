#include "Com_debug.h"
#include <stdio.h>


// 重定向printf函数到串口
int fputc(int ch, FILE *f)
{
    // 将字符发送到串口
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}