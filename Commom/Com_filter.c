#include "Com_filter.h"

#define ALPHA 0.15 // 滤波系数，取值范围0-1，ALPHA越大，滤波效果越好，但是响应速度越慢，ALPHA越小，滤波效果越差，但是响应速度越快，所以需要根据实际需求来设置ALPHA的值，这里我们设置为0.5

int16_t Common_Filter_LowPass(int16_t newValue,int16_t preFilterValue)
{
    //简单的一阶滤波器，y(n)=a*x(n)+(1-a)*y(n-1)，a是滤波系数，取值范围0-1，a越大，滤波效果越好，但是响应速度越慢，a越小，滤波效果越差，但是响应速度越快，所以需要根据实际需求来设置a的值，这里我们设置为0.5
    int16_t filterValue = (int16_t)(ALPHA * newValue + (1 - ALPHA) * preFilterValue);
    return filterValue;
}

KalmanFilter_Struct kfs[3]={
    {0.02, 0, 0, 0, 0.001, 0.543}, // X轴卡尔曼滤波器实例，初始误差协方差为0，过程噪声协方差为0.01，测量噪声协方差为0.1
    {0.02, 0, 0, 0, 0.001, 0.543}, // Y轴卡尔曼滤波器实例，初始误差协方差为0，过程噪声协方差为0.01，测量噪声协方差为0.1
    {0.02, 0, 0, 0, 0.001, 0.543}  // Z轴卡尔曼滤波器实例，初始误差协方差为0，过程噪声协方差为0.01，测量噪声协方差为0.1
}; // X、Y、Z轴卡尔曼滤波器实例

double Common_Filter_KalmanFilter(KalmanFilter_Struct *kf, double input)
{
    //卡尔曼滤波器算法实现，输入参数为卡尔曼滤波器结构体指针和新的测量值，返回值为滤波后的值
    kf->Now_P = kf->LastP + kf->Q; // 预测误差协方差
    kf->Kg = kf->Now_P / (kf->Now_P + kf->R); // 计算卡尔曼增益
    kf->out = kf->out + kf->Kg * (input - kf->out); // 更新滤波后的值
    kf->LastP = (1 - kf->Kg) * kf->Now_P; // 更新误差协方差
    return kf->out; // 返回滤波后的值

}