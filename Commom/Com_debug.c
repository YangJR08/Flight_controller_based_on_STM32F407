#include "Com_debug.h"
#include <stdio.h>


static UART_HandleTypeDef *s_debug_uart = &huart1;

void Debug_SetUart(UART_HandleTypeDef *huart)
{
    if (huart != NULL)
    {
        s_debug_uart = huart;
    }
}

// 将 newlib 的底层输出重定向到串口（printf 通过 _write -> __io_putchar 走这里）
int __io_putchar(int ch)
{
    uint8_t c = (uint8_t)ch;
    UART_HandleTypeDef *uart = s_debug_uart;

    if (uart == NULL)
    {
        return ch;
    }

    // 兼容终端换行显示
    if (c == '\n')
    {
        uint8_t cr = '\r';
        HAL_UART_Transmit(uart, &cr, 1, 1000);
    }

    HAL_UART_Transmit(uart, &c, 1, 1000);
    return ch;
}

// 兼容直接调用 fputc 的代码
int fputc(int ch, FILE *f)
{
    (void)f;
    return __io_putchar(ch);
}