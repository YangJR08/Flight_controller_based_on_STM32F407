#ifndef COM_CONFIG_H
#define COM_CONFIG_H

#include "main.h"
// 系统配置参数


//表示当前连接状态
typedef enum{
    REMOTE_CONNECTED = 0, // 遥控器已连接
    REMOTE_DISCONNECTED,  // 遥控器已断开
} Remote_State;

//飞行状态
typedef enum{
    FLIGHT_IDLE = 0, // 飞行器空闲
    FLIGHT_NORMAL,      // 正常飞行
    FLIGHT_HEIGHT,     // 定高中飞行
    FLIGHT_FALLING,    // 故障
} Flight_State;


#endif // COM_CONFIG_H