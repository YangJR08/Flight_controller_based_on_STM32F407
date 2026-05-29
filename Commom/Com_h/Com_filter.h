#ifndef COM_FILTER_H
#define COM_FILTER_H
#include "Com_debug.h"

//卡尔曼滤波器结构体定义
typedef struct {
    float LastP; // 上一次的误差协方差
    float Now_P; // 当前的误差协方差
    float out; // 滤波后的输出值
    float Kg; // 卡尔曼增益
    float Q; // 过程噪声协方差
    float R; // 测量噪声协方差
} KalmanFilter_Struct;

extern KalmanFilter_Struct kfs[3]; // X、Y、Z轴卡尔曼滤波器实例

int16_t Common_Filter_LowPass(int16_t newValue,int16_t preFilterValue);

double Common_Filter_KalmanFilter(KalmanFilter_Struct *kfs, double input);



#endif // COM_FILTER_H