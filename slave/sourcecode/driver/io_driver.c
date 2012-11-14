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
uint8  IoInit(void)
{
/*	
	P00	串口0发送
	P01	串口0接收
	P02	SCL
	P03	SDA
	P04	SCK        模拟
	P05	MISO	模拟
	P06	MOSI	模拟
	P07	CS		模拟
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
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2=PINSEL2&(~(0x01<<3));

	/*	特殊功能配置	*/
	PINSEL0 |= 0x00000005;	//	配置UART0 功能
	PINSEL0 |= 0x00000050;	//	配置I2C0 功能
	PINSEL0 |= 0x00005500;	//	配置SPI0 口功能
	PINSEL0 |= 0x00050000;	//	配置UART1 功能
	//PINSEL0 |= 0x00000000;  //	配置RJ45 功能配置CTS
	//PINSEL1 |= 0x00000000;	//	配置RJ45 功能配置RTC
	//PINSEL1 |= 0x00000000;	//	配置状态LED 功能配置

	/*	GPIO 方向配置	*/
	IO0DIR	&=	(~SYN6288_BUSY);			//	语音合成芯片工作状态输入口

	IO0DIR	|=	UART0_A;					//	uart0 通道控制输出
	IO0DIR	|=	UART0_B;					//	uart0 通道控制输出

	IO0DIR	|=	UART1_A;					//	uart1 通道控制输出
	IO0DIR	|=	UART1_B;					//	uart1 通道控制输出
	
	IO0DIR	|=	POWER_CONTROL_GPRS;					//	GPRS 模块电源 控制输出
	IO0DIR	|=	POWER_CONTROL_RESERVED;				//	预留的12V 控制输出
	IO0DIR	|=	POWER_CONTROL_PRINT_MACHINE;		//	打印机电源控制输出
	IO0DIR	|=	POWER_CONTROL_IC_MACHINE;			//	接触式IC卡读卡器电源控制输出
	IO0DIR	|=	POWER_CONTROL_COIN_MACHINE;			//	硬币找零机电源控制输出
	IO0DIR	|=	POWER_CONTROL_GPS;					//	GPS 模块电源控制输出
	IO0DIR	|=	POWER_CONTROL_NOTE_MACHINE;			//	纸币找零机电源控制输出
	
	IO0DIR	|=	SYS_STATE_LEN;						//	系统状态等输出口
	
	//	初始化关闭电源和点亮状态灯
	IO0CLR |= POWER_CONTROL_GPRS;			
	IO0CLR |= POWER_CONTROL_RESERVED;		
	IO0CLR |= POWER_CONTROL_PRINT_MACHINE;
	IO0CLR |= POWER_CONTROL_IC_MACHINE;	
	IO0CLR |= POWER_CONTROL_COIN_MACHINE;	
	IO0CLR |= POWER_CONTROL_GPS;			
	IO0CLR |= POWER_CONTROL_NOTE_MACHINE;	
	IO0CLR |= SYS_STATE_LEN;

	
#if (SPI0_MODE == 1)	
	/*	配置模拟SPI从机模式
		IO0.4 -- SCK			输入
		IO0.5 -- MOSI		输入
		IO0.6 -- MISO		输出
		IO0.7 -- SSL	定义为中断
	*/
	PINSEL0 &= (~0x0000FF00);	//	从新配置SPI0口
	PINSEL0 |= 0x0000C000;	//	配置SPI0 口功能
	IO0DIR	&= (~SPI0_SCK);		//	从机输入	
	IO0DIR	&= (~SPI0_MOSI);	//	从机输入
	IO0DIR	|= SPI0_MISO;		//	从机输出
	EXTMODE |= (1<<2);			//	边沿触发
	EXTPOLAR &= (~(1<<2));		//	下降沿触发
	EXTINT = 0x04;
#endif


	return 1;
	
}
uint8 bit_temp;

