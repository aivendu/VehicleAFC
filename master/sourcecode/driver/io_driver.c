#include "includes.h"


#define SPI0_DATA_BIT_NUM			8

#define CPHA						(1<<3)		//	��λ����,0--��һ�����ز���, 1--�ڶ������ز���
#define CPOL						(1<<4)		//	ʱ�Ӽ���,0--�͵�ƽ��Ч,1--�ߵ�ƽ��Ч
#define LSBF						(1<<6)		//	λ��,0--��λ�ȷ�,1--��λ�ȷ�,
#define ABRT						(1<<3)		//	�ӻ���ֹ
#define MODF						(1<<4)		//	ģʽ����
#define ROVR						(1<<5)		//	�����
#define WCOL						(1<<6) 		//	д��ͻ

// io�ж�
uint8 spi0_send_data, spi0_rec_data;
uint8 spi0_ctr_var, spi0_state;
OS_EVENT *spi0_mbox;

uint8  IoInit(void)
{
	/*	P00 ��GPRSͨ�Ŵ���0����
		P01 ��GPRSͨ�Ŵ���0����
		P02 SCL
		P03 SDA
		P04 SCK
		P05 MISO
		P06 MOSI
		P07 CS
		P08 ��PADͨ�Ŵ���1����
		P09 ��PADͨ�Ŵ���1����
		P014	����
		��	��
		P015	RJ45 CTS
		P016	RJ45 RTS
		P017	RJ45 ��λ
		P030	״̬��
	*/
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2 = PINSEL2 & (~(0x01 << 3));

	PINSEL0 |= 0x00000005;	//	����UART0 ����
	PINSEL0 |= 0x00000050;	//	����I2C0 ����
	PINSEL0 |= 0x00005500;	//	����SPI0 �ڹ���
	PINSEL0 |= 0x00050000;	//	����UART1 ����
	PINSEL0 |= 0x00000000;  //	����RJ45 ��������CTS
	PINSEL1 |= 0x00000000;	//	����RJ45 ��������RTC
	PINSEL1 |= 0x00000000;	//	����״̬LED ��������
	IO0DIR  |= RJ45_CTS;	//	�����������RJ45 ��CTS
	IO0DIR  |= RJ45_RTS;	//	�����������RJ45 ��RTC
	IO0DIR  |= RJ45_RST;	//	�����������RJ45 ��reset
	IO0DIR  |= STATE_LED;	//	�����������LED


#if (SPI0_INT_EN == 0)
	//	SPI0ģ��˿ڷ�������
	PINSEL0 &= (~0x00005500);	//	����SPI0 �ڹ���
	IO0DIR	|=	SPI0_SCK;		//	����ʱ�����
	IO0DIR	&=	(~SPI0_MISO);	//	��������
	IO0DIR	|=	SPI0_MOSI;		//	�������
	IO0DIR	|=	SPI0_CS;		//	����Ƭѡ���
#endif

	return 1;

}


