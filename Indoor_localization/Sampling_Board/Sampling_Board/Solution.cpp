/*
*********************************************************************************************************
*
*	文件名称 : Solution.cpp
*	版    本 : V1.0
*	说    明 : 这是室内定位项目足底压力传感器阵列压力结算程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-15     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Solution.h"
#include "Function.h"

/*
*********************************************************************************************************
*	                                      变量声明
*********************************************************************************************************
*/
/* Plantar缓冲区修改互斥锁 */
extern SemaphoreHandle_t Plantar_Buff_Mutex;
/* 压力数据缓存区 */
extern float Pressure_Buff[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];
/* 时间戳数组 */
extern uint32_t Time_Stamp[FRAME_IN_BUFF];
/* ADC采样数据 */
extern uint32_t voltage[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN];       //ADC读出转化后数据存放
/* 压力传感器阵列行列数定义 */
extern const uint8_t Sensor_Num_Line;       //压力传感器阵列行数
extern const uint8_t Sensor_Num_Column[SENSOR_NUM_LINE][2];      //压力传感器阵列列数
/* 蓝牙串口声明 */
extern BluetoothSerial SerialBT;
/* 压力传感器阵列数据缓冲区变量声明 */
uint32_t Plantar_Voltage_Buff_1[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //压力传感器Buff1
uint32_t Plantar_Voltage_Buff_2[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];          //压力传感器Buff2
/* IMU数据缓冲区变量声明 */
float IMU_Accelerated_Buff_1[3*FRAME_IN_BUFF];              //IMU加速度Buff1
float IMU_Accelerated_Buff_2[3*FRAME_IN_BUFF];              //IMU加速度Buff2
float IMU_Gyroscope_Buff_1[3*FRAME_IN_BUFF];                //IMU陀螺仪Buff1
float IMU_Gyroscope_Buff_2[3*FRAME_IN_BUFF];                //IMU陀螺仪Buff2
float IMU_Angle_Buff_1[3*FRAME_IN_BUFF];                    //IMU倾角Buff1
float IMU_Angle_Buff_2[3*FRAME_IN_BUFF];                    //IMU倾角Buff2
int16_t IMU_Magnetic_Buff_1[3*FRAME_IN_BUFF];               //IMU磁力计Buff1
int16_t IMU_Magnetic_Buff_2[3*FRAME_IN_BUFF];               //IMU磁力计Buff2
int32_t IMU_Atmospheric_Buff_1[FRAME_IN_BUFF];              //IMU气压计Buff1
int32_t IMU_Atmospheric_Buff_2[FRAME_IN_BUFF];              //IMU气压计Buff2
int32_t IMU_Height_Buff_1[FRAME_IN_BUFF];                   //IMU高度Buff1
int32_t IMU_Height_Buff_2[FRAME_IN_BUFF];                   //IMU高度Buff2
float IMU_Quaternion_Buff_1[4*FRAME_IN_BUFF];               //IMU四元数Buff1
float IMU_Quaternion_Buff_2[4*FRAME_IN_BUFF];               //IMU四元数Buff2
/* 时间戳缓冲区变量声明 */
uint32_t Plantar_Time_Stamp_1[FRAME_IN_BUFF];
uint32_t Plantar_Time_Stamp_2[FRAME_IN_BUFF];
uint32_t IMU_Time_Stamp_1[FRAME_IN_BUFF];
uint32_t IMU_Time_Stamp_2[FRAME_IN_BUFF];
/* 结构体变量声明 */
IMU_Buff_TypeDef IMU_Buff;                      //惯性传感器Buff结构体声明
Plantar_Buff_TypeDef Plantar_Buff;              //压力传感器阵列Buff结构体声明
/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
/**
 * @brief Get_TimeStamp 获取时间戳
 * @return TimeStamp 32位时间戳
 */
inline uint32_t Get_TimeStamp(void)
{
    return millis();
}

/**
 * @brief Plantar_Solution 构造函数
 */
Plantar_Solution::Plantar_Solution()
{
    Solution_LOG("Plantar Solution Object Created \n\r");
}

/**
 * @brief ~Plantar_Solution 析构函数
 */
Plantar_Solution::~Plantar_Solution()
{
    Solution_LOG("Plantar Solution Object Deleted \n\r");
}

/**
 * @brief Buff_Init 压力传感器阵列数据缓冲区初始化
 *
 */
void Plantar_Solution::Buff_Init(void)
{
    /* 初始化Buff */
    Plantar_Buff.Write_Buff = Plantar_Voltage_Buff_1;
    Plantar_Buff.Read_Buff = Plantar_Voltage_Buff_2;
    Plantar_Buff.Write_Time_Buff = Plantar_Time_Stamp_1;
    Plantar_Buff.Read_Time_Buff = Plantar_Time_Stamp_2;
    Plantar_Buff.Write = 0;
    Plantar_Buff.Read = 0;
    Plantar_Buff.Write_Frame = 0;
    //Plantar_Buff.Read_Frame = 0;
}

/**
 * @brief Buff_Full_Judge 判断缓冲区是否写满
 *
 * @return int8_t -1缓冲区数据个数错误  0缓冲区未写满  1缓冲区已经写满
 */
int8_t Plantar_Solution::Buff_Full_Judge(void)
{
    Solution_LOG("Amount:%d \n\r",Plantar_Buff.Write);
    if(Plantar_Buff.Write % SENSOR_NUM_TOTAL != 0)
    {
        Solution_LOG("Error number of buffer data \n\r");
        return -1;
    }    
    if(Plantar_Buff.Write >= SENSOR_NUM_TOTAL*FRAME_IN_BUFF)
    {
        return 1;
    }
    else
    {
        return 0;
    }                         
}

/**
 * @brief Read_Write_Buff_Switch 交换读写缓冲区
 *
 * @return int8_t -1表示读写缓冲区异常  1表示读写缓冲区交换成功
 */
int8_t Plantar_Solution::Read_Write_Buff_Switch(void)
{
    uint32_t *read_buff;        
    uint32_t *write_buff;
    uint32_t *read_time_buff;        
    uint32_t *write_time_buff;
    /* 判断读写缓冲区状态是否异常 */
    if(Plantar_Buff.Write_Buff == NULL || Plantar_Buff.Read_Buff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_buff = Plantar_Buff.Read_Buff;
    write_buff = Plantar_Buff.Write_Buff;
    read_time_buff = Plantar_Buff.Read_Time_Buff;
    write_time_buff = Plantar_Buff.Write_Time_Buff;
    /* 交换读写缓冲区 */
    #if PLANTAR_BUFF_MUTEX_ENABLE > 0u
        if(Plantar_Buff_Mutex != NULL)      //Plantar缓冲区修改互斥锁不为空
        {
            if(xSemaphoreTake(Plantar_Buff_Mutex, portMAX_DELAY) == pdTRUE)       //申请Plantar缓冲区修改互斥锁
            {
    #endif            
                /* 设置读写缓冲区 */
                Plantar_Buff.Read_Buff = write_buff;
                Plantar_Buff.Write_Buff = read_buff;
                /* 设置时间戳读写缓冲区 */
                Plantar_Buff.Read_Time_Buff = write_time_buff;
                Plantar_Buff.Write_Time_Buff = read_time_buff;
                /* 设置数据计数值 */
                Plantar_Buff.Read = 0;
                Plantar_Buff.Write = 0;
                Plantar_Buff.Write_Frame = 0;
                //Plantar_Buff.Read_Frame = 0;
    #if PLANTAR_BUFF_MUTEX_ENABLE > 0u
                xSemaphoreGive(Plantar_Buff_Mutex);           //释放Plantar缓冲区修改互斥锁
            }
            else
            {
                Solution_LOG("Application for mutex failed \n\r");      //申请Plantar缓冲区修改互斥锁失败
                return -1;
            }
        }
        else
        {
            Solution_LOG("The mutex is not created \n\r");      //Plantar缓冲区修改互斥锁未创建
        }            
    #endif

    return 1;                     
}

/**
 * @brief TimeStamp 记录时间戳
 *
 */
void Plantar_Solution::TimeStamp(void)
{
    *(Plantar_Buff.Write_Time_Buff + Plantar_Buff.Write_Frame) = Get_TimeStamp();
    Plantar_Buff.Write_Frame++;
}


/**
 * @brief Put_into_Buff 将采集出来的压力传感器阵列存入相应缓冲区
 *
 * @param uint32_t voltage_p[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN]
 * @return int8_t -1 存入Buff失败      1 存入Buff成功
 */
int8_t Plantar_Solution::Put_into_Buff(uint32_t voltage_p[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN])
{
    uint8_t i = 0,j = 0;
    /* 判断写缓冲区状态是否异常 */
    if(Plantar_Buff.Write_Buff == NULL)         //写缓冲区不指向NULL
    {
        Solution_LOG("Write buffer pointer error \n\r");
        return -1;       
    } 
    /* 向缓冲区写入数据 */
    #if PLANTAR_BUFF_MUTEX_ENABLE > 0u
        if(Plantar_Buff_Mutex != NULL)      //Plantar缓冲区修改互斥锁不为空
        {
            if(xSemaphoreTake(Plantar_Buff_Mutex, portMAX_DELAY) == pdTRUE)       //申请Plantar缓冲区修改互斥锁
            {
    #endif 
                while(i < Sensor_Num_Line)          //从第一行开始按顺序采集
                {
                    j = Sensor_Num_Column[i][0];
                    while(j <= Sensor_Num_Column[i][1])      //采集一行时从第一列开始顺序采集
                    {
                        *(Plantar_Buff.Write_Buff + Plantar_Buff.Write) = voltage_p[i][j];
                        Plantar_Buff.Write++;
                        j++;
                    }
                    i++;
                }
    #if PLANTAR_BUFF_MUTEX_ENABLE > 0u
                xSemaphoreGive(Plantar_Buff_Mutex);           //释放Plantar缓冲区修改互斥锁
            }
            else
            {
                Solution_LOG("Application for mutex failed \n\r");      //申请Plantar缓冲区修改互斥锁失败
                return -1;
            }
        }
        else
        {
            Solution_LOG("The mutex is not created \n\r");      //Plantar缓冲区修改互斥锁未创建
            return -1;
        }            
    #endif

    return 1;                     
}

// /**
//  * @brief Singel_Point_Calculation  单采样点解算得到压力值
//  *
//  * @param uint32_t *voltage
//  */
// float Plantar_Solution::Singel_Point_Calculation(uint32_t *voltage)
// {
//     float vol;
//     float conductance;
//     float pressure;
//     vol = (float)(*voltage)/1000;       
//     conductance = vol/(2*(prar_V - vol));
//     float index = (conductance - para_b)/para_c;
//     pressure = para_a*exp(-index*index);
//     Solution_LOG("Pressure :%f g \n\r",pressure);     //输出压力值

//     return pressure;
// }

/**
 * @brief Singel_Point_Calculation  单采样点解算得到压力值
 *
 * @param uint32_t *voltage
 */
float Plantar_Solution::Singel_Point_Calculation(uint32_t *voltage)
{
    float vol;
    float conductance;
    float pressure;
    vol = (float)(*voltage)/1000;       
    conductance = ((vol/vref)-1)/Rf;
    float index = (conductance*1000 - para_b)/para_c;
    pressure = para_a*exp(-index*index);
    Solution_LOG("Conductance :%f \n\r",conductance);     //输出电导值
    Solution_LOG("Pressure :%f g \n\r",pressure);     //输出压力值

    return pressure;
}

// /**
//  * @brief Conductance_Calculation  单采样点解算得到电导值
//  * @brief uint32_t *voltage  测量出的电压值
//  * @return float 返回电导值
//  */
// float Plantar_Solution::Conductance_Calculation(uint32_t *voltage)
// {
//     float vol;
//     float conductance;
//     /* 计算电导值 */
//     vol = (float)(*voltage)/1000;
//     conductance = vol/(2*(prar_V - vol));
//     Solution_LOG("Conductance :%f \n\r",conductance);     //输出压力值

//     return conductance;        
// }

/**
 * @brief Conductance_Calculation  单采样点解算得到电导值
 * @brief uint32_t *voltage  测量出的电压值
 * @return float 返回电导值
 */
float Plantar_Solution::Conductance_Calculation(uint32_t *voltage)
{
    float vol;
    float conductance;
    /* 计算电导值 */
    vol = (float)(*voltage)/1000;
    conductance = ((vol/vref)-1)/Rf;;
    Solution_LOG("Conductance :%f \n\r",conductance);     //输出电导值

    return conductance;        
}

// /**
//  * @brief Pressure_Calculation  对缓冲区中的数据进行解算，得到压力值
//  *
//  * @param float *Buff
//  * @return int8_t -1 解算失败      1 解算成功
//  */
// int8_t Plantar_Solution::Pressure_Calculation(float *buff)
// {
//     uint32_t *pointer;
//     float voltage;
//     float conductance;
//     /* 判断读缓冲区状态是否异常 */
//     if(Plantar_Buff.Read_Buff == NULL)         //读缓冲区不指向NULL
//     {
//         Solution_LOG("Read buffer pointer error \n\r");
//         return -1;       
//     }
//     /* 解算压力值 */
//     while(Plantar_Buff.Read < SENSOR_NUM_TOTAL*FRAME_IN_BUFF)
//     {
//         voltage = (float)*(Plantar_Buff.Read_Buff + Plantar_Buff.Read)/1000;       
//         conductance = voltage/(2*(prar_V - voltage));
//         float index = (conductance - para_b)/para_c;
//         buff[Plantar_Buff.Read] = para_a*exp(-index*index);
//         Plantar_Buff.Read++; 
//     }
//     Solution_LOG("Pressure[0] :%f g \n\r",buff[0]);     //输出压力值

//     return 1;        
// }

/**
 * @brief Pressure_Calculation  对缓冲区中的数据进行解算，得到压力值
 *
 * @param float *Buff
 * @return int8_t -1 解算失败      1 解算成功
 */
int8_t Plantar_Solution::Pressure_Calculation(float *buff)
{
    uint32_t *pointer;
    float voltage;
    float conductance;
    /* 判断读缓冲区状态是否异常 */
    if(Plantar_Buff.Read_Buff == NULL)         //读缓冲区不指向NULL
    {
        Solution_LOG("Read buffer pointer error \n\r");
        return -1;       
    }
    /* 解算压力值 */
    while(Plantar_Buff.Read < SENSOR_NUM_TOTAL*FRAME_IN_BUFF)
    {
        voltage = (float)*(Plantar_Buff.Read_Buff + Plantar_Buff.Read)/1000;       
        conductance = ((voltage/vref)-1)/Rf;;
        float index = (conductance*1000 - para_b)/para_c;
        buff[Plantar_Buff.Read] = para_a*exp(-index*index);
        Plantar_Buff.Read++; 
    }
    Solution_LOG("Pressure[0] :%f g \n\r",buff[0]);     //输出压力值

    return 1;        
}

/**
 * @brief IMU_Solution 构造函数
 */
IMU_Solution::IMU_Solution()
{
    Solution_LOG("IMU Solution Object Created \n\r");
}

/**
 * @brief ~IMU_Solution 析构函数
 */
IMU_Solution::~IMU_Solution()
{
    Solution_LOG("IMU Solution Object Deleted \n\r");
}

/**
 * @brief Buff_Init IMU数据缓冲区初始化
 *
 */
void IMU_Solution::Buff_Init(void)
{
    /* 初始化Buff */
    IMU_Buff.Write_AccBuff = IMU_Accelerated_Buff_1;
    IMU_Buff.Read_AccBuff = IMU_Accelerated_Buff_2;
    IMU_Buff.Write_GyroBuff = IMU_Gyroscope_Buff_1;
    IMU_Buff.Read_GyroBuff = IMU_Gyroscope_Buff_2;
    IMU_Buff.Write_AngleBuff = IMU_Angle_Buff_1;
    IMU_Buff.Read_AngleBuff = IMU_Angle_Buff_2;
    IMU_Buff.Write_MagBuff = IMU_Magnetic_Buff_1;
    IMU_Buff.Read_MagBuff = IMU_Magnetic_Buff_2;
    IMU_Buff.Write_AtmoBuff = IMU_Atmospheric_Buff_1;
    IMU_Buff.Read_AtmoBuff = IMU_Atmospheric_Buff_2;
    IMU_Buff.Write_HightBuff = IMU_Height_Buff_1;
    IMU_Buff.Read_HightBuff = IMU_Height_Buff_2;
    IMU_Buff.Write_QuatBuff = IMU_Quaternion_Buff_1;
    IMU_Buff.Read_QuatBuff = IMU_Quaternion_Buff_2;
    IMU_Buff.Write_Time_Buff = IMU_Time_Stamp_1;
    IMU_Buff.Read_Time_Buff = IMU_Time_Stamp_2;
    IMU_Buff.Write_Frame = 0;
}

/**
 * @brief Buff_Full_Judge 判断缓冲区是否写满
 *
 * @return int8_t  0缓冲区未写满  1缓冲区已经写满
 */
int8_t IMU_Solution::Buff_Full_Judge(void)
{
    Solution_LOG("Number of write frames:%d \n\r",IMU_Buff.Write_Frame); 
    if(IMU_Buff.Write_Frame >= FRAME_IN_BUFF)
    {
        return 1;
    }
    else
    {
        return 0;
    }                         
}

/**
 * @brief Read_Write_Buff_Switch 交换读写缓冲区
 *
 * @return int8_t -1表示读写缓冲区异常  1表示读写缓冲区交换成功
 */
int8_t IMU_Solution::Read_Write_Buff_Switch(void)
{
    float *read_acc_buff;        
    float *write_acc_buff;
    float *read_gyro_buff;        
    float *write_gyro_buff;
    float *read_angle_buff;        
    float *write_angle_buff;
    int16_t *read_mag_buff;
    int16_t *write_mag_buff;
    int32_t *read_atmo_buff;
    int32_t *write_atmo_buff;
    int32_t *read_hight_buff;
    int32_t *write_hight_buff;
    float *read_quat_buff;
    float *write_quat_buff;
    uint32_t *read_timestamp_buff;
    uint32_t *write_timestamp_buff;    
    /* 判断加速度读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_AccBuff == NULL || IMU_Buff.Read_AccBuff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_gyro_buff = IMU_Buff.Read_AccBuff;
    write_gyro_buff = IMU_Buff.Write_AccBuff;
    /* 判断陀螺仪读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_GyroBuff == NULL || IMU_Buff.Read_GyroBuff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_acc_buff = IMU_Buff.Read_GyroBuff;
    write_acc_buff = IMU_Buff.Write_GyroBuff;
    /* 判断倾角读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_AngleBuff == NULL || IMU_Buff.Read_AngleBuff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_angle_buff = IMU_Buff.Read_AngleBuff;
    write_angle_buff = IMU_Buff.Write_AngleBuff;
    /* 判断磁力计读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_MagBuff == NULL || IMU_Buff.Read_MagBuff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_mag_buff = IMU_Buff.Read_MagBuff;
    write_mag_buff = IMU_Buff.Write_MagBuff;
    /* 判断气压计读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_AtmoBuff == NULL || IMU_Buff.Read_AtmoBuff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_atmo_buff = IMU_Buff.Read_AtmoBuff;
    write_atmo_buff = IMU_Buff.Write_AtmoBuff;
    /* 判断高度值读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_HightBuff == NULL || IMU_Buff.Read_HightBuff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_hight_buff = IMU_Buff.Read_HightBuff;
    write_hight_buff = IMU_Buff.Write_HightBuff;
    /* 判断四元数读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_QuatBuff == NULL || IMU_Buff.Read_QuatBuff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_quat_buff = IMU_Buff.Read_QuatBuff;
    write_quat_buff = IMU_Buff.Write_QuatBuff;
    /* 判断时间戳读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_Time_Buff == NULL || IMU_Buff.Read_Time_Buff == NULL)         //读写缓冲区不指向NULL
    {
        Solution_LOG("Read write buffer pointer error \n\r");
        return -1;       
    }
    read_timestamp_buff = IMU_Buff.Read_Time_Buff;
    write_timestamp_buff = IMU_Buff.Write_Time_Buff;
    /* 交换读写缓冲区 */
    #if IMU_BUFF_MUTEX_ENABLE > 0u
        if(IMU_Buff_Mutex != NULL)      //IMU缓冲区修改互斥锁不为空
        {
            if(xSemaphoreTake(IMU_Buff_Mutex, portMAX_DELAY) == pdTRUE)       //申请IMU缓冲区修改互斥锁
            {
    #endif            
                /* 设置写缓冲区 */
                IMU_Buff.Write_AccBuff = read_acc_buff;
                IMU_Buff.Write_GyroBuff = read_gyro_buff;
                IMU_Buff.Write_AngleBuff = read_angle_buff;
                IMU_Buff.Write_MagBuff = read_mag_buff;
                IMU_Buff.Write_AtmoBuff = read_atmo_buff;
                IMU_Buff.Write_HightBuff = read_hight_buff;
                IMU_Buff.Write_QuatBuff = read_quat_buff;
                IMU_Buff.Write_Time_Buff = read_timestamp_buff;                
                /* 设置读缓冲区 */
                IMU_Buff.Read_AccBuff = write_acc_buff;
                IMU_Buff.Read_GyroBuff = write_gyro_buff;
                IMU_Buff.Read_AngleBuff = write_angle_buff;
                IMU_Buff.Read_MagBuff = write_mag_buff;
                IMU_Buff.Read_AtmoBuff = write_atmo_buff;
                IMU_Buff.Read_HightBuff = write_hight_buff;
                IMU_Buff.Read_QuatBuff = write_quat_buff;
                IMU_Buff.Read_Time_Buff = write_timestamp_buff;
                /* 设置读写帧 */
                IMU_Buff.Write_Frame = 0;
    #if IMU_BUFF_MUTEX_ENABLE > 0u
                xSemaphoreGive(IMU_Buff_Mutex);           //释放IMU缓冲区修改互斥锁
            }
            else
            {
                Solution_LOG("Application for mutex failed \n\r");      //申请IMU缓冲区修改互斥锁失败
                return -1;
            }
        }
        else
        {
            Solution_LOG("The mutex is not created \n\r");      //IMU缓冲区修改互斥锁未创建
        }            
    #endif

    return 1;                     
}

/**
 * @brief TimeStamp 记录时间戳
 *
 */
void IMU_Solution::TimeStamp(void)
{
    *(IMU_Buff.Write_Time_Buff + IMU_Buff.Write_Frame) = Get_TimeStamp();
}

/**
 * @brief Put_into_Buff 将采集出来的IMU数据存入相应缓冲区
 *
 * @param float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat
 * @return int8_t -1 存入Buff失败      1 存入Buff成功
 */
int8_t IMU_Solution::Put_into_Buff(float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat)
{
    uint8_t i = 0;
    /* 判断写缓冲区状态是否异常 */
    if(IMU_Buff.Read_AccBuff == NULL || IMU_Buff.Read_GyroBuff == NULL || IMU_Buff.Read_AngleBuff == NULL || IMU_Buff.Read_MagBuff == NULL)         //写缓冲区不指向NULL
    {
        Solution_LOG("Write buffer pointer error \n\r");
        return -1;       
    } 
    /* 向缓冲区写入数据 */
    #if IMU_BUFF_MUTEX_ENABLE > 0u
        if(IMU_Buff_Mutex != NULL)      //IMU缓冲区修改互斥锁不为空
        {
            if(xSemaphoreTake(IMU_Buff_Mutex, portMAX_DELAY) == pdTRUE)       //申请IMU缓冲区修改互斥锁
            {
    #endif     
                for(i=0; i<3; i++)
                {
                    *(IMU_Buff.Read_AccBuff + IMU_Buff.Write_Frame*3 + i) = acc[i];
                    *(IMU_Buff.Read_GyroBuff + IMU_Buff.Write_Frame*3 + i) = gyro[i];
                    *(IMU_Buff.Read_AngleBuff + IMU_Buff.Write_Frame*3 + i) = angle[i];
                    *(IMU_Buff.Read_MagBuff + IMU_Buff.Write_Frame*3 + i) = mag[i];
                }
                *(IMU_Buff.Read_AtmoBuff + IMU_Buff.Write_Frame) = atom; 
                *(IMU_Buff.Read_HightBuff + IMU_Buff.Write_Frame) = hight;
                for(i=0; i<4; i++)
                {
                    *(IMU_Buff.Read_QuatBuff + IMU_Buff.Write_Frame*4 + i) = quat[i];
                }                
                IMU_Buff.Write_Frame++;                
    #if IMU_BUFF_MUTEX_ENABLE > 0u
                xSemaphoreGive(IMU_Buff_Mutex);           //释放IMU缓冲区修改互斥锁
            }
            else
            {
                Solution_LOG("Application for mutex failed \n\r");      //申请IMU缓冲区修改互斥锁失败
                return -1;
            }
        }
        else
        {
            Solution_LOG("The mutex is not created \n\r");      //IMU缓冲区修改互斥锁未创建
            return -1;
        }            
    #endif

    return 1;                     
}

/* 蓝牙传输帧数据存储空间 */
uint8_t Plantar_Transfer[4*SENSOR_NUM_TOTAL];
uint8_t Plantar_TimeStamp_Transfer[4];
uint8_t IMU_TimeStamp_Transfer[4];
uint8_t Accelerated_Transfer[12];
uint8_t Gyroscope_Transfer[12];
uint8_t Angle_Transfer[12];
uint8_t Magnetic_Transfer[6];
uint8_t Atmospheric_Transfer[4];
uint8_t Height_Transfer[4];
uint8_t Quaternion_Transfer[16];

/**
 * @brief Plantar_Data_Frame_Handling 压力传感器数据帧处理
 * @param uint8_t frame  需要发送是缓冲区中第几帧
 */
void Plantar_Data_Frame_Handling(uint8_t frame)
{
    uint8_t i;

    /* 时间戳处理 */
    u32_u8Arry(*(Plantar_Buff.Read_Time_Buff + frame), Plantar_TimeStamp_Transfer);   
    /* 压力传感器数据处理 */
    for(i=0; i<SENSOR_NUM_TOTAL; i++)
    {
        float_u8Arry(Plantar_Transfer + 4*i, Pressure_Buff + frame*SENSOR_NUM_TOTAL + i, false);
    }
}

/**
 * @brief IMU_Data_Frame_Handling IMU数据帧处理
 * @param uint8_t frame  需要发送是缓冲区中第几帧
 */
void IMU_Data_Frame_Handling(uint8_t frame)
{
    uint8_t i;

    /* 时间戳处理 */
    u32_u8Arry(*(IMU_Buff.Read_Time_Buff + frame), IMU_TimeStamp_Transfer); 
    /* IMU数据处理 */
    for(i=0; i<3; i++)
    {
        float_u8Arry(Accelerated_Transfer + 4*i, IMU_Buff.Read_AccBuff + frame*3 + i, false);
        float_u8Arry(Gyroscope_Transfer + 4*i, IMU_Buff.Read_GyroBuff + frame*3 + i, false);
        float_u8Arry(Angle_Transfer + 4*i, IMU_Buff.Read_AngleBuff + frame*3 + i, false);
        u16_u8Arry(*(IMU_Buff.Read_MagBuff + frame*3 + i), Magnetic_Transfer + 2*i);        
    }
    u32_u8Arry(*(IMU_Buff.Read_AtmoBuff + frame), Atmospheric_Transfer);
    u32_u8Arry(*(IMU_Buff.Read_HightBuff + frame), Height_Transfer);
    for(i=0; i<4; i++)
    {
        float_u8Arry(Quaternion_Transfer + 4*i, IMU_Buff.Read_QuatBuff + frame*4 + i, false);        
    }    
}


/* 蓝牙传输数据帧缓冲区 */
uint8_t Plantar_Transfer_Buffer[4*SENSOR_NUM_TOTAL + 15];
uint8_t Acc_Transfer_Buffer[27];
uint8_t Gyro_Transfer_Buffer[27];
uint8_t Angle_Transfer_Buffer[27];
uint8_t Mag_Transfer_Buffer[21];
uint8_t Atmo_Transfer_Buffer[19];
uint8_t Hight_Transfer_Buffer[19];
uint8_t Quat_Transfer_Buffer[31];

/**
 * @brief Plantar_Data_Frame_Transmit 发送压力传感器数据帧
 * 
 */
void Plantar_Data_Frame_Transmit(void)
{
    uint16_t i;
 
    /* 压力传感器数据传输 */
    for(i=0; i<5; i++)
    {
        Plantar_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Plantar_Transfer_Buffer[5] = 0x40;              //数据类型
    for(i=0; i<4; i++)
    {
        Plantar_Transfer_Buffer[6+i] = Plantar_TimeStamp_Transfer[i];           //时间戳
    }
    for(i=0; i<4*SENSOR_NUM_TOTAL; i++)
    {
        Plantar_Transfer_Buffer[10+i] = Plantar_Transfer[i];            //数据位
    }
    for(i=0; i<5; i++)
    {
        Plantar_Transfer_Buffer[4*SENSOR_NUM_TOTAL + 10 +i] = 0x68;         //协议尾
    }
    SerialBT.write(Plantar_Transfer_Buffer,sizeof(Plantar_Transfer_Buffer));
}

/**
 * @brief IMU_Data_Frame_Transmit 发送IMU数据帧
 * 
 */
void IMU_Data_Frame_Transmit(void)
{
    uint8_t i;
    
    /* 加速度数据传输 */
    for(i=0; i<5; i++)
    {
        Acc_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Acc_Transfer_Buffer[5] = 0x50;              //数据类型
    for(i=0; i<4; i++)
    {
        Acc_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];           //时间戳
    }
    for(i=0; i<12; i++)
    {
        Acc_Transfer_Buffer[10 + i] = Accelerated_Transfer[i];        //数据位
    }
    for(i=0; i<5; i++)
    {
        Acc_Transfer_Buffer[22 + i] = 0x68;         //协议尾
    }
    SerialBT.write(Acc_Transfer_Buffer, sizeof(Acc_Transfer_Buffer));
    /* 陀螺仪数据传输 */
    for(i=0; i<5; i++)
    {
        Gyro_Transfer_Buffer[i] = 0x71;         //协议头
    }
    Gyro_Transfer_Buffer[5] = 0x51;             //数据类型
    for(i=0; i<4; i++)
    {
        Gyro_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];            //时间戳
    }
    for(i=0; i<12; i++)
    {
        Gyro_Transfer_Buffer[10 + i] = Gyroscope_Transfer[i];           //数据位
    }
    for(i=0; i<5; i++)
    {
        Gyro_Transfer_Buffer[22 + i] = 0x68;        //协议尾
    }
    SerialBT.write(Gyro_Transfer_Buffer, sizeof(Gyro_Transfer_Buffer));
    /* 角度数据传输 */
    for(i=0; i<5; i++)
    {
        Angle_Transfer_Buffer[i] = 0x71;        //协议头
    }
    Angle_Transfer_Buffer[5] = 0x52;            //数据类型
    for(i=0; i<4; i++)
    {
        Angle_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];           //时间戳
    }
    for(i=0; i<12; i++)
    {
        Angle_Transfer_Buffer[10 + i] = Angle_Transfer[i];              //数据位
    }
    for(i=0; i<5; i++)
    {
        Angle_Transfer_Buffer[22 + i] = 0x68;        //协议尾
    }
    SerialBT.write(Angle_Transfer_Buffer, sizeof(Angle_Transfer_Buffer));
    /* 磁力计数据传输 */
    for(i=0; i<5; i++)
    {
        Mag_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Mag_Transfer_Buffer[5] = 0x53;              //数据类型
    for(i=0; i<4; i++)
    {
        Mag_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
    }
    for(i=0; i<6; i++)
    {
        Mag_Transfer_Buffer[10 + i] = Magnetic_Transfer[i];             //数据位
    }
    for(i=0; i<5; i++)
    {
        Mag_Transfer_Buffer[16 + i] = 0x68;         //协议尾
    }
    SerialBT.write(Mag_Transfer_Buffer, sizeof(Mag_Transfer_Buffer));
    /* 气压计数据传输 */
    for(i=0; i<5; i++)
    {
        Atmo_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Atmo_Transfer_Buffer[5] = 0x54;              //数据类型
    for(i=0; i<4; i++)
    {
        Atmo_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
    }
    for(i=0; i<4; i++)
    {
        Atmo_Transfer_Buffer[10 + i] = Atmospheric_Transfer[i];             //数据位
    }
    for(i=0; i<5; i++)
    {
        Atmo_Transfer_Buffer[14 + i] = 0x68;         //协议尾
    }
    SerialBT.write(Atmo_Transfer_Buffer, sizeof(Atmo_Transfer_Buffer));
    /* 高度数据传输 */
    for(i=0; i<5; i++)
    {
        Hight_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Hight_Transfer_Buffer[5] = 0x55;              //数据类型
    for(i=0; i<4; i++)
    {
        Hight_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
    }
    for(i=0; i<4; i++)
    {
        Hight_Transfer_Buffer[10 + i] = Height_Transfer[i];             //数据位
    }
    for(i=0; i<5; i++)
    {
        Hight_Transfer_Buffer[14 + i] = 0x68;         //协议尾
    }
    SerialBT.write(Hight_Transfer_Buffer, sizeof(Hight_Transfer_Buffer));
    /* 四元数数据传输 */
    for(i=0; i<5; i++)
    {
        Quat_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Quat_Transfer_Buffer[5] = 0x56;              //数据类型
    for(i=0; i<4; i++)
    {
        Quat_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
    }
    for(i=0; i<16; i++)
    {
        Quat_Transfer_Buffer[10 + i] = Quaternion_Transfer[i];             //数据位
    }
    for(i=0; i<5; i++)
    {
        Quat_Transfer_Buffer[26 + i] = 0x68;         //协议尾
    }
    SerialBT.write(Quat_Transfer_Buffer, sizeof(Quat_Transfer_Buffer));
}

