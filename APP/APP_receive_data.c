#include "APP_receive_data.h"
#include "APP_FreeRTOS_Task.h"
#include "Com_config.h"
#include <stdint.h>

//yaw，pit，roll初始值500
Remote_Data remote_data = {.throttle=0,.yaw=500, .pitch=500, .roll=500, .altitude=0, .shutdown=0 }; // 定义一个全局变量来存储接收到的遥控数据

//定义数组来存储接收到的数据，大小为TX_PLOAD_WIDTH字节，初始值为0
uint8_t receive_buffer[TX_PLOAD_WIDTH] = {0};

static uint32_t max_state_start_time = 0; // 记录进入MAX状态的时间
static uint32_t min_state_start_time = 0; // 记录进入MIN状态的时间
//宏定义帧头
#define FRAME_HEADER_1 'Y'
#define FRAME_HEADER_2 'J'
#define FRAME_HEADER_3 'R'

//接收遥控数据并且解析
//返回值：0:接收到数据校验通过，1:没有接收到数据，或者校验失败
uint8_t APP_receive_data(void)
{
    //清楚一下接收缓冲区，避免上次的数据干扰，调用memset
    memset(receive_buffer, 0, sizeof(receive_buffer));
    Int_SI24R1_TxPacket(receive_buffer); // 从SI24R1接收数据并存储到receive_buffer中
    // 在这里可以添加数据解析逻辑，将receive_buffer中的数据填充到remote_data结构体中
    //先判断是否受到数据
    if(strlen((char*)receive_buffer) == 0)
    {
        return 1; // 没有接收到数据
    }
    
    //前三位是帧头，用来判断数据的有效性，可以根据实际情况修改帧头的内容和长度
    if(receive_buffer[0] != FRAME_HEADER_1 || receive_buffer[1] != FRAME_HEADER_2 || receive_buffer[2] != FRAME_HEADER_3)
    {
        debug_printf("接收到的数据帧头错误，数据无效\r\n");
        return 1; // 数据帧头错误，数据无效
    }
    //计算校验和，简单的累加方式
    uint32_t checksum = 0;
    //定义一个变量解析接收到的校验和
    uint32_t received_checksum = (receive_buffer[13] << 24) | (receive_buffer[14] << 16) | (receive_buffer[15] << 8) | receive_buffer[16];
    //帧尾校验，和发送端保持一直
    for (uint8_t i = 0; i < 13; i++)
    {
        checksum += receive_buffer[i];
    }
    
    if (checksum != received_checksum)
    {
        debug_printf("接收到的数据校验失败，数据无效\r\n");
        return 1; // 数据校验失败，数据无效
    }

    //高位在前接收的数据格式为：油门高字节，油门低字节，偏航高字节，偏航低字节，俯仰高字节，俯仰低字节，滚转高字节，滚转低字节
    // 定高1字节，关机1字节，校验和4字节（可以根据实际情况调整校验和的计算方式和长度）
    if (APP_RemoteData_Lock(portMAX_DELAY) != pdTRUE)
    {
        return 1; // 无法获取互斥锁，数据无效
    }

    remote_data.throttle = (int16_t)(receive_buffer[3] << 8 | receive_buffer[4]);
    remote_data.yaw = (int16_t)(receive_buffer[5] << 8 | receive_buffer[6]);
    remote_data.pitch = (int16_t)(receive_buffer[7] << 8 | receive_buffer[8]);
    remote_data.roll = (int16_t)(receive_buffer[9] << 8 | receive_buffer[10]);
    remote_data.altitude = (int8_t)receive_buffer[11];
    remote_data.shutdown = (int8_t)receive_buffer[12];

    APP_RemoteData_Unlock();
    //用VOFAT打印接收到的数据，方便调试
    debug_printf(":%d,%d,%d,%d,%d,%d\n", remote_data.throttle, remote_data.yaw, remote_data.pitch, remote_data.roll, remote_data.altitude, remote_data.shutdown);

    
    return 0; // 返回0表示接收到数据且校验通过
}


//处理连接状态函数
//connection_status:1表示遥控器未连接，0表示遥控器已连接,APP_receive_data函数的返回值
void APP_connection_state(uint8_t connection_status)
{
    if(connection_status == 0)
    {
        //遥控器已连接，更新状态变量
        aircraft_state.remote_state = REMOTE_CONNECTED;
    }
    else if(connection_status == 1)
    {   //需要有一个重试次数，有可能只是不稳点造成短暂未收到数据，避免因为偶尔一次接收失败就认为遥控器未连接
        static uint8_t retry_count = 0; // 定义一个静态变量来记录连续接收失败的次数
        retry_count++;
        if (retry_count > R_MAX_RETRY_COUNT) // 连续接收失败超过最大重试次数，认为遥控器未连接
        {
            aircraft_state.remote_state = REMOTE_DISCONNECTED;
            retry_count = 0; // 重置计数器
        }
    }

}

