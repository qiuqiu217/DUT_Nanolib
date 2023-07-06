/*
*********************************************************************************************************
*
*	文件名称 : Sampling_Board.ino
*	版    本 : V1.0
*	说    明 : 这是室内定位项目采集板的的主文件，主控采用ESP32。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-03-10     Qiuqiu&Meipo    首次发布
*       V1.1    2023-04-11     Qiuqiu&Meipo    第一版修改
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "includes.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
    #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
    #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

/*
*********************************************************************************************************
*	                                        任务相关宏定义
*********************************************************************************************************
*/
/* 启动任务宏定义 */
#define APP_TASK_START_PRIO                         15u
#define APP_TASK_START_STK_SIZE                     1024u
/* 压力传感器采集任务宏定义 */
#define APP_TASK_PLANTAR_PRIO                       13u
#define APP_TASK_PLANTAR_STK_SIZE                   1024*2u
/* 数据处理任务宏定义 */
#define APP_TASK_DATA_HANDLING_PRIO                 5u
#define APP_TASK_DATA_HANDLING_STK_SIZE             1024*5u
/* 数据发送任务宏定义 */
#define APP_TASK_DATA_TRANSFER_PRIO                 10u
#define APP_TASK_DATA_TRANSFER_STK_SIZE             1024*5u
/* Debug串口任务宏定义 */
#define APP_TASK_COMMAND_PRIO                       8u
#define APP_TASK_COMMAND_STK_SIZE                   1024*4u
/* 惯性传感器任务宏定义 */
#define APP_TASK_IMU_PRIO                           14u
#define APP_TASK_IMU_STK_SIZE                       1024*4u

/*
*********************************************************************************************************
*                                           任务句柄声明
*********************************************************************************************************
*/  
static TaskHandle_t TASK_Handle_Start;              //启动任务句柄
static TaskHandle_t TASK_Handle_Plantar;            //压力传感器阵列采集任务句柄
static TaskHandle_t TASK_Handle_Data_Handling;      //数据处理任务句柄
static TaskHandle_t TASK_Handle_Data_Transfer;      //数据发送任务句柄
static TaskHandle_t TASK_Handle_Command;            //Debug命令任务句柄
static TaskHandle_t TASK_Handle_IMU;                //惯性传感器任务句柄

/*
*********************************************************************************************************
*	                                            宏定义
*********************************************************************************************************
*/
/* 启用/禁用Plantar任务debug打印 */
#if 0
	#define PLANTAR_TASK_LOG            COM_DEBUG.printf
#else
	#define PLANTAR_TASK_LOG
#endif
/* 启用/禁用IMU任务debug打印 */
#if 0
	#define IMU_TASK_LOG                COM_DEBUG.printf
#else
	#define IMU_TASK_LOG
#endif
/* 启用/禁用数据处理任务debug打印 */
#if 0
	#define DATA_HANDLING_TASK_LOG      COM_DEBUG.printf
#else
	#define DATA_HANDLING_TASK_LOG
#endif
/* 启用/禁用数据发送任务debug打印 */
#if 0
	#define DATA_TRANSFER_TASK_LOG      COM_DEBUG.printf
#else
	#define DATA_TRANSFER_TASK_LOG
#endif
/* 启用/禁用Debug命令任务debug打印 */
#if 0
	#define COMMAND_TASK_LOG            COM_DEBUG.printf
#else
	#define COMMAND_TASK_LOG
#endif
/*
*********************************************************************************************************
*                                               变量声明
*********************************************************************************************************
*/
/* -------------------------- 信号量/互斥锁/消息队列句柄 --------------------------- */
/* 全局变量修改互斥锁 */
SemaphoreHandle_t Global_Variable_Mutex = NULL;
/* Plantar缓冲区修改互斥锁 */
SemaphoreHandle_t Plantar_Buff_Mutex = NULL;
/* IMU初始化互斥锁 */
SemaphoreHandle_t IMU_Cali_Mutex = NULL;
/* IMU数据采集信号量 */
SemaphoreHandle_t IMU_Semaphore = NULL;
/* 数据处理消息队列 */
QueueHandle_t Data_Handling_Queue = NULL;
/* 数据发送消息队列 */
QueueHandle_t Data_Transmit_Queue = NULL;
/* 数据发送完成消息队列 */
QueueHandle_t Data_Done_Queue = NULL;
/* Debug命令解算消息队列 */
QueueHandle_t Debug_Queue = NULL;
/* 指令采集消息队列 */
QueueHandle_t ComSampling_Queue = NULL;
/* 传感器采集事件组 */
EventGroupHandle_t Sampling_Event = NULL;
/* 蓝牙传输事件组 */
EventGroupHandle_t Bluetooth_Event = NULL;

