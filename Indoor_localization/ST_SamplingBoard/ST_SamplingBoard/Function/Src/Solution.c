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

/*
*********************************************************************************************************
*	                                      变量声明
*********************************************************************************************************
*/
/* RTOS相关变量 */
extern osMutexId_t Transfer_MutexHandle;        /* SPP发送互斥锁 */

/* 压力数据缓存区 */
extern Plantar_Buff_TypeDef Plantar_Buff;              //压力传感器阵列Buff结构体声明
extern IMU_Buff_TypeDef IMU_Buff;                      //惯性传感器Buff结构体声明
float Pressure_Buff[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];

/* 采集板压力计算参数 */
const float para_a = 624.1;      /* 函数形式: y = a*exp(-((b*x)/c)^2) */
const float para_b = 0.6779;
const float para_c = 0.2652;
const float vref = 0.58;
const float Rf = 4700; 

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

/*
*********************************************************************************************************
*	                                      函数定义
*********************************************************************************************************
*/

/**
 * @brief Singel_Point_Calculation  单采样点解算得到压力值
 * @param uint32_t *voltage
 */
float Singel_Point_Calculation(uint16_t *voltage)
{
    float vol;
    float conductance;
    float pressure;
    vol = (float)(*voltage)/1000.0f;       
    conductance = ((vol/vref)-1)/Rf;
    float index = (conductance*1000 - para_b)/para_c;
    pressure = para_a*exp(-index*index);
    Solution_LOG("Conductance :%f \n\r",conductance);     //输出电导值
    Solution_LOG("Pressure :%f g \n\r",pressure);     //输出压力值

    return pressure;
}

/**
 * @brief Conductance_Calculation  单采样点解算得到电导值
 * @brief uint32_t *voltage  测量出的电压值
 * @return float 返回电导值
 */
float Conductance_Calculation(uint16_t *voltage)
{
    float vol;
    float conductance;
    /* 计算电导值 */
    vol = (float)(*voltage)/1000.0f;
    conductance = ((vol/vref)-1)/Rf;;
    Solution_LOG("Conductance :%f \n\r",conductance);     //输出电导值

    return conductance;        
}

/**
 * @brief Pressure_Calculation  对缓冲区中的数据进行解算，得到压力值
 * @param float *Buff
 * @return int8_t -1 解算失败      1 解算成功
 */
int8_t Pressure_Calculation(float *buff)
{
    float voltage;
    float conductance;
    uint16_t i;
    
    /* 判断读缓冲区状态是否异常 */
    if(Plantar_Buff.Read_Buff == NULL)         //读缓冲区不指向NULL
    {
        Solution_LOG("Read buffer pointer error \n\r");
        return RET_ERROR;       
    }
    else
    {
        /* 解算压力值 */
        for(i=0; i<SENSOR_NUM_TOTAL*FRAME_IN_BUFF; i++)
        {
            voltage = (float)(*(Plantar_Buff.Read_Buff + i))/1000.0f;       
            conductance = ((voltage/vref)-1)/Rf;;
            float index = (conductance*1000 - para_b)/para_c;
            *(buff + i) = para_a*exp(-index*index);
        }
        return RET_OK; 
    }       
}

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
 * @brief Plantar_Data_Frame_Init 压力传感器数据帧初始化
 * 
 */
