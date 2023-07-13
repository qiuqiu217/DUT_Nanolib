/*
*********************************************************************************************************
*
*	文件名称 : SPP.h
*	版    本 : V1.0
*	说    明 : 这是室内定位项目蓝牙串口程序文件。
*
*	修改记录 :
*		版本号      日期            作者          说明
*		V1.0    2023-07-10         Qiuqiu      首次发布
*
*	Copyright (C), 2023-2028, c2403336037@mail.dlut.edu.cn
*
*********************************************************************************************************
*/

#include "SPP.h"

/*
*********************************************************************************************************
*	                                      变量声明
*********************************************************************************************************
*/
/* RTOS相关变量 */
extern osMutexId_t SPP_MutexHandle;      //SPP互斥锁

const uint32_t bt_hc04_baud_sel_list[BT_HC04_BAUD_NUM] = { 9600, 19200, 115200, 256000, 460800};

// 初始化指令
const bt_ch04_init_at_cmd_t bt_hc04_init_resp_list[BT_HC04_INIT_CMD_COUNT] = 
{
    // 握手
    {"AT\r\n", "OK\r\n"},

    // 设置串口波特率
    {"AT+BAUD=460800,N\r\n", "OK+BAUD=460800\r\n"},

    // 设置蓝牙名称
    {"AT+NAME=ST-Slave-R\r\n", "OKsetNAME\r\n"},

    // 设置角色为从机
    {"AT+ROLE=Slave\r\n", "OK+ROLE=Slave\r\n"},
    
    // 设置SPP模式
    {"AT+SSP=1\r\n", "OK\r\n"},
};

uint8_t SPP_RXBuffer[64];

/*
*********************************************************************************************************
*	                                      函数定义
*********************************************************************************************************
*/
/**
 * @brief HC04_Init     AT指令初始化蓝牙串口模块
 * @return int8_t RET_ERROR 初始化失败      RET_OK 初始化成功
 */
int8_t HC04_Init(void)
{
    uint16_t send_at_cmd_len = 0;
    uint8_t *send_at_cmd_buf = 0;
    uint8_t *recv_at_cmd_buf = 0;
    uint8_t ctr;
    int8_t ret = RET_ERROR;
    
    if(SPP_MutexHandle != NULL)      //Plantar缓冲区修改互斥锁不为空
    {
        if(xSemaphoreTake(SPP_MutexHandle, pdMS_TO_TICKS(100)) == pdTRUE)       //申请SPP互斥锁
        {
            if(HC04_AutoBaud() == RET_OK)
            {
                /* 修改通讯波特率 */
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
                /* 修改蓝牙名称 */
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
                /* 修改SPP模式 */
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
           SPP_LOG("Application for mutex failed \r\n");      //申请SPP互斥锁失败
        }
        xSemaphoreGive(SPP_MutexHandle);           //释放SPP互斥锁
    }
    else
    {
        SPP_LOG("The mutex not created \r\n");      //SPP互斥锁未创建
        ret = RET_ERROR;
    }
    
    return ret;
}


/**
 * @brief HC04_AutoBaud     HC04蓝牙模块波特率自适应
 * @return int8_t RET_ERROR 初始化失败      RET_OK 初始化成功
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