/* ---------------------------- 外部变量声明 --------------------------------- */
extern const uint16_t Plantar_Rate_Settings[10] = {2,5,10,20,50,100,200,500,1000,2000};      //可设置采样延时

/* ---------------------------- 全局标志位（变量） --------------------------------- */
/* IMU数据及命令标志位 */
volatile uint16_t s_cDataUpdate = 0;
volatile char s_cCmd = 0xff;
/* 压力阵列传感器采样模式 */
uint8_t Plantar_Sampling_Mode = ARRAY_SAMPLINGMODE;
/* 单点采样ADC选通行列 */
uint8_t Selection_Line = 0, Selection_Column = 0;
/* 采样间延时时长 */
uint16_t Plantar_Sampling_Delay = 12;       //压力传感器采样延时
uint16_t IMU_Sampling_Delay = 5;            //IMU采样延时

/* --------------------------------- Buffer声明 ----------------------------------- */
/* ADC采样数据数组 */
uint16_t adc_raw[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN];       //ADC读出原始数据存放
uint32_t voltage[SENSOR_NUM_LINE][SENSOR_NUM_COLUMN];       //ADC读出转化后数据存放
/* 压力数据缓存区 */
float Pressure_Buff[SENSOR_NUM_TOTAL*FRAME_IN_BUFF];
/* 串口接收缓冲区 */
String Serial_RxString = "";
/* 蓝牙接收缓冲区 */
String Bluetooth_RxString = "";
/* 蓝牙串口声明 */
BluetoothSerial SerialBT;
/*
*********************************************************************************************************
*                                              函数声明
*********************************************************************************************************
*/
/* 任务函数声明 */
static  void  AppTaskStart                  (void     *param);
static  void  AppTaskPlantar                (void     *param);
static  void  AppTaskDataHandling           (void     *param);
static  void  AppTaskDataTransfer           (void     *param);
static  void  AppTaskCommand			    (void 	  *param);
static  void  AppTaskIMU			        (void 	  *param);
/* 串口中断回调函数 */
void _cbComDebug(void);
/* 蓝牙事件回调函数 */
void _cbBluetooth(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);

/*
*********************************************************************************************************
*                                               功能函数
*********************************************************************************************************
*/
Plantar Task_Plantar;
Plantar_Solution Plantar_Solution;
IMU_Solution IMU_Solution;

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
 * @brief BSP_Init 板级初始化
 */
void BSP_Init(void)
{
    /* 串口初始化 */
    COM_DEBUG.begin(115200);                    //设置波特率
    COM_DEBUG.onReceive(_cbComDebug);           //设置回调函数
    /* 蓝牙初始化 */
    SerialBT.begin("ESP32_Slave_L");                //Bluetooth device name
    SerialBT.register_callback(_cbBluetooth);       //设置蓝牙事件回调函数（连接 断开 发送 接收）
    /* 初始化串口及蓝牙缓冲区 */
    Serial_RxString.reserve(1024);
    Bluetooth_RxString.reserve(1024);
    /* IMU初始化 */
    //IMU_Init();
    IMU_Solution.Buff_Init();          //初始化IMU采集数据缓冲区
    /* 压力传感器初始化 */
    Task_Plantar.Init();                    //压力传感器阵列采集初始化
    Plantar_Solution.Buff_Init();           //初始化传感器阵列采集数据缓冲区
}

/**
 * @brief setup 启动任务，需要启动时执行一次的任务
 */