void Plantar_Data_Frame_Init(void)
{
    uint8_t i;
    
    /* 压力传感器数据传输 */
    for(i=0; i<5; i++)
    {
        Plantar_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Plantar_Transfer_Buffer[5] = 0x40;              //数据类型
    for(i=0; i<5; i++)
    {
        Plantar_Transfer_Buffer[4*SENSOR_NUM_TOTAL + 10 +i] = 0x68;         //协议尾
    }
}

/**
 * @brief Plantar_Data_Frame_Transmit 发送压力传感器数据帧
 * 
 */
void Plantar_Data_Frame_Transmit(void)
{
    uint16_t i;
    if(Transfer_MutexHandle != NULL)      //Plantar缓冲区修改互斥锁不为空
    {
        if(xSemaphoreTake(Transfer_MutexHandle, pdMS_TO_TICKS(20)) == pdTRUE)       //申请SPP发送互斥锁
        {
            /* 压力传感器数据传输 */
            for(i=0; i<4; i++)
            {
                Plantar_Transfer_Buffer[6+i] = Plantar_TimeStamp_Transfer[i];           //时间戳
            }
            for(i=0; i<4*SENSOR_NUM_TOTAL; i++)
            {
                Plantar_Transfer_Buffer[10+i] = Plantar_Transfer[i];            //数据位
            }
            comSendBuf(SPP_COM, Plantar_Transfer_Buffer, sizeof(Plantar_Transfer_Buffer));
            xSemaphoreGive(Transfer_MutexHandle);           //释放SPP发送互斥锁
        }
        else
        {
           TASK_LOG("Application for mutex failed \n\r");      //申请SPP发送互斥锁失败
        }
    }
    else
    {
        TASK_LOG("The mutex not created \n\r");      //SPP发送互斥锁未创建
    }
}



/**
 * @brief IMU_Data_Frame_Init IMU数据帧初始化
 * 
 */
void IMU_Data_Frame_Init(void)
{
    uint8_t i;
    
    /* 加速度数据传输 */
    for(i=0; i<5; i++)
    {
        Acc_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Acc_Transfer_Buffer[5] = 0x50;              //数据类型
    for(i=0; i<5; i++)
    {
        Acc_Transfer_Buffer[22 + i] = 0x68;         //协议尾
    }
    /* 陀螺仪数据传输 */
    for(i=0; i<5; i++)
    {
        Gyro_Transfer_Buffer[i] = 0x71;         //协议头
    }
    Gyro_Transfer_Buffer[5] = 0x51;             //数据类型
    for(i=0; i<5; i++)
    {
        Gyro_Transfer_Buffer[22 + i] = 0x68;        //协议尾
    }
    /* 角度数据传输 */
    for(i=0; i<5; i++)
    {
        Angle_Transfer_Buffer[i] = 0x71;        //协议头
    }
    Angle_Transfer_Buffer[5] = 0x52;            //数据类型
    for(i=0; i<5; i++)
    {
        Angle_Transfer_Buffer[22 + i] = 0x68;        //协议尾
    }
    /* 磁力计数据传输 */
    for(i=0; i<5; i++)
    {
        Mag_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Mag_Transfer_Buffer[5] = 0x53;              //数据类型
    for(i=0; i<5; i++)
    {
        Mag_Transfer_Buffer[16 + i] = 0x68;         //协议尾
    }
    /* 气压计数据传输 */
    for(i=0; i<5; i++)
    {
        Atmo_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Atmo_Transfer_Buffer[5] = 0x54;              //数据类型
    for(i=0; i<5; i++)
    {
        Atmo_Transfer_Buffer[14 + i] = 0x68;         //协议尾
    }
    /* 高度数据传输 */
    for(i=0; i<5; i++)
    {
        Hight_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Hight_Transfer_Buffer[5] = 0x55;              //数据类型
    for(i=0; i<5; i++)
    {
        Hight_Transfer_Buffer[14 + i] = 0x68;         //协议尾
    }
    /* 四元数数据传输 */
    for(i=0; i<5; i++)
    {
        Quat_Transfer_Buffer[i] = 0x71;          //协议头
    }
    Quat_Transfer_Buffer[5] = 0x56;              //数据类型
    for(i=0; i<5; i++)
    {
        Quat_Transfer_Buffer[26 + i] = 0x68;         //协议尾
    }
}

extern UART_HandleTypeDef huart6;

/**
 * @brief IMU_Data_Frame_Transmit 发送IMU数据帧
 * 
 */
void IMU_Data_Frame_Transmit(void)
{
    uint8_t i;
    
    if(Transfer_MutexHandle != NULL)      //Plantar缓冲区修改互斥锁不为空
    {
        if(xSemaphoreTake(Transfer_MutexHandle, pdMS_TO_TICKS(20)) == pdTRUE)       //申请SPP发送互斥锁
        {
            /* 加速度数据传输 */
            for(i=0; i<4; i++)
            {
                Acc_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];           //时间戳
            }
            for(i=0; i<12; i++)
            {
                Acc_Transfer_Buffer[10 + i] = Accelerated_Transfer[i];        //数据位
            }
            comSendBuf(SPP_COM, Acc_Transfer_Buffer, sizeof(Acc_Transfer_Buffer));
            /* 陀螺仪数据传输 */
            for(i=0; i<4; i++)
            {
                Gyro_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];            //时间戳
            }
            for(i=0; i<12; i++)
            {
                Gyro_Transfer_Buffer[10 + i] = Gyroscope_Transfer[i];           //数据位
            }
            comSendBuf(SPP_COM, Gyro_Transfer_Buffer, sizeof(Gyro_Transfer_Buffer));
            /* 角度数据传输 */
            for(i=0; i<4; i++)
            {
                Angle_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];           //时间戳
            }
            for(i=0; i<12; i++)
            {
                Angle_Transfer_Buffer[10 + i] = Angle_Transfer[i];              //数据位
            }
            comSendBuf(SPP_COM, Angle_Transfer_Buffer, sizeof(Angle_Transfer_Buffer));
            /* 磁力计数据传输 */
            for(i=0; i<4; i++)
            {
                Mag_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
            }
            for(i=0; i<6; i++)
            {
                Mag_Transfer_Buffer[10 + i] = Magnetic_Transfer[i];             //数据位
            }
            comSendBuf(SPP_COM, Mag_Transfer_Buffer, sizeof(Mag_Transfer_Buffer));
            /* 气压计数据传输 */
            for(i=0; i<4; i++)
            {
                Atmo_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
            }
            for(i=0; i<4; i++)
            {
                Atmo_Transfer_Buffer[10 + i] = Atmospheric_Transfer[i];             //数据位
            }
            comSendBuf(SPP_COM, Atmo_Transfer_Buffer, sizeof(Atmo_Transfer_Buffer));
            /* 高度数据传输 */
            for(i=0; i<4; i++)
            {
                Hight_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
            }
            for(i=0; i<4; i++)
            {
                Hight_Transfer_Buffer[10 + i] = Height_Transfer[i];             //数据位
            }
            comSendBuf(SPP_COM, Hight_Transfer_Buffer, sizeof(Hight_Transfer_Buffer));
            /* 四元数数据传输 */
            for(i=0; i<4; i++)
            {
                Quat_Transfer_Buffer[6 + i] = IMU_TimeStamp_Transfer[i];              //时间戳
            }
            for(i=0; i<16; i++)
            {
                Quat_Transfer_Buffer[10 + i] = Quaternion_Transfer[i];             //数据位
            }
            comSendBuf(SPP_COM, Quat_Transfer_Buffer, sizeof(Quat_Transfer_Buffer));
            xSemaphoreGive(Transfer_MutexHandle);           //释放SPP发送互斥锁
        }
        else
        {
           TASK_LOG("Application for mutex failed \n\r");      //申请SPP发送互斥锁失败
        }
    }
    else
    {
        TASK_LOG("The mutex not created \n\r");      //SPP发送互斥锁未创建
    }

}


