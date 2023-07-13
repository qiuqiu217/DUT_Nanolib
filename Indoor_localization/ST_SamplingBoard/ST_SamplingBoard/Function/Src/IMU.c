/*
*********************************************************************************************************
*
*	�ļ����� : IMU.c
*	��    �� : V1.0
*	˵    �� : ���ڶ�λ��ĿIMU�Ĳɼ������ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-06-26         Qiuqiu      �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "IMU.h"

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/* IMU���ݼ������־λ */
volatile uint16_t s_cDataUpdate;
volatile char s_cCmd;
/* IMU����ͨѶ�������б� */
const uint32_t c_uiBaud[10] = {0,4800, 9600, 19200, 38400, 57600, 115200, 230400, 256000, 460800};
/* ���Դ�����Buff�ṹ������ */
IMU_Buff_TypeDef IMU_Buff;
/* IMU���ݻ������������� */
float IMU_Accelerated_Buff_1[3*FRAME_IN_BUFF];              //IMU���ٶ�Buff1
float IMU_Accelerated_Buff_2[3*FRAME_IN_BUFF];              //IMU���ٶ�Buff2
float IMU_Gyroscope_Buff_1[3*FRAME_IN_BUFF];                //IMU������Buff1
float IMU_Gyroscope_Buff_2[3*FRAME_IN_BUFF];                //IMU������Buff2
float IMU_Angle_Buff_1[3*FRAME_IN_BUFF];                    //IMU���Buff1
float IMU_Angle_Buff_2[3*FRAME_IN_BUFF];                    //IMU���Buff2
int16_t IMU_Magnetic_Buff_1[3*FRAME_IN_BUFF];               //IMU������Buff1
int16_t IMU_Magnetic_Buff_2[3*FRAME_IN_BUFF];               //IMU������Buff2
int32_t IMU_Atmospheric_Buff_1[FRAME_IN_BUFF];              //IMU��ѹ��Buff1
int32_t IMU_Atmospheric_Buff_2[FRAME_IN_BUFF];              //IMU��ѹ��Buff2
int32_t IMU_Height_Buff_1[FRAME_IN_BUFF];                   //IMU�߶�Buff1
int32_t IMU_Height_Buff_2[FRAME_IN_BUFF];                   //IMU�߶�Buff2
float IMU_Quaternion_Buff_1[4*FRAME_IN_BUFF];               //IMU��Ԫ��Buff1
float IMU_Quaternion_Buff_2[4*FRAME_IN_BUFF];               //IMU��Ԫ��Buff2
/* ʱ����������������� */
uint32_t IMU_Time_Stamp_1[FRAME_IN_BUFF];
uint32_t IMU_Time_Stamp_2[FRAME_IN_BUFF];


/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/**
 * @brief IMU_Init  ʮ����Դ�������ʼ��
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
 * @brief ��������������ж����Ƿ�Ϊָ���ָ��ͱ��棬����ָ��ͽ�����������һλ
 *
 * @param ucData ���ڽ��յ�������
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
 * @brief ��ӡ����
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
 * @brief ���ڷ��ͺ�������
 *
 * @param p_data ��������ָ��
 * @param uiSize ���ͳ���
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
 * @brief ���������ݸ��º�����
 *
 * @param uiReg �Ĵ�������
 * @param uiRegNum   ��ȡ�ĳ���
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
 * @brief ɨ�贫�������Զ�ƥ�䲨����
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
*	                                    IMU���ݴ��洦��
*********************************************************************************************************
*/
/**
 * @brief Buff_Init IMU���ݻ�������ʼ��
 */
