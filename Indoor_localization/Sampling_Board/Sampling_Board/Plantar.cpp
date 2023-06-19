/*
*********************************************************************************************************
*
*	文件名称 : Plantar.cpp
*	版    本 : V1.0
*	说    明 : 这是室内定位项目压力传感器阵列的采集程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-10     Qiuqiu&Meipo    首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "Plantar.h"

/*
*********************************************************************************************************
*	                                      变量声明
*********************************************************************************************************
*/
/* 全局变量修改互斥锁 */
extern SemaphoreHandle_t Global_Variable_Mutex;
/* ADC采样数据 */
extern uint16_t adc_raw[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN];       //ADC读出原始数据存放
extern uint32_t voltage[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN];       //ADC读出转化后数据存放
/* 单点采样ADC选通行列 */
extern uint8_t Selection_Line;      //压力传感器阵列选通行
extern uint8_t Selection_Column;    //压力传感器阵列选通列
/* 压力阵列传感器采样模式 */
extern uint8_t Plantar_Sampling_Mode;
/* 采样间延时时长 */
extern uint16_t Plantar_Sampling_Delay;
extern const uint16_t Plantar_Rate_Settings[10];
/* 压力传感器阵列行列数定义 */
extern const uint8_t Sensor_Num_Line = SENSOR_NUM_LINE;       //压力传感器阵列行数
extern const uint8_t Sensor_Num_Column[SENSOR_NUM_LINE][2] = { {0,2}, {0,3}, {0,3}, {0,3}, {0,3}, {0,4}, {0,5}, {0,5}, {1,5}, {2,5} };      //压力传感器阵列列数
/*
*********************************************************************************************************
*	                                      函数声明
*********************************************************************************************************
*/
#define NOP do { __asm__ __volatile__ ("nop"); } while (0)

/**
 * @brief nop_delay
 */
void inline nop_delay(uint8_t num)
{
    while(num)
    {
        NOP;
        num--;
    }
}

/**
 * @brief Plantar
 */
Plantar::Plantar()
{
    PLANTAR_LOG("Plantar Object Created \n\r");
}

/**
 * @brief ~Plantar
 */
Plantar::~Plantar()
{
    PLANTAR_LOG("Plantar Object Deleted \n\r");
}

/**
 * @brief 压力传感器阵列初始化函数
 */
void Plantar::Init(void) 
{
    /* 初始化选通控制GPIO */
    pinMode(GATE_C0, OUTPUT);
    pinMode(GATE_C1, OUTPUT);
    pinMode(GATE_C2, OUTPUT);
    pinMode(GATE_R0, OUTPUT);
    pinMode(GATE_R1, OUTPUT);
    pinMode(GATE_R2, OUTPUT);
    pinMode(GATE_R3, OUTPUT);
    pinMode(GATE_R4, OUTPUT);
    pinMode(GATE_R5, OUTPUT);
    pinMode(EN_INH1, OUTPUT);
    pinMode(EN_INH2, OUTPUT);
    pinMode(EN_INH3, OUTPUT);
    digitalWrite(EN_INH1, 0);  //高电平-禁用选通器
    digitalWrite(EN_INH2, 0);  //高电平-禁用选通器
    digitalWrite(EN_INH3, 0);  //高电平-禁用选通器
    /* 初始化ADC */
    analogReadResolution(12);
    if(!adcAttachPin(AOUT))         //将ADC连接至AOUT引脚
    {
        PLANTAR_LOG("ADC init failure \n\r");       //ADC初始化失败
    }
    analogSetPinAttenuation(AOUT, ADC_ATTENDB_MAX);
}

/**
 * @brief Channel_Judge 判断压力传感器阵列选通项是否正确
 * 
 * @param uint8_t Line 需要选通的行
 * @param uint8_t Column 需要选通的列
 * @return int8_t -1 输入选通行列错误      1 输入选通行列正确
 */
int8_t Plantar::Channel_Judge(uint8_t Line, uint8_t Column)
{
    if(Line >= Sensor_Num_Line || Line < 0)     //判断输入行数据是否正确
    {
        PLANTAR_LOG("Channel select error \n\r");       //输入行数据错误
        return -1;
    }
    if(Column < Sensor_Num_Column[Line][0] || Column > Sensor_Num_Column[Line][1])      //判断输入列数据是否正确
    {
        PLANTAR_LOG("Channel select error \n\r");       //输入列数据错误
        return -1;
    }
    else
    {
        return 1;
    }     
}

