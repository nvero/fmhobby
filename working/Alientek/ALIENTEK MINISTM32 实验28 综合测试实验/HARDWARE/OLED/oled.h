#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"
#include "stdlib.h"	    
//SSD1306 OLED 驱动IC驱动代码
//驱动方式:8080并口/4线串口
//版本:V1.1
//正点原子@ALIENTEK
//2010-6-3

//OLED模式设置
//0:4线串行模式
//1:并行8080模式
#define OLED_MODE 1
		    						  
//-----------------OLED端口定义----------------  					   
#define OLED_CS PCout(9)
//#define OLED_RST  PBout(14)//在MINISTM32上直接接到了STM32的复位脚！	
#define OLED_RS PCout(8)
#define OLED_WR PCout(7)		  
#define OLED_RD PCout(6)

//PB0~7,作为数据线
#define DATAOUT(x) GPIOB->ODR=(GPIOB->ODR&0xff00)|(x&0x00FF); //输出
  
//使用4线串行接口时使用 
#define OLED_SCLK PBout(0)
#define OLED_SDIN PBout(1)
		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
//OLED控制用函数
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y,const u8 *p);	 
#endif  
	 