void Io2Exception(void) {
	uint32 temp,temp_old;				//	IO口数据
	uint16 rec_temp=0;					//	接收缓存
	uint8  send_temp=spi0_send_data;	//	发送缓存
	//uint8  send_temp=0xAA;
	uint8  ctr_var=SPI0_COMM_CONFIG;		//	控制字锁存
	uint8  spi0_comm_bit_num = 0;		//	接收位数
	
	OS_ENTER_CRITICAL();

	temp = IO0PIN;
	temp_old = temp;
	if ((temp & SPI0_CS) == 0) {
		if (ctr_var & CPHA) {				//	相位控制，采样在第二个时钟边沿
			if (ctr_var & CPOL) {			//	时钟在帧传输之间保持高电平
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	下降沿
						//	下降沿移出数据
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	低位先传
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;		//	移出最低位数据
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	高位先传
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;		//	移出最高位数据
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					else if ((temp & SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	上升沿
						//	上升沿采样数据
						if (spi0_comm_bit_num < 8) {			//	数据未接收完成
							if (ctr_var & LSBF) {		//	低位先传
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	收到1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	收到0
							}
							else {							//	高位先传
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	收到1
								else 					rec_temp &= (~0x01);		//	收到0
							}
						}
						spi0_comm_bit_num ++;
						
					}
					temp_old = temp;
				}
			}
			else {			//	时钟在帧传输之间保持低电平
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	下降沿
						//	下降沿采样数据
						if (spi0_comm_bit_num < 8) {			//	数据未接收完成
							if (ctr_var & LSBF) {		//	低位先传
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	收到1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	收到0
							}
							else {							//	高位先传
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	收到1
								else 					rec_temp &= (~0x01);		//	收到0
							}
						}
						spi0_comm_bit_num ++;
					}
					else if ((temp & SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	上升沿
						//	上升沿移出数据
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	低位先传
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;		//	移出最低位数据
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	高位先传
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;		//	移出最高位数据
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					temp_old = temp;
				}
			}
		}
		else {									//	相位控制，采样在第一个时钟边沿
			//	在第一个时钟采样时，数据需要在被片选后就移出数据
			if (ctr_var & LSBF) {		//	低位先传
				if (send_temp & 0x01)	IO0SET |= SPI0_MISO;		//	移出最低位数据
				else					IO0CLR |= SPI0_MISO;
			}
			else {		//	高位先传
				if (send_temp & 0x80)	IO0SET |= SPI0_MISO;		//	移出最高位数据
				else					IO0CLR |= SPI0_MISO;
			}
			if (ctr_var & CPOL) {			//	时钟在帧传输之间保持高电平
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	下降沿
						//	下降沿采样收数据
						if (spi0_comm_bit_num < 8) {			//	数据未接收完成
							if (ctr_var & LSBF) {		//	低位先传
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	收到1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	收到0
							}
							else {							//	高位先传
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	收到1
								else 					rec_temp &= (~0x01);		//	收到0
							}
						}
						spi0_comm_bit_num ++;
					}
					else if (((temp & SPI0_SCK) == SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	上升沿
						//	上升沿移出数据
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	低位先传
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;		//	移出最低位数据
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	高位先传
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;		//	移出最高位数据
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					temp_old = temp;
				}
			}
			else {			//	时钟在帧传输之间保持低电平
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	下降沿
						//	下降沿移出数据
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	低位先传
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;	//	移出最低位数据
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	高位先传
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;	//	移出最高位数据
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					else if ((temp & SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	上升沿
						//	上升沿采样数据
						if (spi0_comm_bit_num < 8) {			//	数据未接收完成
							if (ctr_var & LSBF) {		//	低位先传
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	收到1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	收到0
							}
							else {							//	高位先传
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	收到1
								else 					rec_temp &= (~0x01);		//	收到0
							}
						}
						spi0_comm_bit_num ++;
					}
					temp_old = temp;
				}
			}
		}
		if (spi0_comm_bit_num == 8) {
			SpiDataHandle(rec_temp);	//	收发数据处理
			spi0_rec_data = rec_temp;
			if (OSMboxPost(spi0_mbox,&spi0_rec_data) != OS_NO_ERR) {			//	如果还没读取则丢掉当前数据
				spi0_state |= ROVR;
			}
			//	回环测试
			/*
			if ((spi0_send_data==0xff) && (spi0_rec_data==0x00))  {

			}
			else if (spi0_rec_data != (spi0_send_data+1))  {
				goto err;
			}
			spi0_send_data = spi0_rec_data;	*/
		}
		else {
		/*err:
			bit_temp = spi0_comm_bit_num;
			spi0_rec_data = rec_temp;
			if (bit_temp == 8) {
				spi0_send_data = spi0_rec_data;
			}
			else {
				spi0_send_data ++;
			}*/
		}
	}
	if ((IO0PIN & (1<<15))==0) {

	}
	EXTINT = 0x04;
	VICVectAddr = 0;
	OS_EXIT_CRITICAL();
}


