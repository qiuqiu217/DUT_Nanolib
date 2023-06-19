#include "includes.h"
#include "MainTask.h"	

/********************************************************************************
//说明
//1,支持16位单声道/立体声ape的解码
//2,最高支持44.1K的APE格式(Fast压缩流畅)
//3,对某些ape文件,可能不支持,可用千千静听软件进行转换一下,即可正常播放
//4,ape暂不支持快进，快退。
********************************************************************************/ 	 
 
__apectrl * apectrl;	//APE播放控制结构体
struct ape_ctx_t  *apex; 
extern filter_int *filterbuf64;		//需要2816字节 
int *decoded0;
int *decoded1;
int firstbyte;  	
int currentframe;
int nblocks;
int bytesconsumed;
int bytesinbuffer;
u8 *readptr;

/*
*********************************************************************************************************
*	函 数 名: Ape_Memory
*	功能说明: 专门用于获取FLAC音频运行时的动态内存
*	形    参: FM     0获取，1释放。
*	返 回 值: 0 表示成功， 1表示失败
*********************************************************************************************************
*/
uint8_t Ape_Memory(int8_t FM)
{
	u8 res;
  if(FM==0)   //获取内存
	{		
	 res=rt_init_mem (SDRAM, 4*1024*1024);
	 if(res!=0)return 1;
	 apex=rt_alloc_mem(SDRAM,sizeof(struct ape_ctx_t));
	 apectrl=rt_alloc_mem(SDRAM,sizeof(__apectrl));
	}
	else//释放内存.
	{
	  rt_free_mem(SDRAM, apectrl);
		rt_free_mem(SDRAM, apex);
	}
	return 0;
}
/*
*********************************************************************************************************
*	函 数 名: GetAPERuntime
*	功能说明: 专门用于获取FLAC音频的运行时间，方便emWin显示歌曲列表时时间的显示。
*	形    参: pFileName     文件句柄 
*             uiTotalTime   歌曲执行时间
*	返 回 值: TRUE(1) 表示成功， FALSE(0)表示失败
*********************************************************************************************************
*/
uint32_t GetAPERuntime(uint8_t *pFileName, uint32_t *uiTotalTime)
{
	
	FIL fout_flac;
  u8 res;
  u32 totalsamples; 
	
	/* 获取FLAC相关信息，主要是执行时间 */
	res=Ape_Memory(0);   //申请动态内存
	if(res!=0)return FALSE;
	res=f_open(&fout_flac,(TCHAR*)pFileName,FA_READ); //读取文件错误
  if(res!=0)return FALSE;	
	res=ape_parseheader(&fout_flac,apex);//分析ape文件头 
	apectrl->bps=apex->bps;					//得到的位数(ape,我们仅支持16位)
	apectrl->samplerate=apex->samplerate;	//得到采样率,ape支持48Khz以下的采样率,在高就可能卡了...
	if(apex->totalframes>1)totalsamples=apex->finalframeblocks+apex->blocksperframe*(apex->totalframes-1);
	else totalsamples=apex->finalframeblocks;        //一个声道的总采样数
	apectrl->totsec=totalsamples/apectrl->samplerate;//得到文件总时间
	apectrl->bitrate=(fout_flac.fsize-apex->firstframe)*8/apectrl->totsec;//得到位速
	apectrl->outsamples=APE_BLOCKS_PER_LOOP*2;//PCM输出数据量(16位为单位) 
	apectrl->datastart=apex->firstframe;	//得到第一帧的地址
	apectrl->ch=apex->channels;           //声道
	apectrl->totalframes=apex->totalframes;//总块数
  f_close(&fout_flac);	
	*uiTotalTime = apectrl->totsec ;
	
	return TRUE;	
}
/*
*********************************************************************************************************
*	函 数 名: GetAPEInfoo
*	功能说明: 专门用于获取APE音频的相关信息，方便播放FLAC歌曲时相关信息的显示。
*	形    参: _pPath   文件句柄 
*	返 回 值: 无
*********************************************************************************************************
*/
void GetAPEInfo(uint8_t *_pPath)
{
	uint32_t uiTotalTime;
	 
	GetAPERuntime((uint8_t *)_pPath, &uiTotalTime);
	g_tWav.uiTotalTime = apectrl->totsec ;
	sprintf((char *)g_tWav.usInfo, "%dHzs/%dbits/", apectrl->samplerate,apectrl->bps);
	switch(apectrl->ch)
	{
		case 2:
			strcat((char *)g_tWav.usInfo, "Stereo/");
			break;

		case 1:
			strcat((char *)g_tWav.usInfo, "Mono/");
			break;
		
		default:
			break;
	}
	switch(apex->compressiontype)           
	{
		case 1000:
			strcat((char *)g_tWav.usInfo, "Fast");
			break;
		
		case 2000:
			strcat((char *)g_tWav.usInfo, "Normal");
			break;
		
		case 3000:
			strcat((char *)g_tWav.usInfo, "Higt");
			break;
		
		case 4000:
			strcat((char *)g_tWav.usInfo, "Extra High");
			break;
		
		case 5000:
			strcat((char *)g_tWav.usInfo, "Insane");
			break;
		
		default:
			break;
	}	
}

