#ifndef _IO_DRIVER_H
#define _IO_DRIVER_H

/*
	P00	串口0发送
	P01	串口0接收
	P02	SCL
	P03	SDA
	P04	SCK
	P05	MISO
	P06	MOSI
	P07	CS
	P08	串口1发送
	P09	串口1接收
	p014	下载
	　	　
	P015	从控制器 串口0 TXD返回
	P016	从控制器 串口0 RXD返回
	P011	语音芯片 busy检测
	P012	串口0  4052控制
	P013	串口0  4052控制
	P017	串口1  4052控制
	P018	串口1  4052控制
	P010	gprs 电源控制
	P021	路由器电源预留控制
	P022	打印电源控制
	P023	IC卡电源控制
	P025	硬币电源控制
	P026	gps电源控制
	P027	纸币电源控制
	P030	状态灯
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