void setup() 
{
    BSP_Init();
    delay(500);
    /* 创建全局变量修改互斥锁 */
    Global_Variable_Mutex = xSemaphoreCreateMutex();
    /* 创建Plantar缓冲区修改互斥锁 */
    Plantar_Buff_Mutex = xSemaphoreCreateMutex();
    /* 创建IMU校正互斥锁 */
    IMU_Cali_Mutex = xSemaphoreCreateMutex();
    /* 创建IMU数据采集信号量 */
    IMU_Semaphore = xSemaphoreCreateBinary();
    /* 创建数据解算消息队列 */
    Data_Handling_Queue = xQueueCreate((UBaseType_t ) 10,       /* 消息队列的长度 */ 
                                       (UBaseType_t ) 1);       /* 消息的大小 */
    /* 创建数据发送消息队列 */
    Data_Transmit_Queue = xQueueCreate((UBaseType_t ) 10,       /* 消息队列的长度 */ 
                                       (UBaseType_t ) 1);       /* 消息的大小 */ 
    /* 创建数据发送完成消息队列 */
    Data_Done_Queue = xQueueCreate((UBaseType_t ) 10,           /* 消息队列的长度 */ 
                                   (UBaseType_t ) 1);           /* 消息的大小 */                                      
    /* 创建Debug命令解算消息队列 */
    Debug_Queue = xQueueCreate((UBaseType_t ) 5,                /* 消息队列的长度 */ 
                               (UBaseType_t ) 1);               /* 消息的大小 */     
    /* 创建指令采集消息队列 */
    ComSampling_Queue = xQueueCreate((UBaseType_t ) 1,                /* 消息队列的长度 */ 
                                     (UBaseType_t ) 1);               /* 消息的大小 */
    /* 创建传感器采集事件组 */
    Sampling_Event = xEventGroupCreate();  
    /* 创建蓝牙串口传输事件组 */
    Bluetooth_Event = xEventGroupCreate();   
    /* 创建启动函数 */
    xTaskCreatePinnedToCore(AppTaskStart,                       //任务函数
                            "App Start Task",                   //任务名称
                            APP_TASK_START_STK_SIZE,            //堆栈大小
                            NULL,                               //传递参数
                            APP_TASK_START_PRIO,                //任务优先级
                            &TASK_Handle_Start,                 //任务句柄
                            tskNO_AFFINITY);                    //无关联，不绑定在任何一个核上
}

/**
 * @brief loop 循环任务，需要重复循环执行的任务
 */
void loop() 
{
    /* 空loop 任务由RTOS调度 */
}

/*
*********************************************************************************************************
*                                               任务函数
*********************************************************************************************************
*/
/**
 * @brief AppTaskStart 启动任务
 * @param *param 任务传递参数
 */
void AppTaskStart(void *param)
{ 
    /* 创建压力传感器阵列采样任务 */
    xTaskCreatePinnedToCore(AppTaskPlantar,                         //任务函数
                            "Plantar Task",                         //任务名称
                            APP_TASK_PLANTAR_STK_SIZE,              //堆栈大小
                            NULL,                                   //传递参数
                            APP_TASK_PLANTAR_PRIO,                  //任务优先级
                            &TASK_Handle_Plantar,                   //任务句柄
                            APP_CPU);                               //绑定在CPU1上

    /* 创建数据处理任务 */
    xTaskCreatePinnedToCore(AppTaskDataHandling,                    //任务函数
                            "Data Handling Task",                   //任务名称
                            APP_TASK_DATA_HANDLING_STK_SIZE,        //堆栈大小
                            NULL,                                   //传递参数
                            APP_TASK_DATA_HANDLING_PRIO,            //任务优先级
                            &TASK_Handle_Data_Handling,             //任务句柄
                            APP_CPU);                               //绑定在CPU1上
                            
    /* 创建数据发送任务 */
    xTaskCreatePinnedToCore(AppTaskDataTransfer,                    //任务函数
                            "Data Transfer Task",                   //任务名称
                            APP_TASK_DATA_TRANSFER_STK_SIZE,        //堆栈大小
                            NULL,                                   //传递参数
                            APP_TASK_DATA_TRANSFER_PRIO,            //任务优先级
                            &TASK_Handle_Data_Transfer,             //任务句柄
                            APP_CPU);                               //绑定在CPU1上

    /* 创建惯性传感器读取任务 */
    xTaskCreatePinnedToCore(AppTaskIMU,                             //任务函数
                            "IMU Task",                             //任务名称
                            APP_TASK_IMU_STK_SIZE,                  //堆栈大小
                            NULL,                                   //传递参数
                            APP_TASK_IMU_PRIO,                      //任务优先级
                            &TASK_Handle_IMU,                       //任务句柄
                            APP_CPU);                               //绑定在CPU1上                         

    /* 创建Debug命令任务 */
    xTaskCreatePinnedToCore(AppTaskCommand,                         //任务函数
                            "Command Event Task",                   //任务名称
                            APP_TASK_COMMAND_STK_SIZE,              //堆栈大小
                            NULL,                                   //传递参数
                            APP_TASK_COMMAND_PRIO,                  //任务优先级
                            &TASK_Handle_Command,                   //任务句柄
                            APP_CPU);                               //绑定在CPU1上

    /* 删除自身任务 */
    vTaskDelete(NULL);
}

extern const uint8_t Sensor_Num_Column[SENSOR_NUM_LINE][2];      //压力传感器阵列列数
/**
 * @brief AppTaskPlantar 压力传感器阵列采样任务
 * @param *param 任务传递参数
 */
