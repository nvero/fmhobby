#ifndef __PS2_H
#define __PS2_H	 
#include "delay.h"	   
#include "sys.h"
//Mini STM32开发板
//PS2 驱动代码 
//正点原子@ALIENTEK
//2010/6/17
				   
//物理接口定义
//PS2输入 		  				    
#define PS2_SCL PAin(15)	//PA15
#define PS2_SDA PAin(13)	//PA13
//PS2输出
#define PS2_SCL_OUT PAout(15)	//PA15
#define PS2_SDA_OUT PAout(13)	//PA13

//设置PS2_SCL输入输出状态.		  
#define PS2_SET_SCL_IN()  {GPIOA->CRH&=0X0FFFFFFF;GPIOA->CRH|=0X80000000;}
#define PS2_SET_SCL_OUT() {GPIOA->CRH&=0X0FFFFFFF;GPIOA->CRH|=0X30000000;}	  
//设置PS2_SDA输入输出状态.		  
#define PS2_SET_SDA_IN()  {GPIOA->CRH&=0XFF0FFFFF;GPIOA->CRH|=0X00800000;}
#define PS2_SET_SDA_OUT() {GPIOA->CRH&=0XFF0FFFFF;GPIOA->CRH|=0X00300000;} 

#define MOUSE    0X20 //鼠标模式
#define KEYBOARD 0X10 //键盘模式
#define CMDMODE  0X00 //发送命令
//PS2_Status当前状态标志
//[5:4]:当前工作的模式;[7]:接收到一次数据
//[6]:校验错误;[3:0]:收到的数据长度;	 
extern u8 PS2_Status;       //定义为命令模式
extern u8 PS2_DATA_BUF[16]; //ps2数据缓存区
extern u8 MOUSE_ID;

void PS2_Init(void);
u8 PS2_Send_Cmd(u8 cmd);
void PS2_Set_Int(u8 en);
u8 PS2_Get_Byte(void);
void PS2_En_Data_Report(void);  
void PS2_Dis_Data_Report(void);		  				    
#endif





