/*
*********************************************************************************************************
*	函 数 名: APE_TimeShow
*	功能说明: 用于获取音频当前的播放时间 
*	形    参: _pFile      FIL文件
*            flacctrl  __flacctrl 类型结构体指针变量
*	返 回 值: 无
*********************************************************************************************************
*/

static void APE_TimeShow(FIL *_pFile, __apectrl *apectrl)
{	
	uint64_t uiPos;
	uint32_t uiTempPos;
	
	/* 获取当前播放位置 */
	uiTempPos = _pFile->fptr;
	uiPos = uiTempPos - apectrl->datastart;
	
	/* 获取音频当前的播放时间 */
	g_tWav.uiCurTime = (uint64_t)(uiPos * g_tWav.uiTotalTime) / (_pFile->fsize -  apectrl->datastart);
	
	/* 恢复当前正在播放的位置 */
	f_lseek (_pFile, uiTempPos);
}



/*********************************************************************************
*	函 数 名: GetAPEDecoderData
*	功能说明: 解码APE音频数据。
*	形    参: file 文件系统句柄
********************************************************************************/

void GetAPEDecoderData(FIL *file)
{
	 int blockstodecode;
   u8 res;

		if(currentframe<apex->totalframes&&nblocks==0)//还有未解码的帧?
			 {
      //计算一帧里面有多少个blocks?
			  if(currentframe==(apex->totalframes-1))nblocks=apex->finalframeblocks;
			  else nblocks=apex->blocksperframe; 
			  apex->currentframeblocks=nblocks;
			//初始化帧解码
			  init_frame_decoder(apex,readptr,&firstbyte,&bytesconsumed);
			  readptr+=bytesconsumed;
				bytesinbuffer-=bytesconsumed;
       }
			//开始帧解码 
				if(nblocks>0)
				  {					
				    blockstodecode=AUDIO_MIN(APE_BLOCKS_PER_LOOP,nblocks);//获得一次要解码的blocks个数  
				    res=decode_chunk(apex,readptr,&firstbyte,&bytesconsumed,decoded0,decoded1,blockstodecode);
				   if(res!=0)
				     {
						     res=1;
				     }

					 readptr+=bytesconsumed;			  //解码指针偏移到新数据位置
				   bytesinbuffer-=bytesconsumed; 	//buffer里面的数据量减少
           if(bytesconsumed>4*APE_BLOCKS_PER_LOOP)//出现错误了
				      {
					      res=1;								
				      }
					 if(bytesinbuffer<4*APE_BLOCKS_PER_LOOP)//需要补充新数据了
				      { 
					     
								f_lseek(file,file->fptr-bytesinbuffer);
								res=f_read(file,apectrl->i2STempBuff,APE_FILE_BUF_SZ,(u32*)&bytesinbuffer);
					      if(res)//出错了.
					        { 
						        res=1;
					        }								
					      readptr=apectrl->i2STempBuff;
				      }
            nblocks-=blockstodecode;//block计数递减   
            if(nblocks==0)
							currentframe++;								
			    }
    						
}	

/*
*********************************************************************************************************
*	函 数 名: ape_TimeSet
*	功能说明: 用于设置flac播放位置（快进，快退）
*	形    参: _pFile      FIL文件
*	返 回 值: 无
* 暂时不能实现
*********************************************************************************************************
*/
void ape_TimeSet(FIL *_pFile)
{	
	
}



