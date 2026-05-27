#ifndef APP_RECEIVE_DATA_H
#define APP_RECEIVE_DATA_H

#include "APP_FreeRTOS_Task.h"
#include <stdint.h>
#include "APP_mutex.h"
#include <string.h>

//遥控数据定义，保持和遥控器发送的数据格式一致，方便解析
typedef struct
{
    int16_t throttle;  //油门值，范围
    int16_t yaw;       //偏航值，范围
    int16_t pitch;     //俯仰值，范围
    int16_t roll;      //滚转值，范围
    int8_t altitude;  //定高值，范围
    int8_t shutdown;  //关机值，范围
} Remote_Data;


//接收遥控数据并且解析
//返回值：0:接收到数据校验通过，1:没有接收到数据，或者校验失败
uint8_t APP_receive_data(void);

#endif // APP_RECEIVE_DATA_H