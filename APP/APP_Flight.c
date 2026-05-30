#include "APP_Flight.h"
#include "APP_receive_data.h"
#include "Com_imu.h"
#include "com_debug.h"

/*
直接使用
Motor_Struct motor_con[MOTOR_NUM] ={
    {&htim1, TIM_CHANNEL_4, 200}, //左上电机
    {&htim1, TIM_CHANNEL_2, 200}, //左下电机
    {&htim1, TIM_CHANNEL_1, 200}, //右上电机
    {&htim1, TIM_CHANNEL_3, 200}  //右下电机
};
*/

Gyro_Accel_struct  gyro_accel_data; // 定义一个全局变量来存储陀螺仪和加速度计的数据
//上次滤波后的值
Gyro_struct last_gyro_data = {0}; // 定义一个全局变量来存储上次滤波后的陀螺仪
Euler_Angle_struct euler_angle_data; // 定义一个全局变量来存储解算得到的欧拉角数据

/*-------PID------*/
//俯仰角PID结构体
PID_TypeDef pitch_pid = {
    .kp = -7.0f, // 比例增益
    .ki = 0.0f, // 积分增益
    .kd = 0.0f, // 微分增益
    .error = 0.0f,
    .desire = 0.0f,
    .measure = 0.0f,
    .last_error = 0.0f,
    .integral = 0.0f,
    .output = 0.0f
};
//俯仰角的内环PID结构体，Y轴角速度PID
PID_TypeDef pitch_rate_pid = {
    .kp = 3.0f, // 比例增益
    .ki = 0.0f, // 积分增益
    .kd = 0.5f, // 微分增益
    .error = 0.0f,
    .desire = 0.0f,
    .measure = 0.0f,
    .last_error = 0.0f,
    .integral = 0.0f,
    .output = 0.0f
};
//横滚角PID结构体
PID_TypeDef roll_pid = {
    .kp = -7.0f, // 比例增益
    .ki = 0.0f, // 积分增益
    .kd = 0.0f, // 微分增益
    .error = 0.0f,
    .desire = 0.0f,
    .measure = 0.0f,
    .last_error = 0.0f,
    .integral = 0.0f,
    .output = 0.0f
};
//横滚角的内环PID结构体，X轴角速度PID
PID_TypeDef roll_rate_pid = {
    .kp = 3.0f, // 比例增益
    .ki = 0.0f, // 积分增益
    .kd = 0.5f, // 微分增益
    .error = 0.0f,
    .desire = 0.0f,
    .measure = 0.0f,
    .last_error = 0.0f,
    .integral = 0.0f,
    .output = 0.0f
};
//偏航角PID结构体
PID_TypeDef yaw_pid = {
    .kp = -3.0f, // 比例增益
    .ki = 0.0f, // 积分增益
    .kd = 0.0f, // 微分增益
    .error = 0.0f,
    .desire = 0.0f,
    .measure = 0.0f,
    .last_error = 0.0f,
    .integral = 0.0f,
    .output = 0.0f
};
//偏航角的内环PID结构体，Z轴角速度PID
PID_TypeDef yaw_rate_pid = {
    .kp = -5.0f, // 比例增益
    .ki = 0.0f, // 积分增益
    .kd = 0.0f, // 微分增益
    .error = 0.0f,
    .desire = 0.0f,
    .measure = 0.0f,
    .last_error = 0.0f,
    .integral = 0.0f,
    .output = 0.0f
};
/*-------PID------*/

void APP_Flight_Init(void)
{
    //1、初始化mpu6050
    Int_MPU6050_Init();
    //2、初始化电机
    for(int i=0;i<MOTOR_NUM;i++)
    {
        Int_motor_start(&motor_con[i]);
    }

}