void AppTaskPlantar(void *param)
{ 
    BaseType_t x_return = pdPASS;                   // 定义一个创建信息返回值，默认为 pdPASS
    uint32_t r_queue;                               // 定义一个接收消息的变量 
    uint32_t s_queue = PRESSURE_SOLUTION;           // 定义一个发送消息的变量
    EventBits_t uxBits;                             //接收到的事件组位

    while(true)
    {
        /* 判断是否开启压力传感器采集 */
        uxBits = xEventGroupWaitBits(Sampling_Event,                //等待的事件组  
                                     PLANTAR_SAMPLING,              //等待的事件位
                                     pdFALSE,                       //返回前是否清除事件
                                     pdFALSE,                       //是否等待所有事件
                                     portMAX_DELAY);                //等待时间
        if((uxBits & PLANTAR_SAMPLING) != 0)
        {
            /* 阵列循环采样 */
            if(Plantar_Sampling_Mode == ARRAY_SAMPLINGMODE)
            {
                //xSemaphoreGive(IMU_Semaphore);                  //释放IMU采集信号量，用于同步采集任务
                Plantar_Solution.TimeStamp();
                Task_Plantar.Single_Frame_Read(adc_raw,voltage);
                Plantar_Solution.Put_into_Buff(voltage);           //将采集数据放入缓冲区
                if(Plantar_Solution.Buff_Full_Judge() == 1)        //如果缓冲区已经写满
                {
                    PLANTAR_TASK_LOG("Plantar write buff full \n\r");
                    if(Plantar_Solution.Read_Write_Buff_Switch())     //交换缓冲区     
                    {
                        PLANTAR_TASK_LOG("Plantar buff switch succeed \n\r");                    
                    }
                    x_return = xQueueSend( Data_Handling_Queue,     //消息队列的句柄 
                                        &s_queue,                //发送的消息内容 
                                        0 );                     //等待时间 0                    
                }
                PLANTAR_TASK_LOG("Array Sampling Succeed \n\r");          
            }
            /* 单点循环采样 */            
            else if(Plantar_Sampling_Mode == SINGLEPOINT_SAMPLINGMODE )
            { 
                Task_Plantar.Channel_Selection(Selection_Line,Selection_Column);
                Task_Plantar.ADC_Oneshot_Read(&adc_raw[Selection_Line][Selection_Column],&voltage[Selection_Line][Selection_Column]);
                Plantar_Solution.Singel_Point_Calculation(&voltage[Selection_Line][Selection_Column]);
                PLANTAR_TASK_LOG("Single Point Sampling Succeed \n\r");
            }
            /* 接收指令采样 */
            else if(Plantar_Sampling_Mode == INSTRUCTIONS_SAMPLINGMODE)
            {
                /* 等待采样指令 */
                x_return = xQueueReceive( ComSampling_Queue,            /* 消息队列的句柄 */ 
                                          &r_queue,                     /* 发送的消息内容 */ 
                                          portMAX_DELAY);               /* 等待时间:一直等 */
                if (x_return == pdTRUE)
                {
                    if(r_queue == SINGLE_INSTRUCTION)        //如果是单点指令采样消息
                    {
                        Task_Plantar.Channel_Selection(Selection_Line,Selection_Column);
                        //nop_delay(20);       //模拟信号建立时间
                        Task_Plantar.ADC_Oneshot_Read(&adc_raw[Selection_Line][Selection_Column],&voltage[Selection_Line][Selection_Column]);
                        PLANTAR_TASK_LOG("Instructions Sampling Succeed \n\r");
                        float conductance = Plantar_Solution.Conductance_Calculation(&voltage[Selection_Line][Selection_Column]);
                        COM_DEBUG.printf("Sampling millivolts value = %ld mV\n\r", voltage[Selection_Line][Selection_Column]);      //读取ADC转化后数据 单位：mV
                        COM_DEBUG.printf("Sampling conductance = %f \n\r", conductance);
                    }
                    else if(r_queue == ARRAY_INSTRUCTION)        //如果是阵列指令采样消息
                    {
                        Task_Plantar.Single_Frame_Read(adc_raw,voltage);
                        uint8_t i = 0, j = 0;
                        float Pressure = 0;
                        while(i < SENSOR_NUM_LINE)          //从第一行开始按顺序采集
                        {
                            j = Sensor_Num_Column[i][0];
                            while(j <= Sensor_Num_Column[i][1])      //采集一行时从第一列开始顺序采集
                            {                        
                                Pressure = Pressure + Plantar_Solution.Singel_Point_Calculation(&voltage[i][j]);
                                j++;
                            }
                            i++;
                        }
                        COM_DEBUG.printf("Total pressure = %f g \n\r", Pressure);
                    }
                }
            }
            vTaskDelay(Plantar_Sampling_Delay / portTICK_PERIOD_MS);      //采集间隔设置
        }
    }
}

