#include <stm32f10x_lib.h>
#include "adc.h"
#include "delay.h"
//Mini STM32开发板
//ADC 驱动代码			 
//正点原子@ALIENTEK
//2010/6/7
		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{    
	//先初始化IO口
 	RCC->APB2ENR|=1<<2;    //使能PORTA口时钟 
	GPIOA->CRL&=0XFFFF0000;//PA0 1 2 3 anolog输入
	//通道10/11设置			 
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降! 
	RCC->CFGR|=2<<14;      	 
	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
	ADC1->CR1&=~(1<<8);    //非扫描模式
						
	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐
	ADC1->CR2|=1<<23;      //使能温度传感器

	ADC1->SQR1&=~(0XF<<20);
	ADC1->SQR1&=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 			   
	//设置通道0~3的采样时间
	ADC1->SMPR2&=0XFFFFF000;//通道0,1,2,3采样时间清空	  
	ADC1->SMPR2|=7<<9;      //通道3  239.5周期,提高采样时间可以提高精确度	 
	ADC1->SMPR2|=7<<6;      //通道2  239.5周期,提高采样时间可以提高精确度	 
	ADC1->SMPR2|=7<<3;      //通道1  239.5周期,提高采样时间可以提高精确度	 
	ADC1->SMPR2|=7<<0;      //通道0  239.5周期,提高采样时间可以提高精确度
		 
	ADC1->SMPR1&=~(7<<18);  //清除通道16原来的设置	 
	ADC1->SMPR1|=7<<18;     //通道16  239.5周期,提高采样时间可以提高精确度	 

	ADC1->CR2|=1<<0;	    //开启AD转换器	 
	ADC1->CR2|=1<<3;        //使能复位校准  
	while(ADC1->CR2&1<<3);  //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  
}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
	//设置转换序列	  		 
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=ch;		  			    
	ADC1->CR2|=1<<22;       //启动规则转换通道 
	while(!(ADC1->SR&1<<1));//等待转换结束	 	   
	return ADC1->DR;		//返回adc值	
}
//得到ADC采样内部温度传感器的值
//取10次,然后平均
u16 Get_Temp(void)
{
	u16 temp_val=0;
	u8 t;
	for(t=0;t<10;t++)
	{
		temp_val+=Get_Adc(TEMP_CH);
		delay_ms(5);
	}
	return temp_val/10;
}
		   


