//根据陀螺仪计算出欧拉角，单位是度
void APP_Flight_Get_euler_angel(void)
{
    //1、使用mpu6050的硬件接口得到六轴数据
    Int_MPU6050_Get_Data(&gyro_accel_data);
    //先打印角速度
    //debug_printf(":%d,%d,%d\n", gyro_accel_data.gyro.gyro_x, gyro_accel_data.gyro.gyro_y, gyro_accel_data.gyro.gyro_z);
    //再打印加速度
    //debug_printf(":%d,%d,%d\n", gyro_accel_data.accel.accel_x, gyro_accel_data.accel.accel_y, gyro_accel_data.accel.accel_z);
    //2、对角速度低通滤波，对采集数据及时性比较高，傅里叶变换延时比较大，所以我们直接使用简单的一阶滤波iir滤波器，y(n)=a*x(n)+(1-a)*y(n-1)，a是滤波系数，取值范围0-1，a越大，滤波效果越好，但是响应速度越慢，a越小，滤波效果越差，但是响应速度越快，所以需要根据实际需求来设置a的值，这里我们设置为0.5
    last_gyro_data.gyro_x = gyro_accel_data.gyro.gyro_x = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_x, last_gyro_data.gyro_x);
    last_gyro_data.gyro_y = gyro_accel_data.gyro.gyro_y = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_y, last_gyro_data.gyro_y);
    last_gyro_data.gyro_z = gyro_accel_data.gyro.gyro_z = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_z, last_gyro_data.gyro_z);
    //3、加速度抖动比较大，使用卡尔曼滤波来滤波，卡尔曼滤波的原理是根据系统的状态方程和测量方程来估计系统的状态，卡尔曼滤波器会根据系统的状态方程来预测系统的状态，然后根据测量方程来更新系统的状态，这样可以有效地滤除噪声，提高测量的准确性，所以我们需要定义一个卡尔曼滤波器结构体来存储卡尔曼滤波器的参数和状态，然后实现一个卡尔曼滤波函数来进行滤波，这样可以提高加速度数据的准确性，减少抖动的影响
    gyro_accel_data.accel.accel_x = Common_Filter_KalmanFilter(&kfs[0], gyro_accel_data.accel.accel_x);
    gyro_accel_data.accel.accel_y = Common_Filter_KalmanFilter(&kfs[1], gyro_accel_data.accel.accel_y);
    gyro_accel_data.accel.accel_z = Common_Filter_KalmanFilter(&kfs[2], gyro_accel_data.accel.accel_z);

    //4、练习互补解算后续移植四元数解算
    //优先使用加速度解计算
    //euler_angle_data.pitch = atan2(gyro_accel_data.accel.accel_x, gyro_accel_data.accel.accel_z) * 180 / M_PI;
    //euler_angle_data.roll = atan2(gyro_accel_data.accel.accel_y, gyro_accel_data.accel.accel_z) * 180 / M_PI;
    //偏航角只能角速度积分
    //任务时间是6ms
    //euler_angle_data.yaw += gyro_accel_data.gyro.gyro_z * 0.006; // 0.006是采样时间，单位是秒，这里假设采样时间为10ms，所以我们把角速度乘以0.01来得到偏航角的增量，然后累加到偏航角上，这样就得到了当前的偏航角值，单位是度
    Common_IMU_GetEulerAngle(&gyro_accel_data, &euler_angle_data, 0.006);
    
}

void APP_flight_pid_process(void)
{
    //对俯仰角进行PID计算
    //外环PID计算，如果是平稳飞行，就是0，如果需要遥控飞行就是遥控器的值
    pitch_pid.desire = (remote_data.pitch - 500) * 0.02f; // 期望值，这里假设我们希望保持水平飞行，所以期望值为0度
    pitch_pid.measure = euler_angle_data.pitch; // 测量值，当前的俯仰角

    //内环PID计算，期望值是外环的输出，测量值是当前的角速度
    pitch_rate_pid.desire = pitch_pid.output; // 期望值，外环PID的输出
    pitch_rate_pid.measure = gyro_accel_data.gyro.gyro_y;

    //计算双环PID
    Com_PID_Calculate_Chain(&pitch_pid, &pitch_rate_pid);
    //debug_printf(":%f,%f,%f\n", pitch_pid.error,pitch_pid.output, pitch_rate_pid.output);

    //对横滚角进行PID计算
    roll_pid.desire = (remote_data.roll - 500) * 0.02f; // 期望值，这里假设我们希望保持水平飞行，所以期望值为0度
    roll_pid.measure = euler_angle_data.roll; // 测量值，当前的横滚角

    //内环PID计算，期望值是外环的输出，测量值是当前的角速度
    roll_rate_pid.desire = roll_pid.output; // 期望值，外环PID的输出
    roll_rate_pid.measure = gyro_accel_data.gyro.gyro_x;

    //计算双环PID
    Com_PID_Calculate_Chain(&roll_pid, &roll_rate_pid);
    //debug_printf(":%f,%f,%f\n", roll_pid.error,roll_pid.output, roll_rate_pid.output);
    
    //对偏航角进行PID计算
    yaw_pid.desire = (remote_data.yaw - 500) * 0.02f; // 期望值，这里假设我们希望保持水平飞行，所以期望值为0度
    yaw_pid.measure = euler_angle_data.yaw; // 测量值，当前的偏航角

    //内环PID计算，期望值是外环的输出，测量值是当前的角速度
    yaw_rate_pid.desire = yaw_pid.output; // 期望值，外环PID的输出
    yaw_rate_pid.measure = gyro_accel_data.gyro.gyro_z;

    //计算双环PID
    Com_PID_Calculate_Chain(&yaw_pid, &yaw_rate_pid);
    //debug_printf(":%f,%f,%f\n", yaw_pid.error,yaw_pid.output, yaw_rate_pid.output);
}

