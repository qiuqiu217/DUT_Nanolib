/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI所需的系统函数
*	文件名称 : App_SysFunction.c
*	版    本 : V1.1
*	说    明 : 图片显示和保存的API
*	修改记录 :
*		版本号    日期          作者         说明
*		V1.0    2013-03-26    Eric2013       首发
*       V1.1    2014-05-23    Eric2013       删除此文件中多余的函数，只保留截图和BMP图片加载
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "GUI_Main.h"
#include "ff_gen_drv.h"
#include "sd_diskio_dma.h"



/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_libdbg printf
#else
	#define printf_libdbg(...)
#endif


/*
*********************************************************************************************************
*	                                         用于SD卡
*********************************************************************************************************
*/
FRESULT result;
FIL file;
FIL FileSave;   //专门用于图片的保存
DIR DirInf;
UINT bw;
FATFS fs;
char DiskPath[4]; /* SD卡逻辑驱动路径，比盘符0，就是"0:/" */


/* FatFs API的返回值 */
const char * FR_Table[]= 
{
	"FR_OK：成功",				                             /* (0) Succeeded */
	"FR_DISK_ERR：底层硬件错误",			                 /* (1) A hard error occurred in the low level disk I/O layer */
	"FR_INT_ERR：断言失败",				                     /* (2) Assertion failed */
	"FR_NOT_READY：物理驱动没有工作",			             /* (3) The physical drive cannot work */
	"FR_NO_FILE：文件不存在",				                 /* (4) Could not find the file */
	"FR_NO_PATH：路径不存在",				                 /* (5) Could not find the path */
	"FR_INVALID_NAME：无效文件名",		                     /* (6) The path name format is invalid */
	"FR_DENIED：由于禁止访问或者目录已满访问被拒绝",         /* (7) Access denied due to prohibited access or directory full */
	"FR_EXIST：文件已经存在",			                     /* (8) Access denied due to prohibited access */
	"FR_INVALID_OBJECT：文件或者目录对象无效",		         /* (9) The file/directory object is invalid */
	"FR_WRITE_PROTECTED：物理驱动被写保护",		             /* (10) The physical drive is write protected */
	"FR_INVALID_DRIVE：逻辑驱动号无效",		                 /* (11) The logical drive number is invalid */
	"FR_NOT_ENABLED：卷中无工作区",			                 /* (12) The volume has no work area */
	"FR_NO_FILESYSTEM：没有有效的FAT卷",		             /* (13) There is no valid FAT volume */
	"FR_MKFS_ABORTED：由于参数错误f_mkfs()被终止",	         /* (14) The f_mkfs() aborted due to any parameter error */
	"FR_TIMEOUT：在规定的时间内无法获得访问卷的许可",		 /* (15) Could not get a grant to access the volume within defined period */
	"FR_LOCKED：由于文件共享策略操作被拒绝",				 /* (16) The operation is rejected according to the file sharing policy */
	"FR_NOT_ENOUGH_CORE：无法分配长文件名工作区",		     /* (17) LFN working buffer could not be allocated */
	"FR_TOO_MANY_OPEN_FILES：当前打开的文件数大于_FS_SHARE", /* (18) Number of open files > _FS_SHARE */
	"FR_INVALID_PARAMETER：参数无效"	                     /* (19) Given parameter is invalid */
};

/*
*********************************************************************************************************
*	函 数 名: MountSD
*	功能说明: SD卡的挂载及其容量获取。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MountSD(void)
{
	/* 注册SD卡驱动 */
	FATFS_LinkDriver(&SD_Driver, DiskPath);

	/* 挂载文件系统 */
	result = f_mount(&fs, DiskPath, 0);	/* Mount a logical drive */
	if (result != FR_OK)
	{
		printf_libdbg("挂载文件系统失败 (%s)\r\n", FR_Table[result]);
	}
}

/*
*********************************************************************************************************
*	函 数 名: _WriteByte2File()
*	功能说明: 写文件到SD卡或者其他存储介质
*	形    参：Data 要写的数据， p 指向FIL类型变量      	
*	返 回 值: 无
*********************************************************************************************************
*/
void _WriteByte2File(U8 Data, void * p) 
{
	result = f_write (p, &Data, 1, &bw);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