void IMU_Buff_Init(void)
{
    /* ��ʼ��Buff */
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
 * @brief Buff_Full_Judge �жϻ������Ƿ�д��
 *
 * @return _RET_TYPE  RET_ERROR������δд��  RET_OK�������Ѿ�д��
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
 * @brief Read_Write_Buff_Switch ������д������
 *
 * @return _RET_TYPE RET_ERROR��ʾ��д�������쳣  RET_OK��ʾ��д�����������ɹ�
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
    /* �жϼ��ٶȶ�д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_AccBuff == NULL || IMU_Buff.Read_AccBuff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_gyro_buff = IMU_Buff.Read_AccBuff;
    write_gyro_buff = IMU_Buff.Write_AccBuff;
    /* �ж������Ƕ�д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_GyroBuff == NULL || IMU_Buff.Read_GyroBuff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_acc_buff = IMU_Buff.Read_GyroBuff;
    write_acc_buff = IMU_Buff.Write_GyroBuff;
    /* �ж���Ƕ�д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_AngleBuff == NULL || IMU_Buff.Read_AngleBuff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_angle_buff = IMU_Buff.Read_AngleBuff;
    write_angle_buff = IMU_Buff.Write_AngleBuff;
    /* �жϴ����ƶ�д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_MagBuff == NULL || IMU_Buff.Read_MagBuff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_mag_buff = IMU_Buff.Read_MagBuff;
    write_mag_buff = IMU_Buff.Write_MagBuff;
    /* �ж���ѹ�ƶ�д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_AtmoBuff == NULL || IMU_Buff.Read_AtmoBuff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_atmo_buff = IMU_Buff.Read_AtmoBuff;
    write_atmo_buff = IMU_Buff.Write_AtmoBuff;
    /* �жϸ߶�ֵ��д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_HightBuff == NULL || IMU_Buff.Read_HightBuff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_hight_buff = IMU_Buff.Read_HightBuff;
    write_hight_buff = IMU_Buff.Write_HightBuff;
    /* �ж���Ԫ����д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_QuatBuff == NULL || IMU_Buff.Read_QuatBuff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_quat_buff = IMU_Buff.Read_QuatBuff;
    write_quat_buff = IMU_Buff.Write_QuatBuff;
    /* �ж�ʱ�����д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Write_Time_Buff == NULL || IMU_Buff.Read_Time_Buff == NULL)         //��д��������ָ��NULL
    {
        IMU_LOG("Read write buffer pointer error \n\r");
        return RET_ERROR;       
    }
    read_timestamp_buff = IMU_Buff.Read_Time_Buff;
    write_timestamp_buff = IMU_Buff.Write_Time_Buff;
        
    /* ����д������ */
    IMU_Buff.Write_AccBuff = read_acc_buff;
    IMU_Buff.Write_GyroBuff = read_gyro_buff;
    IMU_Buff.Write_AngleBuff = read_angle_buff;
    IMU_Buff.Write_MagBuff = read_mag_buff;
    IMU_Buff.Write_AtmoBuff = read_atmo_buff;
    IMU_Buff.Write_HightBuff = read_hight_buff;
    IMU_Buff.Write_QuatBuff = read_quat_buff;
    IMU_Buff.Write_Time_Buff = read_timestamp_buff;                
    /* ���ö������� */
    IMU_Buff.Read_AccBuff = write_acc_buff;
    IMU_Buff.Read_GyroBuff = write_gyro_buff;
    IMU_Buff.Read_AngleBuff = write_angle_buff;
    IMU_Buff.Read_MagBuff = write_mag_buff;
    IMU_Buff.Read_AtmoBuff = write_atmo_buff;
    IMU_Buff.Read_HightBuff = write_hight_buff;
    IMU_Buff.Read_QuatBuff = write_quat_buff;
    IMU_Buff.Read_Time_Buff = write_timestamp_buff;
    /* ���ö�д֡ */
    IMU_Buff.Write_Frame = 0;
   
    return RET_OK;                     
}

/**
 * @brief Put_into_Buff ���ɼ�������IMU���ݴ�����Ӧ������
 *
 * @param float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat
 * @return _RET_TYPE RET_ERROR ����Buffʧ��      RET_OK ����Buff�ɹ�
 */
_RET_TYPE IMU_Put_into_Buff(float *acc, float *gyro, float *angle, int16_t *mag, int32_t atom, int32_t hight, float *quat)
{
    uint8_t i = 0;
    /* �ж�д������״̬�Ƿ��쳣 */
    if(IMU_Buff.Read_AccBuff == NULL || IMU_Buff.Read_GyroBuff == NULL || IMU_Buff.Read_AngleBuff == NULL || IMU_Buff.Read_MagBuff == NULL)         //д��������ָ��NULL
    {
        IMU_LOG("Write buffer pointer error \n\r");
        return RET_ERROR;       
    } 
    /* �򻺳���д������ */  
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
 * @brief IMU_TimeStamp ����IMU�ɼ�ʱ���
 */
void IMU_TimeStamp(void)
{
    *(IMU_Buff.Write_Time_Buff + IMU_Buff.Write_Frame) = Get_TimeStamp();
}


