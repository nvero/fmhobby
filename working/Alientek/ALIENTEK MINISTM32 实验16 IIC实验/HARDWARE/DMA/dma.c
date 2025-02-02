#include "dma.h"
//Mini STM32开发板
//DMA 驱动代码			 
//正点原子@ALIENTEK
//2010/6/8	

u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度 		    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量  
void MYDMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	u32 DR_Base;  //做缓冲用,不知道为什么.非要不可
	RCC->AHBENR|=1<<0;//开启DMA1时钟
	DR_Base=cpar;
	DMA_CHx->CPAR=DR_Base; 	 //DMA1 外设地址 
	DMA_CHx->CMAR=(u32)cmar; //DMA1,存储器地址
	DMA1_MEM_LEN=cndtr;      //保存DMA传输数据量
	DMA_CHx->CNDTR=cndtr;    //DMA1,传输数据量
	DMA_CHx->CCR=0X00000000;//复位
	DMA_CHx->CCR|=1<<4;  //从存储器读
	DMA_CHx->CCR|=0<<5;  //普通模式
	DMA_CHx->CCR|=0<<6;  //外设地址非增量模式
	DMA_CHx->CCR|=1<<7;  //存储器增量模式
	DMA_CHx->CCR|=0<<8;  //外设数据宽度为8位
	DMA_CHx->CCR|=0<<10; //存储器数据宽度8位
	DMA_CHx->CCR|=1<<12; //中等优先级
	DMA_CHx->CCR|=0<<14; //非存储器到存储器模式		  	
} 
//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
	DMA_CHx->CCR&=~(1<<0);       //关闭DMA传输 
	DMA_CHx->CNDTR=DMA1_MEM_LEN; //DMA1,传输数据量 
	DMA_CHx->CCR|=1<<0;          //开启DMA传输
}	  

 

























