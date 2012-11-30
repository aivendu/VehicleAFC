#include "includes.h"


void DacInit(void)
{
	PINSEL1 |= ((1 << 19) + (1 << 18));	//	定义IO功能为DAC输出。
	IO1DIR = IO1DIR & (~(1 << 16));	//	定义IO方向
	DACR |= (1 << 16);				//	延时2.5ns

}

void DacOut(uint16 val)
{
	DACR &= 0xffff003f;				//	清除输出
	DACR |= (val << 7);				//	输出DA值
}



