#include "includes.h"


void DacInit(void)
{
	PINSEL1 |= ((1 << 19) + (1 << 18));	//	����IO����ΪDAC�����
	IO1DIR = IO1DIR & (~(1 << 16));	//	����IO����
	DACR |= (1 << 16);				//	��ʱ2.5ns

}

void DacOut(uint16 val)
{
	DACR &= 0xffff003f;				//	������
	DACR |= (val << 7);				//	���DAֵ
}