//根据PID的输出来控制电机的速度
void APP_flight_control_motor(void)
{
    //判断当前飞行状态，根据不同的飞行状态来控制电机的速度，这里我们以FLIGHT_NORMAL为例，其他状态可以根据实际需求来设置不同的电机速度
    switch(aircraft_state.flight_state)
    {
        case FLIGHT_IDLE:
            //飞行器空闲，电机停止
            for(int i=0;i<MOTOR_NUM;i++)
            {
                motor_con[i].duty_cycle = 0;
            }
            break;
        case FLIGHT_NORMAL:
            //俯仰角向前飞有角速度，有向后飞，前两个电机转的快，后两个转的慢
            //正常飞行，根据PID输出控制电机速度
            //偏航角是对角线一组进行调节
            //对偏航角pid输出进行限制
            yaw_rate_pid.output = Com_Limit(yaw_rate_pid.output, -100, 100);
            motor_con[MOTOR_LEFT_UP].duty_cycle = remote_data.throttle + pitch_rate_pid.output - roll_rate_pid.output + yaw_rate_pid.output;
            motor_con[MOTOR_LEFT_DOWN].duty_cycle = remote_data.throttle - pitch_rate_pid.output - roll_rate_pid.output - yaw_rate_pid.output;
            motor_con[MOTOR_RIGHT_UP].duty_cycle = remote_data.throttle + pitch_rate_pid.output + roll_rate_pid.output - yaw_rate_pid.output;
            motor_con[MOTOR_RIGHT_DOWN].duty_cycle = remote_data.throttle - pitch_rate_pid.output + roll_rate_pid.output + yaw_rate_pid.output;
            break;
        case FLIGHT_HEIGHT:
            //定高中飞行，保持一定的高度，电机速度根据高度误差来调整，这里我们假设定高值为1米，实际应用中可以根据需要来设置定高值
            if(remote_data.altitude > 1)
            {
                //高度过高，降低电机速度
                for(int i=0;i<MOTOR_NUM;i++)
                {
                    motor_con[i].duty_cycle = remote_data.throttle - 50; // 这里假设我们降低50的占空比来降低高度，实际应用中可以根据需要来调整这个值
                }
            }
            else if(remote_data.altitude < 1)
            {
                //高度过低，提高电机速度
                for(int i=0;i<MOTOR_NUM;i++)
                {
                    motor_con[i].duty_cycle = remote_data.throttle + 50; // 这里假设我们提高50的占空比来提高高度，实际应用中可以根据需要来调整这个值
                }
            }
            else
            {
                //高度合适，保持当前电机速度
                for(int i=0;i<MOTOR_NUM;i++)
                {
                    motor_con[i].duty_cycle = remote_data.throttle;
                }
            }
            break;
        case FLIGHT_FALLING:

    }

    //安全限制，当油门设置小于50时，强制电机停止，避免飞行器失控
    if(remote_data.throttle < 50)
    {
        for(int i=0;i<MOTOR_NUM;i++)
        {
            motor_con[i].duty_cycle = 0;
        }

        //设置电机速度
        for(int i=0;i<MOTOR_NUM;i++)
        {   
            //其实在电机控制中也对占空比进行了600的限制
            motor_con[i].duty_cycle = Com_Limit(motor_con[i].duty_cycle, 0, 600);
            Int_motor_set_speed(&motor_con[i]);
        }
    }
}