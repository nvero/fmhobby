#ifndef __TEXT_H__
#define __TEXT_H__	 
#include <stm32f10x_lib.h>				    
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//文本显示程序 驱动代码		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/5/23 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////	 

					    	    
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);//得到汉字的点阵码
void Show_Font(u8 x,u16 y,u8 *font,u8 size,u8 mode);//在指定位置显示一个汉字
void Show_Str(u8 x,u16 y,u8*str,u8 size,u8 mode);//在指定位置显示一个字符串 
void Show_Str_Mid(u8 x,u16 y,u8*str,u8 size,u8 len);
void Show_PTStr(u8 x,u8 y,u8*str,u8 size,u8 mode,u8 len,u16 offx);//显示部分字符
void Str_Scroll(u8 x,u8 y,u8*str,u8 size,u8 mode,u8 len,u8 start);//滚动字符串
void my_stradd(u8*str1,u8*str2);//将str2与str1相加,结果保存在str1


#endif
