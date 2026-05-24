#include "int_LED.h"


//LED结构体数组，包含四个LED的GPIO端口和引脚
LED_Struct LED_con[LED_NUM] = {
    {T_LF_LED_GPIO_Port, T_LF_LED_Pin},
    {D_LF_LED_GPIO_Port, D_LF_LED_Pin},
    {D_RI_LED_GPIO_Port, D_RI_LED_Pin},
    {T_RI_LED_GPIO_Port, T_RI_LED_Pin}
};

/*
打开LED灯
*/
void Int_LED_On(LED_Struct *LED)
{
    //直接修改引脚电平为低电平
    HAL_GPIO_WritePin(LED->GPIOx, LED->GPIO_Pin, GPIO_PIN_RESET);
}

//关闭LED灯
void Int_LED_Off(LED_Struct *LED)
{
    //直接修改引脚电平为高电平
    HAL_GPIO_WritePin(LED->GPIOx, LED->GPIO_Pin, GPIO_PIN_SET);
}

//LED翻转
void Int_LED_Toggle(LED_Struct *LED)
{
    //直接翻转引脚电平
    HAL_GPIO_TogglePin(LED->GPIOx, LED->GPIO_Pin);
}