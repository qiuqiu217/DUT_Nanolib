#ifndef __APP_FLAC_LIB_H__ 
#define __APP_FLAC_LIB_H__  

#include "stdint.h" 
//flaC 标签 
typedef __packed struct 
{
   uint8_t id[3];		   	//ID,在文件起始位置,必须是flaC 4个字母 
}FLAC_Tag;

//metadata 数据块头信息结构体 
typedef __packed struct 
{
  uint8_t head;		   	//metadata block头
	uint8_t size[3];			//metadata block数据长度	
}MD_Block_Head;


//FLAC控制结构体
typedef __packed struct 
{ 
    uint32_t totsec ;				   //整首歌时长,单位:秒
    uint32_t cursec ;				   //当前播放时长
	
    uint32_t bitrate;	   			//比特率
	uint32_t samplerate;				//采样率
	uint16_t outsamples;				//PCM输出数据量大小
	uint16_t bps;					      //位数,比如16bit,24bit,32bit
	uint32_t  ch;                //声道
	uint32_t datastart;				 //数据帧开始的位置(在文件里面的偏移)
	int bytesleft;				     //文件位置
	
	uint8_t* i2sbuf0;          // 声音缓存0
	uint8_t* i2sbuf1;          // 声音缓存1
	uint8_t* i2STempBuff;      // 声音数据缓存
	uint8_t* FileBuff;         // 文件缓存

}__flacctrl;

extern __flacctrl * flacctrl;



uint32_t GetFLACRuntime(uint8_t *pFileName, uint32_t *uiTotalTime);
void GetFLACInfo(uint8_t *_pPath);
void FLACMusicPlay(const char *filename);
#endif




























