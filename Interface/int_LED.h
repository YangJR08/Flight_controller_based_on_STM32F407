#ifndef INT_LED_H
#define INT_LED_H

#include "main.h"

//创建LED控制结构体
typedef struct{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} LED_Struct;

//创建LED编号枚举
typedef enum{
    T_LF_LED = 0,
    D_LF_LED,
    D_RI_LED,
    T_RI_LED,
    LED_NUM
} LED_ID;

//声明LED结构体数组
extern LED_Struct LED_con[LED_NUM];


//LED翻转
void Int_LED_Toggle(LED_Struct *LED);

/*
打开LED灯
*/
void Int_LED_On(LED_Struct *LED);
//关闭LED灯
void Int_LED_Off(LED_Struct *LED);

#endif /* INT_LED_H */