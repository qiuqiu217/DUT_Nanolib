





#include "includes.h"
#include "MainTask.h"	 
#include "bitstreamf.h"
#include "flacdecoder.h"

/********************************************************************************
//说明
//1,支持16位单声道/立体声flac的解码最高支持96K的flac格式(流畅)
//2,支持24位单声道/立体声flac的解码最高支持48K的flac格式(流畅)
//3,对某些flac文件,可能不支持,可用格式工厂软件进行转换一下,即可正常播放
********************************************************************************/ 	 

__flacctrl * flacctrl;	//flac解码控制结构体
FLACContext *fc;



//分析FLAC文件
//fx:flac文件指针
//fc:flac解码容器
//返回值:0,分析成功
// 其他,错误代码
u8 flac_init(FIL* fx)
{
	FLAC_Tag * flactag;
	MD_Block_Head *flacblkh;
	uint8_t *buf; 
	u8 endofmetadata=0;			//最后一个metadata标记
	int blocklength; 
	u32 br;
	u8 res;

	buf=rt_alloc_mem(SDRAM,512);	//申请256字节内存
	f_lseek(fx,0);				//偏移到文件头
	f_read(fx,buf,4,&br);		//读取4字节 
	flactag=(FLAC_Tag*)buf;		//强制转换为flac tag标签
	if(strncmp("fLaC",(char*)flactag->id,4)!=0) 
	{
		rt_free_mem(SDRAM, buf);		//释放内存
		return 2;				//非flac文件
    } 
    while(!endofmetadata) 
	 {
		  f_read(fx,buf,4,&br);
        if(br<4)break;
		  flacblkh=(MD_Block_Head*)buf;
		  endofmetadata=flacblkh->head&0X80;	//判断是不是最后一个block?
		  blocklength=((u32)flacblkh->size[0]<<16)|((u16)flacblkh->size[1]<<8)|(flacblkh->size[2]);//得到块大小
        if((flacblkh->head&0x7f)==0) 		//head最低7位为0,则表示是STREAMINFO块
         { 
			      res=f_read(fx,buf,blocklength,&br);
            if(res!=FR_OK)break;  
            fc->min_blocksize=((u16)buf[0]<<8) |buf[1];					//最小块大小
            fc->max_blocksize=((u16)buf[2]<<8) |buf[3];					//最大块大小
            fc->min_framesize=((u32)buf[4]<<16)|((u16)buf[5]<<8)|buf[6];//最小帧大小
            fc->max_framesize=((u32)buf[7]<<16)|((u16)buf[8]<<8)|buf[9];//最大帧大小
            fc->samplerate=((u32)buf[10]<<12)|((u16)buf[11]<<4)|((buf[12]&0xf0)>>4);//采样率
            fc->channels=((buf[12]&0x0e)>>1)+1;							//音频通道数
            fc->bps=((((u16)buf[12]&0x01)<<4)|((buf[13]&0xf0)>>4))+1;	//采样位数16?24?32? 
            fc->totalsamples=((u32)buf[14]<<24)|((u32)buf[15]<<16)|((u16)buf[16]<<8)|buf[17];//一个声道的总采样数

			flacctrl->samplerate=fc->samplerate;       //采样率
				
      //fctrl->bps=fc->bps;					 
			flacctrl->totsec=(fc->totalsamples/fc->samplerate);//得到总时间 
        }else 	//忽略其他帧的处理 
		      { 
            if(f_lseek(fx,fx->fptr+blocklength)!=FR_OK)
            { 
				       rt_free_mem(SDRAM, buf); //释放内存.
				       return 3;
            }
		      }
    } 
	rt_free_mem(SDRAM, buf);//释放内存.
	if(flacctrl->totsec)
	{
		flacctrl->outsamples=fc->max_blocksize*2;//PCM输出数据量(*2,表示2个声道的数据量)
		flacctrl->bps=fc->bps;			//采样位数(16/24/32)
		flacctrl->ch=fc->channels;     //声道
		flacctrl->datastart=fx->fptr;	//FLAC数据帧开始的地址
		flacctrl->bitrate=((fx->fsize-flacctrl->datastart)*8)/flacctrl->totsec;//得到FLAC的位速
	}else return 2;	//总时间为0?有问题的flac文件
	return 0;
} 