/*
*********************************************************************************************************
*	函 数 名: APEMusicPlay
*	功能说明: 播放APE音频
*	形    参: filename  歌曲的路径 
*	返 回 值: 无
*********************************************************************************************************
*/
void APEMusicPlay(const char *filename) 
{
	OS_ERR       err;
	CPU_TS	     ts;
	OS_FLAGS     xResult;
	FRESULT res = FR_NOT_READY; /* 先将其初始化为此参数，避免代码中不必要的检测 */
	FIL fout;
	uint8_t ucFileOpenFlag = 0;



	
	
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
					  ape_TimeSet(&fout);     //暂时不能实现
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
					/* 获取要播放APE文件的相关信息 */
				  GetAPEInfo((uint8_t *)filename);
					
					/* 文件被打开标志设置 */
					ucFileOpenFlag = 1;
					
					/* 打开APE音频文件*/
					res = f_open(&fout, (TCHAR*)filename, FA_READ);
					f_lseek(&fout, apectrl->datastart);
					APE_TimeShow(&fout, apectrl);

					/* 初始化部分变量*/ 
            filterbuf64=rt_alloc_mem(SDRAM,10240);  
	          decoded0=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4);
	          decoded1=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4); 
	          apectrl->i2sbuf0=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4);
	          apectrl->i2sbuf1=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4);
            apectrl->i2STempBuff=rt_alloc_mem(SDRAM,APE_FILE_BUF_SZ);  				
					/*初始化APE解码器 */
			        memset(apectrl->i2sbuf0,0,APE_BLOCKS_PER_LOOP*4);
			        memset(apectrl->i2sbuf1,0,APE_BLOCKS_PER_LOOP*4);
							
		      f_read(&fout,apectrl->i2STempBuff,APE_FILE_BUF_SZ,(u32*)&bytesinbuffer);
          readptr=apectrl->i2STempBuff;
          currentframe=0;
          firstbyte=3;  		//Take account of the little-endian 32-bit byte ordering					
					/* 获取3帧解码好的数据 */
					GetAPEDecoderData(&fout);
					memcpy(apectrl->i2sbuf0,decoded1,APE_BLOCKS_PER_LOOP*4);
					GetAPEDecoderData(&fout);
					memcpy(apectrl->i2sbuf1,decoded1,APE_BLOCKS_PER_LOOP*4);
					GetAPEDecoderData(&fout);
					
					/* 初始化SAI音频接口进行播放 */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, apectrl->samplerate, 2);
					Play_DMA_Init(apectrl->i2sbuf0, apectrl->i2sbuf1, apectrl->outsamples);
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
					rt_free_mem(SDRAM, apectrl->i2sbuf0);
					rt_free_mem(SDRAM, apectrl->i2sbuf1);
					rt_free_mem(SDRAM,decoded0);
					rt_free_mem(SDRAM,decoded1);
					rt_free_mem(SDRAM,filterbuf64);
					rt_free_mem(SDRAM,apectrl->i2STempBuff);
					Ape_Memory(1);
					nblocks=0;
					return;

				/* 获取歌曲播放时间 */
				case MusicTaskAudioGetTime_7:
					if(ucFileOpenFlag == 1)
          APE_TimeShow(&fout, apectrl);
					break;
					
				/* 开始播放，歌曲切换下一曲使用 */
				case MusicTaskAudioStart_8:
					/* 获取要播放FLAC文件的相关信息 */
				  GetAPEInfo((uint8_t *)filename);
				 
					
					/* mp3文件被打开标志设置 */
					ucFileOpenFlag = 1;
					
					/* 打开FLAC音频文件*/
					res = f_open(&fout, (TCHAR*)filename, FA_READ);
          f_lseek(&fout, apectrl->datastart);
				  APE_TimeShow(&fout, apectrl);	
			
					/* 初始化部分变量*/
            filterbuf64=rt_alloc_mem(SDRAM,10240);  
	          decoded0=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4);
	          decoded1=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4); 
	          apectrl->i2sbuf0=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4);
	          apectrl->i2sbuf1=rt_alloc_mem(SDRAM,APE_BLOCKS_PER_LOOP*4);
            apectrl->i2STempBuff=rt_alloc_mem(SDRAM,APE_FILE_BUF_SZ); 				
					/*初始化APE解码器 */
			        memset(apectrl->i2sbuf0,0,APE_BLOCKS_PER_LOOP*4);
			        memset(apectrl->i2sbuf1,0,APE_BLOCKS_PER_LOOP*4);
							
					f_read(&fout,apectrl->i2STempBuff,APE_FILE_BUF_SZ,(u32*)&bytesinbuffer);
          readptr=apectrl->i2STempBuff;
					currentframe=0;
				  firstbyte=3;  		//Take account of the little-endian 32-bit byte ordering
					/* 获取3帧解码好的数据 */
					GetAPEDecoderData(&fout);
					memcpy(apectrl->i2sbuf0,decoded1,APE_BLOCKS_PER_LOOP*4);
					GetAPEDecoderData(&fout);
					memcpy(apectrl->i2sbuf1,decoded1,APE_BLOCKS_PER_LOOP*4);
					GetAPEDecoderData(&fout);
					
					/* 初始化SAI音频接口进行播放 */
					AUDIO_Init(1, I2S_Standard_Phillips, SAI_DataSize_16b, apectrl->samplerate, 2);
					Play_DMA_Init(apectrl->i2sbuf0,apectrl->i2sbuf1, apectrl->outsamples);
					AUDIO_Play();
					AUDIO_Pause();
					break;
					
				/* 当前使用的是缓冲0，填充缓冲1，并通过64点FFT实行频谱显示 */
				case MusicTaskAudioFillBuffer1_9:
					DSP_FFT64(apectrl->i2sbuf0);
					GetAPEDecoderData(&fout);
					memcpy(apectrl->i2sbuf1,decoded1,APE_BLOCKS_PER_LOOP*4);
					break;

				/* 当前使用的是缓冲1，填充缓冲0，并通过64点FFT实行频谱显示 */
				case MusicTaskAudioFillBuffer0_10:
					DSP_FFT64(apectrl->i2sbuf1);
					GetAPEDecoderData(&fout);
					memcpy(apectrl->i2sbuf0,decoded1,APE_BLOCKS_PER_LOOP*4);
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
					memset(apectrl->i2sbuf1, 0, APE_BLOCKS_PER_LOOP*4);
				}
				else
				{
					memset(apectrl->i2sbuf0, 0, APE_BLOCKS_PER_LOOP*4);
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
					memset(apectrl->i2sbuf1, 0, APE_BLOCKS_PER_LOOP*4);
				}
				else
				{
					memset(apectrl->i2sbuf0, 0, APE_BLOCKS_PER_LOOP*4);
				}				
				
				/* 等待一次播放时间获取，保证音乐播放的进度条完整显示完毕 */
				OSFlagPend ((OS_FLAG_GRP  *)&FLAG_Music,
							(OS_FLAGS      )MusicTaskAudioGetTime_7,
							(OS_TICK       )0,
							(OS_OPT        )OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
							(CPU_TS       *)&ts,
							(OS_ERR       *)&err);	
				
				/* 等待一次播放时间获取，保证音乐播放的进度条完整显示完毕 */
        APE_TimeShow(&fout, apectrl);
				OSTimeDly(800, OS_OPT_TIME_DLY, &err);
				
				/* 关闭SAI音频接口和MP3文件 */
				AUDIO_Stop();
				
				if(res == FR_OK)
				{
					f_close (&fout); 
				}

				/* 释放相关动态内存 */
          rt_free_mem(SDRAM, apectrl->i2sbuf0);
					rt_free_mem(SDRAM, apectrl->i2sbuf1);
					rt_free_mem(SDRAM,decoded0);
					rt_free_mem(SDRAM,decoded1);
					rt_free_mem(SDRAM,filterbuf64);
				  rt_free_mem(SDRAM,apectrl->i2STempBuff);
					Ape_Memory(1);
				  nblocks=0;
				
				/* 给emWin的音乐播放器发消息，切换到下一首歌曲播放 */
				WM_SendMessageNoPara(hWinMusic, MSG_NextMusic);	
				return;			
			} 	
		}
	}
	
}