//解锁静态函数,飞机状态解锁逻辑，0表示解锁成功，1表示解锁失败
static uint8_t APP_process_unlock(void)
{   //考虑安全问题，解锁条件，解锁完成最终状态，应该是油门为0，避免直接起飞
    switch (aircraft_state.throttle_state) {
        case FREE:
            if (remote_data.throttle >= 900) {
                aircraft_state.throttle_state = MAX;
                max_state_start_time = xTaskGetTickCount(); // 记录进入MAX状态的时间
            }
            break;
        case MAX:
            // MAX状态下的处理逻辑
            //记录持续时间，要求油门持续在MAX状态超过一定时间才认为是解锁成功，避免偶尔一次达到MAX状态就解锁了
            if (remote_data.throttle <= 900) 
            {
                if (xTaskGetTickCount()-max_state_start_time >= pdMS_TO_TICKS(1000)) // 持续时间超过1秒，认为解锁成功
                {
                    //油门保持最高状态操作1s
                    aircraft_state.throttle_state =LEAV_MAX;
                }
                else {
                    //油门没有保持最高状态操作1s，认为解锁失败，重置状态
                    aircraft_state.throttle_state = FREE;
                }
            }
            break;
        case LEAV_MAX:
            // LEAV_MAX状态下的处理逻辑
            if (remote_data.throttle <= 100) {
                //油门回到最低状态，认为解锁成功
                aircraft_state.throttle_state = MIN;
                min_state_start_time = xTaskGetTickCount(); // 记录进入MIN状态的时间
            }
            break;
        case MIN:
            // MIN状态下的处理逻辑
            // 记录持续时间，要求油门持续在MIN状态超过一定时间才认为是解锁成功，避免偶尔一次达到MIN状态就解锁了
            if (xTaskGetTickCount()-min_state_start_time >= pdMS_TO_TICKS(1000)) // 持续时间超过1秒，认为解锁成功
            { 
                //油门保持最低状态操作1s，认为解锁成功
                aircraft_state.throttle_state = UNLOCK;
            }
            else {
                if (aircraft_state.throttle_state > 100) 
                {
                //油门没有保持最低状态操作1s，认为解锁失败，重置状态
                aircraft_state.throttle_state = FREE;
                }
            }
            break;
        case UNLOCK:
            // 解锁状态下的处理逻辑
            break;
        default:
            break;
    }
    if (aircraft_state.throttle_state == UNLOCK) {
        return 0; // 解锁成功
    }

    return 1; // 这里直接返回1表示解锁失败，实际应用中可以添加更多的解锁条件和逻辑，如接收特定的解锁指令、验证安全码等
    
}


//处理飞行状态函数，根据接收到的遥控数据来判断当前飞行状态
void APP_process_flight_state(void)
{
    //使用状态机逻辑实现
    //1、使用轮询的方式，根据接收到的遥控数据来判断当前飞行状态，但本身Freertos任务就是轮询的
    switch (aircraft_state.flight_state) 
    {   //2、只需要编写指向其他状态得代码即可
        case FLIGHT_IDLE:
            if  (APP_process_unlock() == 0) 
            {
                aircraft_state.flight_state = FLIGHT_NORMAL;
                aircraft_state.throttle_state = FREE; // 解锁成功后更新油门状态
            }
            break;
        case FLIGHT_NORMAL:
            //3、普通状态进入定高状态和故障状态
            if (remote_data.altitude > 1)
            {
                aircraft_state.flight_state = FLIGHT_HEIGHT;
                remote_data.altitude = 0; // 定高状态下不再处理定高值，避免干扰飞行控制算法
            }
            //故障状态的判断
            else if (aircraft_state.remote_state == REMOTE_DISCONNECTED)
            {
                aircraft_state.flight_state = FLIGHT_FALLING;
            }
            break;
        case FLIGHT_HEIGHT:
            {
                //取消定高状态
                if (remote_data.altitude == 1)
                {
                    aircraft_state.flight_state = FLIGHT_NORMAL;
                    remote_data.altitude = 0;
                }
                //定高状态下的故障判断
               if (aircraft_state.remote_state == REMOTE_DISCONNECTED)
               {
                    aircraft_state.flight_state = FLIGHT_FALLING;
                }
            }
            break;
        case FLIGHT_FALLING:
            {
                //处理失联故障缓慢停止电机，后续实现

                aircraft_state.flight_state = FLIGHT_IDLE; // 这里直接切换回空闲状态，实际应用中可以添加更多的故障处理逻辑，如触发降落伞、发送警报等
            }
            break;
        default:
            break;
    }
}