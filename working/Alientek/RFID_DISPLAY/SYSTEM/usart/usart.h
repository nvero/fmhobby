#ifndef __USART_H
#define __USART_H
#include <stm32f10x_lib.h>
#include "stdio.h"	 
//Mini STM32������
//����1��ʼ��			   
//����ԭ��@ALIENTEK
//2010/5/27
	  	
extern u8 USART_RX_BUF[64];     //���ջ���,���63���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART_RX_STA;         //����״̬���	

extern u8 USART2_RX_BUF[64];     //���ջ���,���63���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART2_RX_STA;         //����״̬���	

#define EN_USART1_RX 1
#define EN_USART2_RX 1

void uart_init(u32 pclk2,u32 bound);
void uart_init2(u32 pclk2,u32 bound);
#endif	   