/**
 * @brief AppTaskIMU 惯性传感器读取任务
 * @param *param 任务传递参数
 */
void AppTaskIMU(void *param) 
{
    BaseType_t x_return = pdPASS;               // 定义一个创建信息返回值，默认为 pdPASS
    uint32_t s_queue = IMU_SOLUTION;            // 定义一个发送消息的变量
    EventBits_t uxBits;                         //接收到的事件组位
    float fAcc[3], fGyro[3], fAngle[3];
    int16_t sMag[3];
    int32_t dATMO, dHEIGHT;
    float fQUAT[4];
    int i;
     
    while(true)
    {
        /* 判断是否开启IMU采集 */
        uxBits = xEventGroupWaitBits(Sampling_Event,                //等待的事件组  
                                     IMU_SAMPLING,                  //等待的事件位
                                     pdFALSE,                       //返回前是否清除事件
                                     pdFALSE,                       //是否等待所有事件
                                     portMAX_DELAY);                //等待时间
        if((uxBits & IMU_SAMPLING) != 0)
        {
            //xSemaphoreTake(IMU_Semaphore, portMAX_DELAY);
            while (COM_IMU.available())
            {
                WitSerialDataIn(COM_IMU.read());
            }
            /* 如果有IMU数据更新 */
            if(s_cDataUpdate)
            {
                IMU_Solution.TimeStamp();
                /* 处理IMU数据 */            
                for(i = 0; i < 3; i++)
                {
                    fAcc[i] = sReg[AX+i] / 32768.0f * 16.0f * 9.8f;
                    fGyro[i] = sReg[GX+i] / 32768.0f * 2000.0f;
                    fAngle[i] = sReg[Roll+i] / 32768.0f * 180.0f;
                    sMag[i] = sReg[HX+i];
                }
                dATMO = int32_t(sReg[PressureH]<<16 | sReg[PressureL]);
                dHEIGHT = int32_t(sReg[HeightH]<<16 | sReg[HeightL]);
                for(i = 0; i < 4; i++)
                {
                    fQUAT[i] = sReg[q0+i] / 32768.0f;
                }
                if(s_cDataUpdate & ACC_UPDATE)          //打印加速度数据
                {
                    IMU_TASK_LOG("acc_x:%f acc_y:%f acc_z:%f \n\r", fAcc[0], fAcc[1], fAcc[2]);
                    s_cDataUpdate &= ~ACC_UPDATE;
                }
                if(s_cDataUpdate & GYRO_UPDATE)         //打印陀螺仪数据
                {
                    IMU_TASK_LOG("gyro_x:%f gyro_y:%f gyro_z:%f \n\r", fGyro[0], fGyro[1], fGyro[2]);
                    s_cDataUpdate &= ~GYRO_UPDATE;
                }
                if(s_cDataUpdate & ANGLE_UPDATE)        //打印倾角数据
                {
                    IMU_TASK_LOG("angle_x:%f angle_y:%f angle_z:%f \n\r", fAngle[0], fAngle[1], fAngle[2]);
                    s_cDataUpdate &= ~ANGLE_UPDATE;
                }
                if(s_cDataUpdate & MAG_UPDATE)          //打印磁力计数据
                {
                    IMU_TASK_LOG("mag_x:%d mag_y:%d mag_z:%d \n\r", sMag[0], sMag[1], sMag[2]);
                    s_cDataUpdate &= ~MAG_UPDATE;
                }
                if(s_cDataUpdate & ATMO_UPDATE)          //打印气压数据
                {
                    IMU_TASK_LOG("atmosphere pressure:%ld \n\r", dATMO);
                    s_cDataUpdate &= ~ATMO_UPDATE;
                }
                if(s_cDataUpdate & HEIGHT_UPDATE)          //打印高度数据
                {
                    IMU_TASK_LOG("height:%ld \n\r", dHEIGHT);
                    s_cDataUpdate &= ~HEIGHT_UPDATE;
                }
                if(s_cDataUpdate & QUAT_UPDATE)          //打印四元数数据
                {
                    IMU_TASK_LOG("q0:%f q1:%f q2:%f q3:%f \n\r", fQUAT[0], fQUAT[1], fQUAT[2], fQUAT[3]);
                    s_cDataUpdate &= ~QUAT_UPDATE;
                }
                s_cDataUpdate = 0;
            }
            IMU_Solution.Put_into_Buff(fAcc, fGyro, fAngle, sMag, dATMO, dHEIGHT, fQUAT);      //将IMU数据放入缓冲区
            if(IMU_Solution.Buff_Full_Judge() == 1)        //如果缓冲区已经写满
            {
                IMU_TASK_LOG("IMU write buff full \n\r");
                if(IMU_Solution.Read_Write_Buff_Switch())     //交换缓冲区     
                {
                    IMU_TASK_LOG("IMU buff switch succeed \n\r");                    
                }
                x_return = xQueueSend( Data_Handling_Queue,     //消息队列的句柄 
                                       &s_queue,                //发送的消息内容 
                                       0 );                     //等待时间 0                    
            }
            vTaskDelay(IMU_Sampling_Delay / portTICK_PERIOD_MS);      //采集间隔设置
        }
    }
}

