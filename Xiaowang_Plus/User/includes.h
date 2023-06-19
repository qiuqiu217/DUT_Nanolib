/*
*********************************************************************************************************
*
*	模块名称 : 头文件汇总
*	文件名称 : includes.h
*	版    本 : V1.0
*	说    明 : 当前使用头文件汇总
*
*	修改记录 :
*		版本号    日期        作者     说明
*		V1.0    2015-08-02  Eric2013   首次发布
*
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

/*
*********************************************************************************************************
*                                         标准库
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>


/*
*********************************************************************************************************
*                                         其它库
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>
#include  <app_cfg.h>


/*
*********************************************************************************************************
*                                           OS
*********************************************************************************************************
*/

#include  <os.h>


/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/
typedef signed char        s8;
typedef unsigned char      u8;
typedef short              s16;
typedef unsigned short     u16;
typedef int                s32;
typedef unsigned int       u32;
typedef long long          s64;
typedef unsigned long long u64;


/* 音乐播放器用到的事件标志 */
#define MusicTaskReserved_0	        	(1 << 0)
#define MusicTaskAudioAdjust_1	        (1 << 1)
#define MusicTaskAudioPause_2			(1 << 2)
#define MusicTaskAudioResume_3			(1 << 3)
#define MusicTaskReserved_4				(1 << 4)
#define MusicTaskAudioPlay_5			(1 << 5)
#define MusicTaskAudioReturn_6			(1 << 6)
#define MusicTaskAudioGetTime_7			(1 << 7)
#define MusicTaskAudioStart_8			(1 << 8)
#define MusicTaskAudioFillBuffer2_9		(1 << 9)
#define MusicTaskAudioFillBuffer1_10	(1 << 10)
#define MusicTaskReserved_11			(1 << 11)
#define MusicTaskReserved_12			(1 << 12)
#define MusicTaskReserved_13			(1 << 13)
#define MusicTaskReserved_14			(1 << 14)
#define MusicTaskReserved_15			(1 << 15)

#define MusicTaskWaitFlag   		    (MusicTaskAudioFillBuffer1_10 | MusicTaskAudioFillBuffer2_9 | MusicTaskAudioReturn_6 | MusicTaskAudioGetTime_7)
#define MusicTaskWaitBuffer  		    (MusicTaskAudioFillBuffer1_10 | MusicTaskAudioFillBuffer2_9)

/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/

#include  <bsp.h>
#include "bsp_os.h"
#include "Global.h"
#include "ff.h"
#include "rt_Memory.h"
#include "app_audio_lib.h"
#include "app_wav_lib.h"

/*
*********************************************************************************************************
*                                          变量和函数
*********************************************************************************************************
*/
extern uint64_t AppMalloc[100*1024/8];     /* 任务栈 */
extern uint64_t AppMallocCCM[64*1024/8] __attribute__((at(0x10000000)));
extern uint64_t SDRAM[4*1024*1024/8] __attribute__((at(EXT_SDRAM_ADDR + 12 * 1024 * 1024)));
extern OS_FLAG_GRP	  FLAG_Music;

/* 方便RTOS里面使用 */
extern void SysTick_ISR(void);

#define bsp_ProPer1ms  SysTick_ISR


#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
