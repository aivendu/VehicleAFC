#include "includes.h"


#define SPI0_DATA_BIT_NUM			8

#define CPHA						(1<<3)		//	相位控制,0--第一个边沿采样, 1--第二个边沿采样
#define CPOL						(1<<4)		//	时钟极性,0--低电平有效,1--高电平有效
#define LSBF						(1<<6)		//	位序,0--高位先发,1--低位先发,
#define ABRT						(1<<3)		//	从机终止
#define MODF						(1<<4)		//	模式错误
#define ROVR						(1<<5)		//	读溢出
#define WCOL						(1<<6) 		//	写冲突

// io中断
uint8 spi0_send_data, spi0_rec_data;
uint8 spi0_ctr_var, spi0_state;
OS_EVENT *spi0_mbox;

uint8  IoInit(void)
{
	/*	P00 与GPRS通信串口0发送
		P01 与GPRS通信串口0接收
		P02 SCL
		P03 SDA
		P04 SCK
		P05 MISO
		P06 MOSI
		P07 CS
		P08 与PAD通信串口1发送
		P09 与PAD通信串口1接收
		P014	下载
		　	　
		P015	RJ45 CTS
		P016	RJ45 RTS
		P017	RJ45 复位
		P030	状态灯
	*/
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2 = PINSEL2 & (~(0x01 << 3));

	PINSEL0 |= 0x00000005;	//	配置UART0 功能
	PINSEL0 |= 0x00000050;	//	配置I2C0 功能
	PINSEL0 |= 0x00005500;	//	配置SPI0 口功能
	PINSEL0 |= 0x00050000;	//	配置UART1 功能
	PINSEL0 |= 0x00000000;  //	配置RJ45 功能配置CTS
	PINSEL1 |= 0x00000000;	//	配置RJ45 功能配置RTC
	PINSEL1 |= 0x00000000;	//	配置状态LED 功能配置
	IO0DIR  |= RJ45_CTS;	//	设置输出控制RJ45 的CTS
	IO0DIR  |= RJ45_RTS;	//	设置输出控制RJ45 的RTC
	IO0DIR  |= RJ45_RST;	//	设置输出控制RJ45 的reset
	IO0DIR  |= STATE_LED;	//	设置输出控制LED


#if (SPI0_INT_EN == 0)
	//	SPI0模拟端口方向设置
	PINSEL0 &= (~0x00005500);	//	配置SPI0 口功能
	IO0DIR	|=	SPI0_SCK;		//	主机时钟输出
	IO0DIR	&=	(~SPI0_MISO);	//	主机输入
	IO0DIR	|=	SPI0_MOSI;		//	主机输出
	IO0DIR	|=	SPI0_CS;		//	主机片选输出
#endif

	return 1;

}