/**
 * @brief AppTaskDataHandling 压力值解算任务
 * @param *param 任务传递参数
 */
void AppTaskDataHandling(void *param)
{
    EventBits_t uxBits;                 //接收到的事件组位
    BaseType_t x_return = pdTRUE;       // 定义一个创建信息返回值，默认为 pdTRUE 
    uint32_t r_queue;                   // 定义一个接收消息的变量 
    uint32_t s_queue;                   // 定义一个发送消息的变量
    uint8_t i;
     
    while(true)
    {
        uxBits = xEventGroupWaitBits(Bluetooth_Event,               //等待的事件组  
                                     BLUETOOTH_START_TRANSFER,      //等待的事件位
                                     pdFALSE,                       //返回前是否清除事件
                                     pdTRUE,                        //是否等待所有事件
                                     portMAX_DELAY);                //等待时间
        if((uxBits & BLUETOOTH_CONNECT) != 0)
        {
            x_return = xQueueReceive( Data_Handling_Queue,      /* 消息队列的句柄 */ 
                                      &r_queue,                 /* 发送的消息内容 */ 
                                      portMAX_DELAY);           /* 等待时间:一直等 */
            DATA_HANDLING_TASK_LOG("Data_Handling task \n\r");   
            if (x_return == pdTRUE) 
            {
                if(r_queue == PRESSURE_SOLUTION)        //如果是压力值解算消息
                {
                    if(Plantar_Solution.Pressure_Calculation(Pressure_Buff))       //如果压力值解算成功
                    {
                        DATA_HANDLING_TASK_LOG("Plantar data ready \n\r");
                        for(i=0; i<FRAME_IN_BUFF; i++)
                        {
                            Plantar_Data_Frame_Handling(i);
                            /* 告知发送任务将压力传感器数据发送 */             
                            s_queue = PRESSURE_TRANSFER;
                            x_return = xQueueSend( Data_Transmit_Queue,         //消息队列的句柄 
                                                &s_queue,                       //发送的消息内容 
                                                0 );                            //等待时间 0
                            /* 等待压力传感器数据发送完成 */
                            x_return = xQueueReceive( Data_Done_Queue,          // 消息队列的句柄 
                                                    &r_queue,                   // 发送的消息内容 
                                                    5/portTICK_PERIOD_MS);      // 等待时间:5ms                                              
                            if (x_return == pdTRUE && r_queue == PRESSURE_DONE)
                            {
                                DATA_HANDLING_TASK_LOG("no.%d plantar data transfer done \n\r",i);
                            }
                            else
                            {
                                DATA_HANDLING_TASK_LOG("no.%d plantar data transfer error \n\r",i);
                            }
                        }                        
                    }                
                }
                else if(r_queue == IMU_SOLUTION)       //如果是IMU数据消息
                {
                    DATA_HANDLING_TASK_LOG("IMU data ready \n\r");
                    for(i=0; i<FRAME_IN_BUFF; i++)
                    {
                        IMU_Data_Frame_Handling(i);
                        /* 告知发送任务将IMU数据发送 */
                        s_queue = IMU_TRANSFER;
                        x_return = xQueueSend( Data_Transmit_Queue,         //消息队列的句柄 
                                               &s_queue,                    //发送的消息内容 
                                               0 );                         //等待时间 0
                        /* 等待IMU数据发送完成 */
                        x_return = xQueueReceive( Data_Done_Queue,          // 消息队列的句柄 
                                                &r_queue,                   // 发送的消息内容 
                                                5/portTICK_PERIOD_MS);      // 等待时间:5ms
                        if (x_return == pdTRUE && r_queue == IMU_DONE)
                        {
                            DATA_HANDLING_TASK_LOG("no.%d IMU data transfer done \n\r",i);
                        }
                        else
                        {
                            DATA_HANDLING_TASK_LOG("no.%d IMU data transfer error \n\r",i);
                            //COM_DEBUG.printf("no.%d IMU data transfer error \n\r",i);
                        }                    
                    }
                }
            }
            else
            {
                DATA_HANDLING_TASK_LOG("The received message value error \n\r");
            } 
        }                                 
    }
}


