/*
*********************************************************************************************************
*
*	文件名称 : IMU.cpp
*	版    本 : V1.0
*	说    明 : 十轴惯性传感器驱动
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
extern volatile uint16_t s_cDataUpdate;
extern volatile char s_cCmd;
const uint32_t c_uiBaud[10] = {0,4800, 9600, 19200, 38400, 57600, 115200, 230400, 256000, 460800};

/**
 * @brief IMU_Init  十轴惯性传感器初始化
 *
 */
void IMU_Init(void)
{
    WitInit(WIT_PROTOCOL_NORMAL, 0x50);
	WitSerialWriteRegister(SensorUartSend);
	WitRegisterCallBack(SensorDataUpdate);
    WitDelayMsRegister(Delayms);
	AutoScanSensor();
    WitSetBandwidth(BANDWIDTH_256HZ);
    WitSetOutputRate(RRATE_200HZ);
    WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG|RSW_PRESS|RSW_Q);
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
	if(s_ucRxCnt<3)             //Less than three data returned
        return
        ;										
	if(s_ucRxCnt >= 50) 
        s_ucRxCnt = 0
        ;
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
 * @brief 命令处理函数，根据CopeCmdData接收到的命令执行相应的操作
 */
void CmdProcess(void)
{
	switch(s_cCmd)
	{
		case 'a':	if(WitStartAccCali() != WIT_HAL_OK) IMU_LOG("\r\nSet AccCali Error\r\n");
			break;
		case 'm':	if(WitStartMagCali() != WIT_HAL_OK) IMU_LOG("\r\nSet MagCali Error\r\n");
			break;
		case 'e':	if(WitStopMagCali() != WIT_HAL_OK) IMU_LOG("\r\nSet MagCali Error\r\n");
			break;
		case 'u':	if(WitSetBandwidth(BANDWIDTH_5HZ) != WIT_HAL_OK) IMU_LOG("\r\nSet Bandwidth Error\r\n");
			break;
		case 'U':	if(WitSetBandwidth(BANDWIDTH_256HZ) != WIT_HAL_OK) IMU_LOG("\r\nSet Bandwidth Error\r\n");
			break;
		case 'B':	if(WitSetUartBaud(WIT_BAUD_115200) != WIT_HAL_OK) IMU_LOG("\r\nSet Baud Error\r\n");
              else 
              {
                COM_IMU.begin(c_uiBaud[WIT_BAUD_115200]);
                IMU_LOG(" 115200 Baud rate modified successfully\r\n");
              }
			break;
		case 'b':	if(WitSetUartBaud(WIT_BAUD_9600) != WIT_HAL_OK) IMU_LOG("\r\nSet Baud Error\r\n");
              else 
              {
                COM_IMU.begin(c_uiBaud[WIT_BAUD_9600]); 
                IMU_LOG(" 9600 Baud rate modified successfully\r\n");
              }
			break;
		case 'r': if(WitSetOutputRate(RRATE_1HZ) != WIT_HAL_OK)  IMU_LOG("\r\nSet Baud Error\r\n");
			        else IMU_LOG("\r\nSet Baud Success\r\n");
			break;
		case 'R':	if(WitSetOutputRate(RRATE_10HZ) != WIT_HAL_OK) IMU_LOG("\r\nSet Baud Error\r\n");
              else IMU_LOG("\r\nSet Baud Success\r\n");
			break;
        case 'C': if(WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG) != WIT_HAL_OK) IMU_LOG("\r\nSet RSW Error\r\n");
            break;
        case 'c': if(WitSetContent(RSW_ACC) != WIT_HAL_OK) IMU_LOG("\r\nSet RSW Error\r\n");
            break;
		case 'h':	ShowHelp();
			break;
		default :
            break;
	}
	s_cCmd = 0xff;
}

/**
 * @brief 串口发送函数定义
 *
 * @param p_data 发送内容指针
 * @param uiSize 发送长度
 */
void SensorUartSend(uint8_t *p_data, uint32_t uiSize)
{
    COM_IMU.write(p_data, uiSize);//调用arduinoAPI进行串口写
    COM_IMU.flush();//清空串口缓冲区
}


 void Delayms(uint16_t ucMs)
{
    delay(ucMs);
}


void SensorDataRead(void)
{
    WitReadReg(AX, 3);
    WitReadReg(GX, 3);
    WitReadReg(HX, 3);
    WitReadReg(PressureL, 2);
    WitReadReg(HeightL, 2);
    WitReadReg(q0, 4);
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
 * @brief 扫描传感器，自动匹配波特率。
 *
 */
void AutoScanSensor(void)
{
	int i, iRetry;
	
	for(i = 0; i < sizeof(c_uiBaud)/sizeof(c_uiBaud[0]); i++)
	{
   
		COM_IMU.begin(c_uiBaud[i],SERIAL_8N1,16,17);
        COM_IMU.flush();
		iRetry = 2;
		s_cDataUpdate = 0;
    
		do
		{
			WitReadReg(AX, 3);
			delay(200);
            while (COM_IMU.available())
            {
                WitSerialDataIn(COM_IMU.read());
            }
			if(s_cDataUpdate != 0)
			{
				IMU_LOG("Baud rate:%d \n\r",c_uiBaud[i]);
				IMU_LOG("baud find sensor \n\r");
				ShowHelp();
				return ;
			}
			iRetry--;
		}
        while(iRetry);		
	}
	IMU_LOG("can not find sensor\r\n");
	IMU_LOG("please check your connection\r\n");
}