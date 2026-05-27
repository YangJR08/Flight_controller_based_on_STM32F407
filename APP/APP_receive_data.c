#include "APP_receive_data.h"


Remote_Data remote_data = {0}; // 定义一个全局变量来存储接收到的遥控数据

//定义数组来存储接收到的数据，大小为TX_PLOAD_WIDTH字节，初始值为0
uint8_t receive_buffer[TX_PLOAD_WIDTH] = {0};

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