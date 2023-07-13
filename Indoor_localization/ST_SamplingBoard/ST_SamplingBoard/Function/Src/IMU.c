/*
*********************************************************************************************************
*
*	文件名称 : IMU.c
*	版    本 : V1.0
*	说    明 : 室内定位项目IMU的采集程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-06-26         Qiuqiu      首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "IMU.h"

/*
*********************************************************************************************************
*	                                      变量声明
*********************************************************************************************************
*/
/* IMU数据及命令标志位 */
volatile uint16_t s_cDataUpdate;
volatile char s_cCmd;
/* IMU串口通讯波特率列表 */
const uint32_t c_uiBaud[10] = {0,4800, 9600, 19200, 38400, 57600, 115200, 230400, 256000, 460800};
/* 惯性传感器Buff结构体声明 */
IMU_Buff_TypeDef IMU_Buff;
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
uint32_t IMU_Time_Stamp_1[FRAME_IN_BUFF];
uint32_t IMU_Time_Stamp_2[FRAME_IN_BUFF];


/*
*********************************************************************************************************
*	                                      函数定义
*********************************************************************************************************
*/
/**
 * @brief IMU_Init  十轴惯性传感器初始化
 *
 */
void IMU_Init(void)
{
    if(WitInit(WIT_PROTOCOL_NORMAL, 0x50) != WIT_HAL_OK)    IMU_LOG("IMU failed to be initialized\n\r");
	if(WitSerialWriteRegister(SensorUartSend) != WIT_HAL_OK)    IMU_LOG("IMU register write function failed to be configured\n\r");
	if(WitRegisterCallBack(SensorDataUpdate) != WIT_HAL_OK)     IMU_LOG("The IMU callback function fails to be configured\n\r");
    if(WitDelayMsRegister(Delayms) != WIT_HAL_OK)   IMU_LOG("The IMU delay function fails to be configured\n\r");
	AutoScanSensor();
    //WitSetBandwidth(BANDWIDTH_256HZ);
    //WitSetOutputRate(RRATE_200HZ);
    //WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG|RSW_PRESS|RSW_Q);
    IMU_LOG("The IMU initialization is complete \n\r");
} 

/**
 * @brief 串口命令处理函数，判断其是否为指令，是指令就保存，不是指令就将计数器左移一位
 *
 * @param ucData 串口接收到的数据
 */
void CopeCmdData(unsigned char ucData)
{
	static unsigned char s_ucData[50], s_ucRxCnt = 0;
	
	s_ucData[s_ucRxCnt++] = ucData;
	if(s_ucRxCnt<3)return;										//Less than three data returned
	if(s_ucRxCnt >= 50) s_ucRxCnt = 0;
	if(s_ucRxCnt >= 3)
	{
		if((s_ucData[1] == '\r') && (s_ucData[2] == '\n'))
		{
			s_cCmd = s_ucData[0];
			memset(s_ucData,0,50);
			s_ucRxCnt = 0;
		}
		else 
		{
			s_ucData[0] = s_ucData[1];
			s_ucData[1] = s_ucData[2];
			s_ucRxCnt = 2;
		}
	}
}

/**
 * @brief 打印帮助
 */
void ShowHelp(void)
{
	IMU_LOG("\r\n************************	 WIT_SDK_DEMO	************************");
	IMU_LOG("\r\n************************          HELP           ************************\r\n");
	IMU_LOG("UART SEND:a\\r\\n   Acceleration calibration.\r\n");
	IMU_LOG("UART SEND:m\\r\\n   Magnetic field calibration,After calibration send:   e\\r\\n   to indicate the end\r\n");
	IMU_LOG("UART SEND:U\\r\\n   Bandwidth increase.\r\n");
	IMU_LOG("UART SEND:u\\r\\n   Bandwidth reduction.\r\n");
	IMU_LOG("UART SEND:B\\r\\n   Baud rate increased to 115200.\r\n");
	IMU_LOG("UART SEND:b\\r\\n   Baud rate reduction to 9600.\r\n");
	IMU_LOG("UART SEND:R\\r\\n   The return rate increases to 10Hz.\r\n");
    IMU_LOG("UART SEND:r\\r\\n   The return rate reduction to 1Hz.\r\n");
    IMU_LOG("UART SEND:C\\r\\n   Basic return content: acceleration, angular velocity, angle, magnetic field.\r\n");
    IMU_LOG("UART SEND:c\\r\\n   Return content: acceleration.\r\n");
    IMU_LOG("UART SEND:h\\r\\n   help.\r\n");
	IMU_LOG("******************************************************************************\r\n");
}

/**
 * @brief 串口发送函数定义
 *
 * @param p_data 发送内容指针
 * @param uiSize 发送长度
 */
void SensorUartSend(uint8_t *p_data, uint32_t uiSize)
{
    comSendBuf(IMU_COM, p_data, uiSize);
}


 void Delayms(uint16_t ucMs)
{
    osDelay(pdMS_TO_TICKS(ucMs));
}


/**
 * @brief 传感器数据更新函数，
 *
 * @param uiReg 寄存器名称
 * @param uiRegNum   读取的长度
 */
void SensorDataUpdate(uint32_t uiReg, uint32_t uiRegNum)
{
	int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)
        {
            case AZ:
				s_cDataUpdate |= ACC_UPDATE;
                break;
            case GZ:
				s_cDataUpdate |= GYRO_UPDATE;
                break;
            case HZ:
				s_cDataUpdate |= MAG_UPDATE;
                break;
            case Yaw:
				s_cDataUpdate |= ANGLE_UPDATE;
                break;
            case PressureH:
                s_cDataUpdate |= ATMO_UPDATE;
                break;
            case HeightH:
                s_cDataUpdate |= HEIGHT_UPDATE;
                break;                
            case q3:
                 s_cDataUpdate |= QUAT_UPDATE;
                 break;
            default:
				s_cDataUpdate |= READ_UPDATE;
			break;
        }
		uiReg++;
    }
}