/*
*********************************************************************************************************
*	函 数 名: Flac_Memory
*	功能说明: 专门用于获取FLAC音频运行时的动态内存
*	形    参: FM     0获取，1释放。
*	返 回 值: 0 表示成功， 1表示失败
*********************************************************************************************************
*/
uint8_t Flac_Memory(int8_t FM)
{
	u8 res;
  if(FM==0)   //获取内存
	{		
	 res=rt_init_mem (SDRAM, 4*1024*1024);
	 if(res!=0)return 1;
	 fc=rt_alloc_mem(SDRAM,sizeof(FLACContext));
	 flacctrl=rt_alloc_mem(SDRAM,sizeof(__flacctrl));
	}
	else//释放内存.
	{
	  rt_free_mem(SDRAM, flacctrl);
		rt_free_mem(SDRAM, fc);
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: GetFLACRuntime
*	功能说明: 专门用于获取FLAC音频的运行时间，方便emWin显示歌曲列表时时间的显示。
*	形    参: pFileName     文件句柄 
*             uiTotalTime   歌曲执行时间
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
uint32_t GetFLACRuntime(uint8_t *pFileName, uint32_t *uiTotalTime)
{
	
	FIL fout_flac;
  u8 res;	
	
	/* 获取FLAC相关信息，主要是执行时间 */
	res=Flac_Memory(0);
	if(res!=0)return FALSE;
	res=f_open(&fout_flac,(TCHAR*)pFileName,FA_READ); //读取文件错误
  if(res!=0)return FALSE;	
	flac_init(&fout_flac);	//flac解码初始化 (pFileName, &MP3Control);
	f_close(&fout_flac);
	*uiTotalTime = flacctrl->totsec ;
	
	return TRUE;	
}
/*
*********************************************************************************************************
*	函 数 名: GetFLACInfoo
*	功能说明: 专门用于获取FLAC音频的相关信息，方便播放FLAC歌曲时相关信息的显示。
*	形    参: _pPath   文件句柄 
*	返 回 值: 无
*********************************************************************************************************
*/
void GetFLACInfo(uint8_t *_pPath)
{
	uint32_t uiTotalTime;
	 
	GetFLACRuntime((uint8_t *)_pPath, &uiTotalTime);
	g_tWav.uiTotalTime = flacctrl->totsec ;
	sprintf((char *)g_tWav.usInfo, "%dHzs/%dbits/", flacctrl->samplerate,flacctrl->bps);
	switch(flacctrl->ch)
	{
		case 2:
			strcat((char *)g_tWav.usInfo, "Stereo");
			break;

		case 1:
			strcat((char *)g_tWav.usInfo, "Mono");
			break;
		
		default:
			break;
	}
}

/*
*********************************************************************************************************
*	函 数 名: FLAC_TimeShow
*	功能说明: 用于获取音频当前的播放时间 
*	形    参: _pFile      FIL文件
*             pMP3Control MP3Control_T类型结构体指针变量
*	返 回 值: 无
*********************************************************************************************************
*/

static void FLAC_TimeShow(FIL *_pFile, __flacctrl *flacctrl)
{	
	uint64_t uiPos;
	uint32_t uiTempPos;
	
	/* 获取当前播放位置 */
	uiTempPos = _pFile->fptr;
	uiPos = uiTempPos - flacctrl->datastart;
	
	/* 通过这种方法获得文件的大小 */
	g_tWav.uiCurTime = (uint64_t)(uiPos * g_tWav.uiTotalTime) / (_pFile->fsize -  flacctrl->datastart);
	
	/* 恢复当前正在播放的位置 */
	f_lseek (_pFile, uiTempPos);
}


/*********************************************************************************
*	函 数 名: GetFLACDecoderData
*	功能说明: 解码一帧FLAC音频数据。
*	形    参: pFile         文件系统句柄
*             _pBuf         记录解码一帧FLAC数据输出
********************************************************************************/

void GetFLACDecoderData(FIL *file)
{
	int consumed;
	int i;
  int32_t uiPos;
	uint8_t *pbuf=0;
	uint8_t res=0;
	UINT br;

	if(fc->bps==24)
		res=flac_decode_frame24(fc,flacctrl->FileBuff,flacctrl->bytesleft,(s32*)flacctrl->i2STempBuff);
	else 
		res=flac_decode_frame16(fc,flacctrl->FileBuff,flacctrl->bytesleft,(s16*)flacctrl->i2STempBuff); 
	if(res == 0)
		{
	     if(fc->bps==24)	//24位的时候,数据需要单独处理成16位数据 SAI音频接口的24位音频播放暂时未解决 
			 { 
				 pbuf=flacctrl->i2STempBuff;
				 for(i=0;i<fc->blocksize*4;)
//				 for(i=0;i<fc->blocksize*8;)
				  {
					  flacctrl->i2STempBuff[i++]=pbuf[1];
					  flacctrl->i2STempBuff[i]=pbuf[2];
//            i+=2;
//				    flacctrl->i2STempBuff[i++]=pbuf[0];  //24数据处理成16位数据丢掉一个数据，对音质有影响。
					  i++;
					  pbuf+=4;
				  }					
			 }
	     consumed = fc->gb.index / 8;
	     memcpy(flacctrl->FileBuff, &flacctrl->FileBuff[consumed], flacctrl->bytesleft - consumed);
	     flacctrl->bytesleft=flacctrl->bytesleft-consumed;
		   f_read(file, &flacctrl->FileBuff[flacctrl->bytesleft], consumed, &br);
			 if(br>0)
		   flacctrl->bytesleft+=br;
		}
		else
    {  
		 do
		   {
        f_read(file,flacctrl->FileBuff,fc->max_framesize,&br);//读取最大帧长数据
				if(br==0) break;                                     
	      uiPos=flac_seek_frame(flacctrl->FileBuff,br,fc);		//查找帧
		   }while(uiPos==-1);                           //找到正确的帧???
			 f_lseek(file,file->fptr-fc->max_framesize+uiPos);
	    f_read(file,flacctrl->FileBuff,fc->max_framesize,&br);
      flacctrl->bytesleft=br;		
	  }		 
}



/*
*********************************************************************************************************
*	函 数 名: flac_TimeSet
*	功能说明: 用于设置flac播放位置（快进，快退）
*	形    参: _pFile      FIL文件
*          *flacctrl  __flacctrl 类型结构体指针变量
*	返 回 值: 无
*********************************************************************************************************
*/
void flac_TimeSet(FIL *_pFile, __flacctrl *flacctrl)
{	
	int32_t uiPos;
	u32 br=0; 
	
	uiPos  = (uint64_t)(_pFile->fsize -  flacctrl->datastart) * g_tWav.uiCurTime / g_tWav.uiTotalTime ;
	uiPos = uiPos + flacctrl->datastart;
	f_lseek (_pFile, uiPos);      //偏移到数据开始的地方
  do
		{
      f_read(_pFile,flacctrl->FileBuff,fc->max_framesize,&br);//读取最大帧长数据
			if(br==0) break;
	    uiPos=flac_seek_frame(flacctrl->FileBuff,br,fc);		//查找帧
		}while(uiPos==-1);                           //找到正确的帧头.
	f_lseek(_pFile,_pFile->fptr-fc->max_framesize+uiPos);
	f_read(_pFile,flacctrl->FileBuff,fc->max_framesize,&br);
  flacctrl->bytesleft=br;	
}


/*
*********************************************************************************************************
*	函 数 名: FLACMusicPlay
*	功能说明: 播放FLAC音频
*	形    参: filename  歌曲的路径 
*	返 回 值: 无
*********************************************************************************************************
*/
void FLACMusicPlay(const char *filename) 
	
{
	OS_ERR       err;
	CPU_TS	     ts;
	OS_FLAGS     xResult;
	FRESULT res = FR_NOT_READY; /* 先将其初始化为此参数，避免代码中不必要的检测 */
	FIL fout;
	uint8_t ucFileOpenFlag = 0;
  u32 br=0;
	
	
	
	while(1)
		{
		xResult = OSFlagPend ((OS_FLAG_GRP  *)&FLAG_Music,
							  (OS_FLAGS      )0xFFFE,
							  (OS_TICK       )0,
							  (OS_OPT        )OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
							  (CPU_TS       *)&ts,
							  (OS_ERR       *)&err);
		
		if(err == OS_ERR_NONE)
		{
			switch (xResult)
			{
				/* 如果启用emWin多任务的话，此位是被emWin多任务使用 */
				case MusicTaskReserved_0:
					break;
				
				/* 实现快进快退 */
				case MusicTaskAudioAdjust_1:
					  flac_TimeSet(&fout, flacctrl);
					break;

				/* 暂停 */
				case MusicTaskAudioPause_2:
				    AUDIO_Pause();
					break;

				/* 继续 */
				case MusicTaskAudioResume_3:
				    AUDIO_Resume();
					break;	

				/* 此位暂未使用 */
				case MusicTaskReserved_4:
					break;	
				
				/* 开始播放 */
				case MusicTaskAudioPlay_5:
					/* 获取要播放FLAC文件的相关信息 */
				  GetFLACInfo((uint8_t *)filename);
					
					/* 文件被打开标志设置 */
					ucFileOpenFlag = 1;
					
					/* 打开FLAC音频文件*/
					res = f_open(&fout, (TCHAR*)filename, FA_READ);
					f_lseek(&fout, flacctrl->datastart);
					FLAC_TimeShow(&fout, flacctrl);

					/* 初始化部分变量*/ 
          if(fc->min_blocksize==fc->max_blocksize)//必须min_blocksize等于max_blocksize  &&fc->max_blocksize!=0
			       {
				       flacctrl->i2sbuf0=rt_alloc_mem(SDRAM,fc->max_blocksize*4);
					     flacctrl->i2sbuf1=rt_alloc_mem(SDRAM,fc->max_blocksize*4);									
						   fc->decoded0=rt_alloc_mem(SDRAM,fc->max_blocksize*8); //FLAC解码缓存数组0
				       fc->decoded1=rt_alloc_mem(SDRAM,fc->max_blocksize*8); //FLAC解码缓存数组1
						   flacctrl->i2STempBuff=rt_alloc_mem(SDRAM,fc->max_blocksize*8);
               flacctrl->FileBuff=rt_alloc_mem(SDRAM,fc->max_framesize);
					/*初始化FLAC解码器 */
					 
			        memset(flacctrl->i2sbuf0,0,fc->max_blocksize*4);
			        memset(flacctrl->i2sbuf1,0,fc->max_blocksize*4);
		        }  		 
            f_read(&fout,flacctrl->FileBuff,fc->max_framesize,&br);//读取最大帧长数据	 
						flacctrl->bytesleft=br;
					/* 获取3帧解码好的数据 */
					GetFLACDecoderData(&fout);
					memcpy(flacctrl->i2sbuf0,flacctrl->i2STempBuff, flacctrl->outsamples*2);
					GetFLACDecoderData(&fout);
					memcpy(flacctrl->i2sbuf1,flacctrl->i2STempBuff, flacctrl->outsamples*2);
					GetFLACDecoderData(&fout);
					
					/* 初始化SAI音频接口进行播放 */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, flacctrl->samplerate, 2);
					Play_DMA_Init(flacctrl->i2sbuf0, flacctrl->i2sbuf1, flacctrl->outsamples);
					AUDIO_Play();
					break;	
					
				/* 退出音频播放 */
				case MusicTaskAudioReturn_6:
					AUDIO_Stop();
					if(res == FR_OK)
					{
						f_close (&fout); 
					}
        /*  释放内存  */
					rt_free_mem(SDRAM, flacctrl->i2sbuf0);
					rt_free_mem(SDRAM, flacctrl->i2sbuf1);
					rt_free_mem(SDRAM,fc->decoded0);
					rt_free_mem(SDRAM,fc->decoded1);
					rt_free_mem(SDRAM,flacctrl->i2STempBuff);
          rt_free_mem(SDRAM,flacctrl->FileBuff);
					Flac_Memory(1);
					return;

				/* 获取歌曲播放时间 */
				case MusicTaskAudioGetTime_7:
					if(ucFileOpenFlag == 1)
          FLAC_TimeShow(&fout, flacctrl);
					break;
					
				/* 开始播放，歌曲切换下一曲使用 */
				case MusicTaskAudioStart_8:
					/* 获取要播放FLAC文件的相关信息 */
				 GetFLACInfo((uint8_t *)filename);
				 
					
					/* mp3文件被打开标志设置 */
					ucFileOpenFlag = 1;
					
					/* 打开FLAC音频文件*/
					res = f_open(&fout, (TCHAR*)filename, FA_READ);
          f_lseek(&fout, flacctrl->datastart);
				  FLAC_TimeShow(&fout, flacctrl);	
			
					/* 初始化部分变量*/
          if(fc->min_blocksize==fc->max_blocksize&&fc->max_blocksize!=0)//必须min_blocksize等于max_blocksize
			       {
					     flacctrl->i2sbuf0=rt_alloc_mem(SDRAM,fc->max_blocksize*4);
					     flacctrl->i2sbuf1=rt_alloc_mem(SDRAM,fc->max_blocksize*4); 
						   fc->decoded0=rt_alloc_mem(SDRAM,fc->max_blocksize*8); //FLAC解码缓存数组0
				       fc->decoded1=rt_alloc_mem(SDRAM,fc->max_blocksize*8); //FLAC解码缓存数组1
               flacctrl->i2STempBuff=rt_alloc_mem(SDRAM,fc->max_blocksize*8);
               flacctrl->FileBuff=rt_alloc_mem(SDRAM,fc->max_framesize);
					 /*初始化FLAC解码器 */
			        memset(flacctrl->i2sbuf0,0,fc->max_blocksize*4);
			        memset(flacctrl->i2sbuf1,0,fc->max_blocksize*4);
		        }  		 
	           f_read(&fout,flacctrl->FileBuff,fc->max_framesize,&br);//读取最大帧长数据	 
						 flacctrl->bytesleft=br;

					/* 获取3帧解码好的数据 */
					GetFLACDecoderData(&fout);
					memcpy(flacctrl->i2sbuf0,flacctrl->i2STempBuff, flacctrl->outsamples*2);
					GetFLACDecoderData(&fout);
					memcpy(flacctrl->i2sbuf1,flacctrl->i2STempBuff, flacctrl->outsamples*2);
					GetFLACDecoderData(&fout);
					
					/* 初始化SAI音频接口进行播放 */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, flacctrl->samplerate, 2);
					Play_DMA_Init(flacctrl->i2sbuf0,flacctrl->i2sbuf1, flacctrl->outsamples);
					AUDIO_Play();
					AUDIO_Pause();
					break;
					
				/* 当前使用的是缓冲0，填充缓冲1，并通过64点FFT实行频谱显示 */
				case MusicTaskAudioFillBuffer1_9:
					DSP_FFT64(flacctrl->i2sbuf0);
					GetFLACDecoderData(&fout);
					memcpy(flacctrl->i2sbuf1,flacctrl->i2STempBuff, flacctrl->outsamples*2);
					break;

				/* 当前使用的是缓冲1，填充缓冲0，并通过64点FFT实行频谱显示 */
				case MusicTaskAudioFillBuffer0_10:
					DSP_FFT64(flacctrl->i2sbuf1);
				  GetFLACDecoderData(&fout);
					memcpy(flacctrl->i2sbuf0,flacctrl->i2STempBuff, flacctrl->outsamples*2);
					break;	
				
				/* 后5位暂未使用 */
				default: 
					break;
			}
		}
		
		if(res == FR_OK)
		{
			/* 音频文件读取完毕，这一步主要用于歌曲的自动切换 */
			if(f_eof(&fout) != 0)
			{	
				/* 等待其中一个缓冲完成 */
				xResult = OSFlagPend ((OS_FLAG_GRP  *)&FLAG_Music,
										(OS_FLAGS      )MusicTaskWaitBuffer,
										(OS_TICK       )0,
										(OS_OPT        )OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
										(CPU_TS       *)&ts,
										(OS_ERR       *)&err);	
				
				/* 缓冲清零，防止播放快结束的时候重复播放存储的音频数据 */
				if(xResult & MusicTaskAudioFillBuffer1_9)
				{
					memset(flacctrl->i2sbuf1, 0, fc->max_blocksize*4);
				}
				else
				{
					memset(flacctrl->i2sbuf0, 0, fc->max_blocksize*4);
				}
				
				/* 等待另一个缓冲完成 */
				xResult = OSFlagPend ((OS_FLAG_GRP  *)&FLAG_Music,
									  (OS_FLAGS      )MusicTaskWaitBuffer,
									  (OS_TICK       )0,
									  (OS_OPT        )OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
									  (CPU_TS       *)&ts,
									  (OS_ERR       *)&err);
				
				/* 缓冲清零，防止播放快结束的时候重复播放存储的音频数据 */
				if(xResult & MusicTaskAudioFillBuffer1_9)
				{
					memset(flacctrl->i2sbuf1, 0, fc->max_blocksize*4);
				}
				else
				{
					memset(flacctrl->i2sbuf0, 0, fc->max_blocksize*4);
				}				
				
				/* 等待一次播放时间获取，保证音乐播放的进度条完整显示完毕 */
				OSFlagPend ((OS_FLAG_GRP  *)&FLAG_Music,
							(OS_FLAGS      )MusicTaskAudioGetTime_7,
							(OS_TICK       )0,
							(OS_OPT        )OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
							(CPU_TS       *)&ts,
							(OS_ERR       *)&err);	
				
				/* 等待一次播放时间获取，保证音乐播放的进度条完整显示完毕 */
        FLAC_TimeShow(&fout, flacctrl);
				OSTimeDly(800, OS_OPT_TIME_DLY, &err);
				
				/* 关闭SAI音频接口和MP3文件 */
				AUDIO_Stop();
				
				if(res == FR_OK)
				{
					f_close (&fout); 
				}

				/* 释放相关动态内存 */
          rt_free_mem(SDRAM, flacctrl->i2sbuf0);
					rt_free_mem(SDRAM, flacctrl->i2sbuf1);
					rt_free_mem(SDRAM,fc->decoded0);
					rt_free_mem(SDRAM,fc->decoded1);
					rt_free_mem(SDRAM,flacctrl->i2STempBuff);
          rt_free_mem(SDRAM,flacctrl->FileBuff);
					Flac_Memory(1);
				
				/* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
				WM_SendMessageNoPara(hWinMusic, MSG_NextMusic);	
				return;			
			} 	
		}
	}
	
}

