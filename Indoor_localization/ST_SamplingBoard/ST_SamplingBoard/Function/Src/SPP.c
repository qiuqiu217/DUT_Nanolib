/*
*********************************************************************************************************
*
*	�ļ����� : SPP.h
*	��    �� : V1.0
*	˵    �� : �������ڶ�λ��Ŀ�������ڳ����ļ���
*
*	�޸ļ�¼ :
*		�汾��      ����            ����          ˵��
*		V1.0    2023-07-10         Qiuqiu      �״η���
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "SPP.h"

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/* RTOS��ر��� */
extern osMutexId_t SPP_MutexHandle;      //SPP������

const uint32_t bt_hc04_baud_sel_list[BT_HC04_BAUD_NUM] = { 9600, 19200, 115200, 256000, 460800};

// ��ʼ��ָ��
const bt_ch04_init_at_cmd_t bt_hc04_init_resp_list[BT_HC04_INIT_CMD_COUNT] = 
{
    // ����
    {"AT\r\n", "OK\r\n"},

    // ���ô��ڲ�����
    {"AT+BAUD=460800,N\r\n", "OK+BAUD=460800\r\n"},

    // ������������
    {"AT+NAME=ST-Slave-R\r\n", "OKsetNAME\r\n"},

    // ���ý�ɫΪ�ӻ�
    {"AT+ROLE=Slave\r\n", "OK+ROLE=Slave\r\n"},
    
    // ����SPPģʽ
    {"AT+SSP=1\r\n", "OK\r\n"},
};

uint8_t SPP_RXBuffer[64];

/*
*********************************************************************************************************
*	                                      ��������
*********************************************************************************************************
*/
/**
 * @brief HC04_Init     ATָ���ʼ����������ģ��
 * @return int8_t RET_ERROR ��ʼ��ʧ��      RET_OK ��ʼ���ɹ�
 */
int8_t HC04_Init(void)
{
    uint16_t send_at_cmd_len = 0;
    uint8_t *send_at_cmd_buf = 0;
    uint8_t *recv_at_cmd_buf = 0;
    uint8_t ctr;
    int8_t ret = RET_ERROR;
    
    if(SPP_MutexHandle != NULL)      //Plantar�������޸Ļ�������Ϊ��
    {
        if(xSemaphoreTake(SPP_MutexHandle, pdMS_TO_TICKS(100)) == pdTRUE)       //����SPP������
        {
            if(HC04_AutoBaud() == RET_OK)
            {
                /* �޸�ͨѶ������ */
                send_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[1].tx;
                send_at_cmd_len = strlen((char *)send_at_cmd_buf);
                recv_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[1].rx;
                comClearRxFifo(SPP_COM);
                memset(SPP_RXBuffer, '\0', sizeof(SPP_RXBuffer));
                comSendBuf(SPP_COM, send_at_cmd_buf, send_at_cmd_len);
                SPP_LOG("Command send:%s \r\n",send_at_cmd_buf);
                osDelay(pdMS_TO_TICKS(800));
                ctr = 0;
                while(comGetChar(SPP_COM, &SPP_RXBuffer[ctr]))
                {
                    ctr++;
                }                       
                SPP_LOG("Command receive:%s \r\n",SPP_RXBuffer);
                if(strcmp((char *)SPP_RXBuffer,(char *)recv_at_cmd_buf) == 0)
                {
                    comSetBaud(SPP_COM, 460800);
                    ret = RET_OK;
                }
                else    ret = RET_ERROR;
                /* �޸��������� */
                send_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[2].tx;
                send_at_cmd_len = strlen((char *)send_at_cmd_buf);
                recv_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[2].rx;
                comClearRxFifo(SPP_COM);
                memset(SPP_RXBuffer, '\0', sizeof(SPP_RXBuffer));
                comSendBuf(SPP_COM, send_at_cmd_buf, send_at_cmd_len);
                SPP_LOG("Command send:%s \r\n",send_at_cmd_buf);
                osDelay(pdMS_TO_TICKS(800));
                ctr = 0;
                while(comGetChar(SPP_COM, &SPP_RXBuffer[ctr]))
                {
                    ctr++;
                }                       
                SPP_LOG("Command receive:%s \r\n",SPP_RXBuffer);
                if(strcmp((char *)SPP_RXBuffer,(char *)recv_at_cmd_buf) == 0)    ret = RET_OK;
                else    ret = RET_ERROR;
                /* �޸�SPPģʽ */
                send_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[4].tx;
                send_at_cmd_len = strlen((char *)send_at_cmd_buf);
                recv_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[4].rx;
                comClearRxFifo(SPP_COM);
                memset(SPP_RXBuffer, '\0', sizeof(SPP_RXBuffer));
                comSendBuf(SPP_COM, send_at_cmd_buf, send_at_cmd_len);
                SPP_LOG("Command send:%s \r\n",send_at_cmd_buf);
                osDelay(pdMS_TO_TICKS(800));
                ctr = 0;
                while(comGetChar(SPP_COM, &SPP_RXBuffer[ctr]))
                {
                    ctr++;
                }                       
                SPP_LOG("Command receive:%s \r\n",SPP_RXBuffer);
                if(strcmp((char *)SPP_RXBuffer,(char *)recv_at_cmd_buf) == 0)    ret = RET_OK;
                else    ret = RET_ERROR;
            }  
        }
        else
        {
           SPP_LOG("Application for mutex failed \r\n");      //����SPP������ʧ��
        }
        xSemaphoreGive(SPP_MutexHandle);           //�ͷ�SPP������
    }
    else
    {
        SPP_LOG("The mutex not created \r\n");      //SPP������δ����
        ret = RET_ERROR;
    }
    
    return ret;
}


/**
 * @brief HC04_AutoBaud     HC04����ģ�鲨��������Ӧ
 * @return int8_t RET_ERROR ��ʼ��ʧ��      RET_OK ��ʼ���ɹ�
 */
int8_t HC04_AutoBaud(void)
{
    uint16_t send_at_cmd_len = 0;
    uint8_t *send_at_cmd_buf = 0;
    uint8_t *recv_at_cmd_buf = 0;
    uint8_t ctr;
    uint8_t i;
    int8_t ret = RET_ERROR;
    
    send_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[0].tx;
    send_at_cmd_len = strlen((char *)send_at_cmd_buf);
    recv_at_cmd_buf = (uint8_t *)bt_hc04_init_resp_list[0].rx;
    for(i=0; i<BT_HC04_BAUD_NUM; i++)
    {
        comClearRxFifo(SPP_COM);
        memset(SPP_RXBuffer, '\0', sizeof(SPP_RXBuffer));
        comSetBaud(SPP_COM, bt_hc04_baud_sel_list[i]);
        comSendBuf(SPP_COM, send_at_cmd_buf, send_at_cmd_len);
        SPP_LOG("Command send:%s \n\r",send_at_cmd_buf);
        osDelay(pdMS_TO_TICKS(100));
        ctr = 0;
        while(comGetChar(SPP_COM, &SPP_RXBuffer[ctr]))
        {
            ctr++;
        }                       
        SPP_LOG("Command receive:%s \n\r",SPP_RXBuffer);
        if(strcmp((char *)SPP_RXBuffer,(char *)recv_at_cmd_buf) == 0)
        {
            ret = RET_OK;
            break;
        }
    }
    
    return ret;
}






