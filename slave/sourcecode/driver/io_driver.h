#ifndef _IO_DRIVER_H
#define _IO_DRIVER_H

/*
	P00	����0����
	P01	����0����
	P02	SCL
	P03	SDA
	P04	SCK
	P05	MISO
	P06	MOSI
	P07	CS
	P08	����1����
	P09	����1����
	p014	����
	��	��
	P015	�ӿ����� ����0 TXD����
	P016	�ӿ����� ����0 RXD����
	P011	����оƬ busy���
	P012	����0  4052����
	P013	����0  4052����
	P017	����1  4052����
	P018	����1  4052����
	P010	gprs ��Դ����
	P021	·������ԴԤ������
	P022	��ӡ��Դ����
	P023	IC����Դ����
	P025	Ӳ�ҵ�Դ����
	P026	gps��Դ����
	P027	ֽ�ҵ�Դ����
	P030	״̬��
*/

#define UART0_A					(1<<12)
#define UART0_B					(1<<13)

#define UART1_A					(1<<17)
#define UART1_B					(1<<18)

#define SYN6288_BUSY				(1<<11)

#define POWER_CONTROL_GPRS				(1<<10)
#define POWER_CONTROL_RESERVED			(1<<21)
#define	POWER_CONTROL_PRINT_MACHINE		(1<<22)
#define POWER_CONTROL_IC_MACHINE		(1<<23)
#define POWER_CONTROL_COIN_MACHINE		(1<<25)
#define POWER_CONTROL_GPS				(1<<26)
#define POWER_CONTROL_NOTE_MACHINE		(1<<27)

#define SYS_STATE_LEN			(1<<30)

#define SetUart0ChannalControlA(c)		(c?(IO0SET|=UART0_A):(IO0CLR|=UART0_A))
#define SetUart0ChannalControlB(c)		(c?(IO0SET|=UART0_B):(IO0CLR|=UART0_B))
#define SetUart1ChannalControlA(c)		(c?(IO0SET|=UART1_A):(IO0CLR|=UART1_A))
#define SetUart1ChannalControlB(c)		(c?(IO0SET|=UART1_B):(IO0CLR|=UART1_B))


extern uint8  IoInit(void);
extern void Io2Exception(void);


#endif