/**
 * @brief 扫描传感器，自动匹配波特率
 *
 */
static void AutoScanSensor(void)
{
	int i, iRetry;
    uint8_t read;
	
	for(i = 1; i < 10; i++)
	{
		comSetBaud(IMU_COM,c_uiBaud[i]);
		iRetry = 2;
		do
		{
			s_cDataUpdate = 0;
			WitReadReg(AX, 3);
            osDelay(100);
            while (comGetChar(IMU_COM, &read))
            {
                WitSerialDataIn(read);
            }
			if(s_cDataUpdate != 0)
			{
				printf("%d baud find sensor\r\n\r\n", c_uiBaud[i]);
				ShowHelp();
				return ;
			}
			iRetry--;
		}while(iRetry);		
	}
	printf("can not find sensor\r\n");
	printf("please check your connection\r\n");
}

/*
*********************************************************************************************************
*	                                    IMU数据储存处理
*********************************************************************************************************
*/
/**
 * @brief Buff_Init IMU数据缓冲区初始化
 */
void IMU_Buff_Init(void)
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
 * @return _RET_TYPE  RET_ERROR缓冲区未写满  RET_OK缓冲区已经写满
 */
_RET_TYPE IMU_Buff_Full_Judge(void)
{
    IMU_LOG("Number of write frames:%d \n\r",IMU_Buff.Write_Frame); 
    if(IMU_Buff.Write_Frame >= FRAME_IN_BUFF)
    {
        return RET_OK;
    }
    else
    {
        return RET_ERROR;
    }                         
}

/**
 * @brief Read_Write_Buff_Switch 交换读写缓冲区
 *
 * @return _RET_TYPE RET_ERROR表示读写缓冲区异常  RET_OK表示读写缓冲区交换成功
 */
_RET_TYPE IMU_Read_Write_Buff_Switch(void)
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
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_gyro_buff = IMU_Buff.Read_AccBuff;
    write_gyro_buff = IMU_Buff.Write_AccBuff;
    /* 判断陀螺仪读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_GyroBuff == NULL || IMU_Buff.Read_GyroBuff == NULL)         //读写缓冲区不指向NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_acc_buff = IMU_Buff.Read_GyroBuff;
    write_acc_buff = IMU_Buff.Write_GyroBuff;
    /* 判断倾角读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_AngleBuff == NULL || IMU_Buff.Read_AngleBuff == NULL)         //读写缓冲区不指向NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_angle_buff = IMU_Buff.Read_AngleBuff;
    write_angle_buff = IMU_Buff.Write_AngleBuff;
    /* 判断磁力计读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_MagBuff == NULL || IMU_Buff.Read_MagBuff == NULL)         //读写缓冲区不指向NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_mag_buff = IMU_Buff.Read_MagBuff;
    write_mag_buff = IMU_Buff.Write_MagBuff;
    /* 判断气压计读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_AtmoBuff == NULL || IMU_Buff.Read_AtmoBuff == NULL)         //读写缓冲区不指向NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_atmo_buff = IMU_Buff.Read_AtmoBuff;
    write_atmo_buff = IMU_Buff.Write_AtmoBuff;
    /* 判断高度值读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_HightBuff == NULL || IMU_Buff.Read_HightBuff == NULL)         //读写缓冲区不指向NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_hight_buff = IMU_Buff.Read_HightBuff;
    write_hight_buff = IMU_Buff.Write_HightBuff;
    /* 判断四元数读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_QuatBuff == NULL || IMU_Buff.Read_QuatBuff == NULL)         //读写缓冲区不指向NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_quat_buff = IMU_Buff.Read_QuatBuff;
    write_quat_buff = IMU_Buff.Write_QuatBuff;
    /* 判断时间戳读写缓冲区状态是否异常 */
    if(IMU_Buff.Write_Time_Buff == NULL || IMU_Buff.Read_Time_Buff == NULL)         //读写缓冲区不指向NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_timestamp_buff = IMU_Buff.Read_Time_Buff;
    write_timestamp_buff = IMU_Buff.Write_Time_Buff;
        
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
   
    return RET_OK;                     
}

/**
 * @brief Put_into_Buff 将采集出来的IMU数据存入相应缓冲区
 *
 * @param float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat
 * @return _RET_TYPE RET_ERROR 存入Buff失败      RET_OK 存入Buff成功
 */
_RET_TYPE IMU_Put_into_Buff(float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat)
{
    uint8_t i = 0;
    /* 判断写缓冲区状态是否异常 */
    if(IMU_Buff.Read_AccBuff == NULL || IMU_Buff.Read_GyroBuff == NULL || IMU_Buff.Read_AngleBuff == NULL || IMU_Buff.Read_MagBuff == NULL)         //写缓冲区不指向NULL
    {
        IMU_LOG("Write buffer pointer error \n\r");
        return RET_ERROR;       
    } 
    /* 向缓冲区写入数据 */  
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

    return RET_OK;                     
}

/**
 * @brief IMU_TimeStamp 存入IMU采集时间戳
 */
void IMU_TimeStamp(void)
{
    *(IMU_Buff.Write_Time_Buff + IMU_Buff.Write_Frame) = Get_TimeStamp();
}