/**
 * @brief Plantar_Channel_Change 改变压力传感器阵列选通项
 * 
 * @param uint8_t Line 需要选通的行
 * @param uint8_t Column 需要选通的列
 * @return int8_t -1 修改选通项失败      1 修改选通项成果
 */
int8_t Plantar::Channel_Change(uint8_t Line, uint8_t Column)
{
    #if GLOBAL_VARIABLE_MUTEX_ENABLE > 0u
        if(Global_Variable_Mutex != NULL)      //修改全局变量互斥锁不为空
        {
            if(xSemaphoreTake(Global_Variable_Mutex, portMAX_DELAY) == pdTRUE)       //申请修改全局变量互斥锁
            {
    #endif
                Selection_Line = Line;
                Selection_Column = Column;
                PLANTAR_LOG("Change the selection Line:%d Column:%d \n\r", Selection_Line, Selection_Column);
    #if GLOBAL_VARIABLE_MUTEX_ENABLE > 0u
                xSemaphoreGive(Global_Variable_Mutex);           //释放修改选通项互斥锁
            }
            else
            {
                return -1;
                PLANTAR_LOG("Application for mutex failed \n\r");                        
            }
        }
        else
        {
            PLANTAR_LOG("The mutex is not created \n\r");
            return -1; 
        }
    #endif
    
    return 1;
}


/**
 * @brief Channel_Selection 压力传感器阵列选通函数
 * 
 * @param uint8_t Line 需要选通的行
 * @param uint8_t Column 需要选通的列
 */
void Plantar::Channel_Selection(uint8_t Line, uint8_t Column)
{
    uint8_t Column_Level[3];
    uint8_t Line_Level[3];
    /* 配置行选通 */
    if (Line < 5)          //选通项在前五行中
    {
        Line_Level[0] = (Line & 0b001) >> 0;        //设置R0 R1 R2
        Line_Level[1] = (Line & 0b010) >> 1;
        Line_Level[2] = (Line & 0b100) >> 2;
        digitalWrite(GATE_R0, Line_Level[0]);
        digitalWrite(GATE_R1, Line_Level[1]);
        digitalWrite(GATE_R2, Line_Level[2]);       
        digitalWrite(EN_INH2, 1);  //低电平-使能选通器
        digitalWrite(EN_INH3, 0);  //高电平-禁用选通器
        //PLANTAR_LOG("Line Selection:%d\n\r", digitalRead(GATE_R2)*4 + digitalRead(GATE_R1)*2 + digitalRead(GATE_R0) );
    } 
    else                //选通项在后五行中
    {
        Line -= 5;
        Line_Level[0] = (Line & 0b001) >> 0;        //设置R3 R4 R5
        Line_Level[1] = (Line & 0b010) >> 1;
        Line_Level[2] = (Line & 0b100) >> 2;
        digitalWrite(GATE_R3, Line_Level[0]);
        digitalWrite(GATE_R4, Line_Level[1]);
        digitalWrite(GATE_R5, Line_Level[2]);
        digitalWrite(EN_INH3, 1);  //低电平-使能选通器
        digitalWrite(EN_INH2, 0);  //高电平-禁用选通器
        //PLANTAR_LOG("Line Selection:%d\n\r", digitalRead(GATE_R5)*4 + digitalRead(GATE_R4)*2 + digitalRead(GATE_R3) + 5);
    }
    /* 配置列选通 */
    Column_Level[0] = (Column & 0b001) >> 0;        //设置C0 C1 C2
    Column_Level[1] = (Column & 0b010) >> 1;
    Column_Level[2] = (Column & 0b100) >> 2;
    digitalWrite(GATE_C0, Column_Level[0]);
    digitalWrite(GATE_C1, Column_Level[1]);
    digitalWrite(GATE_C2, Column_Level[2]);
    digitalWrite(EN_INH1, 1);  //低电平-使能选通器
    //PLANTAR_LOG("Column Selection:%d\n\r", digitalRead(GATE_C2)*4 + digitalRead(GATE_C1)*2 + digitalRead(GATE_C0));
}

/**
 * @brief Channel_Disable 压力传感器阵列禁用
 * 
 */
void Plantar::Channel_Disable(void)
{
    digitalWrite(EN_INH1, 0);       //高电平-禁用选通器
    digitalWrite(EN_INH2, 0);       //高电平-禁用选通器
    digitalWrite(EN_INH3, 0);       //高电平-禁用选通器    
}

/**
 * @brief Sampling_Mode_Change 修改压力传感器阵列采集模式
 *
 * @param uint8_t Mode 压力传感器阵列采集模式
 * @return int8_t -1 无效修改模式      1 修改采集模式成功
 */
