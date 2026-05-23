#ifndef __COM_DEBUG__
#define __COM_DEBUG__

#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

#define DEBUG_LOG_ENABLE 1 // 1: enable debug logs, 0: disable

// Compile-time default UART for debug output.
// Change this macro if you want to switch the default without calling Debug_SetUart().
#define DEBUG_DEFAULT_UART huart1

#ifdef DEBUG_LOG_ENABLE 
//使用宏定义实现带文件名和行号的调试打印
#define debug_printf(format, ...) printf("[%s:%d]" format, __FILE__, __LINE__, ##__VA_ARGS__)
#else//如果没有开启调试日志，则定义一个空的宏
#define debug_printf(format, ...)
#endif

// 运行时选择调试串口
void Debug_SetUart(UART_HandleTypeDef *huart);

#endif /* __COM_DEBUG__ */