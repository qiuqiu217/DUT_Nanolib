#ifndef __APP_APE_LIB_H__ 
#define __APP_APE_LIB_H__ 
#include "apedecoder.h"
#include "parser.h"
#include "stm32h7xx_hal.h"
#include "includes.h"

//********************************************************************************
//说明
//1,支持16位单声道/立体声ape的解码
//2,最高支持44.1K的APE格式(Fast流畅)
//3,对某些ape文件,可能不支持,可用千千静听软件进行转换一下,即可正常播放
////////////////////////////////////////////////////////////////////////////////// 	 
 
#define APE_FILE_BUF_SZ    			40*1024		//APE解码时,文件buf大小
#define APE_BLOCKS_PER_LOOP			4*1024		 //APE解码时,每个循环解码block的个数

//APE控制结构体
typedef __packed struct 
{ 
    u32 totsec ;				   //整首歌时长,单位:秒
    u32 cursec ;				   //当前播放时长
	
    u32 bitrate;	   			//比特率
	u32 samplerate;				  //采样率
	u16 outsamples;				  //PCM输出数据量大小
	u16 bps;					      //位数
	uint32_t ch;                   //声道
	u32 datastart;				 //数据帧开始的位置(在文件里面的偏移)
	
	u8 *i2sbuf0;           // 声音缓存0
	u8 *i2sbuf1;           // 声音缓存1
	u8 *i2STempBuff;        // 声音数据缓存
	u32 bytesinbuffer;     //数据开始的位置
	u32 totalframes;        //总块数
}__apectrl;

extern __apectrl * apectrl; 

//取2个值里面的较小值.
#ifndef AUDIO_MIN			
#define AUDIO_MIN(x,y)	((x)<(y)? (x):(y))
#endif

uint32_t GetAPERuntime(uint8_t *pFileName, uint32_t *uiTotalTime);
void ape_fill_buffer(u16* buf,u16 size);
void ape_i2s_dma_tx_callback(void);
void ape_get_curtime(FIL*fx,__apectrl *apectrl);
void APEMusicPlay(const char *filename);
#endif