int8_t Plantar::Sampling_Mode_Change(uint8_t Mode)
{
    char Print_buff[30];
    #if GLOBAL_VARIABLE_MUTEX_ENABLE > 0u
        if(Global_Variable_Mutex != NULL)      //修改全局变量互斥锁不为空
        {
            if(xSemaphoreTake(Global_Variable_Mutex, portMAX_DELAY) == pdTRUE)       //申请修改全局变量互斥锁
            {
    #endif
                Plantar_Sampling_Mode = Mode;
                if(Plantar_Sampling_Mode == 0)  sprintf(Print_buff,"Array Samplingmode");
                else if(Plantar_Sampling_Mode == 1)  sprintf(Print_buff,"Singlepoint Samplingmode");
                else if(Plantar_Sampling_Mode == 2)  sprintf(Print_buff,"Instructions Samplingmode");                       
                PLANTAR_LOG("Sampling Mode:%s \n\r", Print_buff);
    #if GLOBAL_VARIABLE_MUTEX_ENABLE > 0u
                xSemaphoreGive(Global_Variable_Mutex);           //释放修改选通项互斥锁
            }
            else
            {
                return -1;
                PLANTAR_LOG("Application for mutex failed \n\r");                        
            }
        }
        else
        {
            PLANTAR_LOG("The mutex is not created \n\r");
            return -1; 
        }
    #endif

    return -1;
}

/**
 * @brief Sampling_Rate_Change 修改压力传感器阵列采集延时
 *
 * @param uint8_t Rate 压力传感器阵列采集延时
 * @return int8_t -1 修改延时失败      1 修改延时成功
 */
int8_t Plantar::Sampling_Rate_Change(uint8_t Rate)
{
    if(Rate > 9)
    {
        PLANTAR_LOG("Invalid input rate value \n\r");
        return -1;         
    }    
    #if GLOBAL_VARIABLE_MUTEX_ENABLE > 0u
        if(Global_Variable_Mutex != NULL)      //修改全局变量互斥锁不为空
        {
            if(xSemaphoreTake(Global_Variable_Mutex, portMAX_DELAY) == pdTRUE)       //申请修改全局变量互斥锁
            {
    #endif
                Plantar_Sampling_Delay = Plantar_Rate_Settings[Rate];
                PLANTAR_LOG("Sampling delay time:%d \n\r", Plantar_Sampling_Delay);
    #if GLOBAL_VARIABLE_MUTEX_ENABLE > 0u
                xSemaphoreGive(Global_Variable_Mutex);           //释放修改选通项互斥锁
            }
            else
            {
                return -1;
                PLANTAR_LOG("Application for mutex failed \n\r");                        
            }
        }
        else
        {
            PLANTAR_LOG("The mutex is not created \n\r");
            return -1; 
        }
    #endif

    return -1;
}


/**
 * @brief ADC_Oneshot_Read ADC单次采集,并可生成原始数据和标定校准过的电压数据
 *
 * @param uint16_t adc_raw adc采集原始数据
 * @param uint16_t voltage 转化后的电压值
 */
void Plantar::ADC_Oneshot_Read(uint16_t *adc_raw, uint32_t *voltage)
{
    #if 0
        *adc_raw = analogRead(AOUT);
        PLANTAR_LOG("ADC analog value = %d\n\r", *adc_raw);         //读取ADC原始数据
    #endif
    *voltage = analogReadMilliVolts(AOUT);
    PLANTAR_LOG("ADC millivolts value = %d mV\n\r", *voltage);      //读取ADC转化后数据 单位：mV
    //Channel_Disable();      //禁用所有通道
}

/**
 * @brief Single_Frame_Read 测量单帧阵列数据
 *
 * @param uint16_t raw_data 数据采集后的保存的原始数据二维数组
 * @param uint16_t voltage_data 转化为电压后的二维数组
 */
void Plantar::Single_Frame_Read(uint16_t raw_data_p[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN],uint32_t voltage_p[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN])
{
    uint8_t i = 0,j = 0;
    while(i < Sensor_Num_Line)          //从第一行开始按顺序采集
    {
        j = Sensor_Num_Column[i][0];
        while(j <= Sensor_Num_Column[i][1])      //采集一行时从第一列开始顺序采集
        {
            Channel_Selection(i,j);         //选通对应行列
            nop_delay(20);       //模拟信号建立时间
            ADC_Oneshot_Read(&raw_data_p[i][j],&voltage_p[i][j]);           //读取选通项电压值
            j++;
        }
        i++;
    }
}
