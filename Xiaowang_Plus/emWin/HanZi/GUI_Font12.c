/*
*********************************************************************************************************
*	                                  
*	模块名称 : 12点阵字体
*	文件名称 : GUI_Font12.c
*	版    本 : V1.0
*	说    明 : 这种汉字显示方式适合使用等宽字体，非等宽的微软雅黑等字体显示效果较差。
*              
*	修改记录 :
*		版本号   日期         作者          说明
*		V1.0    2016-07-16   Eric2013  	    首版    
*                                     
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "GUI.h"
#include "GUI_Type.h"



GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ12_CharInfo[2] =  
{     
    {  6,    6,  1, (void *)"A12"},   /* 字符 */     
    {  12,  12,  2, (void *)"H12"},   /* 汉字 */        
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ12_PropHZ= { 
      0xA1A1,  
      0xFEFE,  
      &GUI_FontHZ12_CharInfo[1], 
      (void *)0,  
}; 

GUI_CONST_STORAGE  GUI_FONT_PROP GUI_FontHZ12_PropASC= { 
      0x0020,  
      0x007F,  
      &GUI_FontHZ12_CharInfo[0], 
      (void GUI_CONST_STORAGE *)&GUI_FontHZ12_PropHZ,  
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ12 =  
{ 
	  GUI_FONTTYPE_USER,
      12,  
      12,  
      1,   
      1,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ12_PropASC 
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ12x2 =  
{ 
	  GUI_FONTTYPE_USER,
      12,  
      12,  
      2,   
      2,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ12_PropASC 
}; 

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