/**
 * @brief AppTaskDataTransfer 压力值解算任务
 * @param *param 任务传递参数
 */
void AppTaskDataTransfer(void *param)
{
    BaseType_t xResult;                     //事件组发送结果
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;          //关闭更高优先级任务唤醒
    BaseType_t x_return = pdTRUE;           // 定义一个创建信息返回值，默认为 pdTRUE
    uint32_t r_queue;                       // 定义一个接收消息的变量
    uint32_t s_queue;                       // 定义一个发送消息的变量
    uint8_t i;
     
    while(true)
    {
        x_return = xQueueReceive( Data_Transmit_Queue,      /* 消息队列的句柄 */ 
                                  &r_queue,                 /* 发送的消息内容 */ 
                                  portMAX_DELAY);           /* 等待时间:一直等 */
        if (x_return == pdTRUE)
        {
            if(r_queue == PRESSURE_TRANSFER)        //如果是压力传感器数据传输
            {
                Plantar_Data_Frame_Transmit();
                /* 告知处理任务压力传感器数据已经传输完成 */
                s_queue = PRESSURE_DONE;
                x_return = xQueueSend(Data_Done_Queue,              //消息队列的句柄 
                                      &s_queue,                     //发送的消息内容 
                                      0 );                          //等待时间 0 
            }
            else if(r_queue == IMU_TRANSFER)        //如果是IMU数据传输
            {
                IMU_Data_Frame_Transmit();
                /* 告知处理任务IMU数据已经传输完成 */
                s_queue = IMU_DONE;
                x_return = xQueueSend(Data_Done_Queue,              //消息队列的句柄 
                                      &s_queue,                     //发送的消息内容 
                                      0 );                          //等待时间 0
            }
        }
        else
        {
            DATA_TRANSFER_TASK_LOG("The received message value error \n\r");
        }
    }
}

/**
 * @brief debug串口任务
 * @param *param 任务传递参数
 */
void AppTaskCommand(void *param)
{
    Command Task_Command;
    BaseType_t x_return = pdTRUE;       // 定义一个创建信息返回值，默认为 pdTRUE
    uint32_t r_queue;                   // 定义一个接收消息的变量 
    int8_t JSON_err;                    //JSON解析结果
    
    while(true)
    {
        /* 判断是否介绍到命令 */
        x_return = xQueueReceive( Debug_Queue,          /* 消息队列的句柄 */ 
                                  &r_queue,             /* 发送的消息内容 */ 
                                  portMAX_DELAY);       /* 等待时间:一直等 */
        if (x_return == pdTRUE) 
        {
            /* 如果是Debug串口接收到命令 */
            if(r_queue == DEBUG_UART_COMMAND)
            {
                const char *Receive_Buff_1 = Serial_RxString.c_str();
                COMMAND_TASK_LOG("Debug Uart receive:%s\n\r",Receive_Buff_1);
                JSON_err = Task_Command.Command_Parsing(Receive_Buff_1);         //解析Debug串口收到的JSON数据
                if(JSON_err == 0)   
                {
                    COMMAND_TASK_LOG("This is not a command\n\r");       //收到的不是一条命令                    
                }
                else if(JSON_err == -1)
                {
                    COMMAND_TASK_LOG("This is not a JSON\n\r");       //收到的不是JSON格式数据                    
                }
                Serial_RxString = "";                   
            }
            /* 如果是蓝牙串口接收到命令 */
            else if(r_queue == BLUETOOTH_COMMAND)
            {
                const char *Receive_Buff_2 = Bluetooth_RxString.c_str();
                COMMAND_TASK_LOG("Bluetooth receive:%s\n\r",Receive_Buff_2);
                JSON_err = Task_Command.Command_Parsing(Receive_Buff_2);         //解析蓝牙串口收到的JSON数据
                if(JSON_err == 0)   
                {
                    COMMAND_TASK_LOG("This is not a command\n\r");       //收到的不是一条命令                    
                }
                else if(JSON_err == -1)
                {
                    COMMAND_TASK_LOG("This is not a JSON\n\r");       //收到的不是JSON格式数据                    
                }
                Bluetooth_RxString = "";
            }
        }
        else
        {
            COMMAND_TASK_LOG("The received message value error");
        }
    } 
}

/*
*********************************************************************************************************
*                                               回调函数
*********************************************************************************************************
*/
/**
 * @brief _cbComDebug Debug串口中断回调函数
 *
 */
void _cbComDebug(void)
{
    BaseType_t x_return = pdPASS;                   //定义一个创建信息返回值，默认为 pdPASS
    uint32_t s_queue = DEBUG_UART_COMMAND;          // 定义一个发送消息的变量

    while (Serial.available()) 
    {
        /* 获取新接收的字符 */
        char inChar = (char)Serial.read();
        Serial_RxString += inChar;
        /* 收到换行符为一整条完整命令 */
        if(inChar == '\n')
        {
            /* 向命令解析任务发送消息 */
            x_return = xQueueSend( Debug_Queue,         //消息队列的句柄 
                                   &s_queue,            //发送的消息内容 
                                   0 );                 //等待时间 0
            break; 
        }                    
    }
}


/* LOG打印设置 */
#if 0
	#define _CBBLUE_LOG     COM_DEBUG.printf
#else
	#define _CBBLUE_LOG     
#endif

/**
 * @brief _cbBluetooth 蓝牙事件回调函数
 * @param esp_spp_cb_event_t event  蓝牙事件 
 * @param esp_spp_cb_param_t *param  参数
 */
void _cbBluetooth(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    BaseType_t xResult;                     //事件组发送结果
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;      //关闭更高优先级任务唤醒
    BaseType_t x_return = pdTRUE;           // 定义一个创建信息返回值，默认为 pdTRUE
    uint32_t s_queue;                       // 定义一个发送消息的变量

    if(event == ESP_SPP_OPEN_EVT || event == ESP_SPP_SRV_OPEN_EVT) //蓝牙连接成功标志 
    {                                                              //蓝牙主机和从机模式对应的标志不同，前面的是主机模式的，后面是从机模式
        _CBBLUE_LOG("connection successful!\r\n");
        /* 置位蓝牙链接事件位 */
        xResult = xEventGroupSetBitsFromISR(Bluetooth_Event,
                                            BLUETOOTH_CONNECT ,             
                                            &xHigherPriorityTaskWoken );                     
        if( xResult != pdFAIL )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        xResult = xEventGroupSetBitsFromISR(Bluetooth_Event,
                                            BLUETOOTH_TRANSFER ,             
                                            &xHigherPriorityTaskWoken );                     
        if( xResult != pdFAIL )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        xResult = xEventGroupSetBitsFromISR(Sampling_Event,
                                            PLANTAR_SAMPLING ,             
                                            &xHigherPriorityTaskWoken );                     
        if( xResult != pdFAIL )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        xResult = xEventGroupSetBitsFromISR(Sampling_Event,
                                            IMU_SAMPLING ,             
                                            &xHigherPriorityTaskWoken );                     
        if( xResult != pdFAIL )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    else if(event == ESP_SPP_CLOSE_EVT)     //蓝牙断开连接标志
    {
        _CBBLUE_LOG("disconnect successful!\r\n");
        /* 清除蓝牙链接事件位 */
        xResult = xEventGroupClearBitsFromISR(Bluetooth_Event,
                                              BLUETOOTH_START_TRANSFER);
        if( xResult != pdFAIL )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        /* 清除压力传感器采集事件位 */
        xResult = xEventGroupClearBitsFromISR(Sampling_Event,
                                              PLANTAR_SAMPLING);
        if( xResult != pdFAIL )
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } 
    }
    else if(event == ESP_SPP_DATA_IND_EVT)  //数据接收标志
    {
        _CBBLUE_LOG("  receive complete! \r\n");
        while (SerialBT.available()) 
        {
            /* 获取新接收的字符 */
            char inChar = (char)SerialBT.read();
            Bluetooth_RxString += inChar;
            /* 收到换行符为一整条完整命令 */
            if(inChar == '\n')
            {
                COM_DEBUG.printf(Bluetooth_RxString.c_str());
                s_queue = BLUETOOTH_COMMAND;          // 定义一个发送消息的变量
                /* 向命令解析任务发送消息 */
                x_return = xQueueSend( Debug_Queue,         //消息队列的句柄 
                                    &s_queue,               //发送的消息内容 
                                    0 );                    //等待时间 0 
                break;
            }                    
        }
    }
    else if(event == ESP_SPP_WRITE_EVT)     //数据发送标志
    {
        _CBBLUE_LOG("  send complete! \r\n");
    }
}
