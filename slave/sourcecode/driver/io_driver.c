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
uint8  IoInit(void)
{
/*	
	P00	����0����
	P01	����0����
	P02	SCL
	P03	SDA
	P04	SCK        ģ��
	P05	MISO	ģ��
	P06	MOSI	ģ��
	P07	CS		ģ��
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
	PINSEL0 = 0x00000000;
	PINSEL1 = 0x00000000;
	PINSEL2=PINSEL2&(~(0x01<<3));

	/*	���⹦������	*/
	PINSEL0 |= 0x00000005;	//	����UART0 ����
	PINSEL0 |= 0x00000050;	//	����I2C0 ����
	PINSEL0 |= 0x00005500;	//	����SPI0 �ڹ���
	PINSEL0 |= 0x00050000;	//	����UART1 ����
	//PINSEL0 |= 0x00000000;  //	����RJ45 ��������CTS
	//PINSEL1 |= 0x00000000;	//	����RJ45 ��������RTC
	//PINSEL1 |= 0x00000000;	//	����״̬LED ��������

	/*	GPIO ��������	*/
	IO0DIR	&=	(~SYN6288_BUSY);			//	�����ϳ�оƬ����״̬�����

	IO0DIR	|=	UART0_A;					//	uart0 ͨ���������
	IO0DIR	|=	UART0_B;					//	uart0 ͨ���������

	IO0DIR	|=	UART1_A;					//	uart1 ͨ���������
	IO0DIR	|=	UART1_B;					//	uart1 ͨ���������
	
	IO0DIR	|=	POWER_CONTROL_GPRS;					//	GPRS ģ���Դ �������
	IO0DIR	|=	POWER_CONTROL_RESERVED;				//	Ԥ����12V �������
	IO0DIR	|=	POWER_CONTROL_PRINT_MACHINE;		//	��ӡ����Դ�������
	IO0DIR	|=	POWER_CONTROL_IC_MACHINE;			//	�Ӵ�ʽIC����������Դ�������
	IO0DIR	|=	POWER_CONTROL_COIN_MACHINE;			//	Ӳ���������Դ�������
	IO0DIR	|=	POWER_CONTROL_GPS;					//	GPS ģ���Դ�������
	IO0DIR	|=	POWER_CONTROL_NOTE_MACHINE;			//	ֽ���������Դ�������
	
	IO0DIR	|=	SYS_STATE_LEN;						//	ϵͳ״̬�������
	
	//	��ʼ���رյ�Դ�͵���״̬��
	IO0CLR |= POWER_CONTROL_GPRS;			
	IO0CLR |= POWER_CONTROL_RESERVED;		
	IO0CLR |= POWER_CONTROL_PRINT_MACHINE;
	IO0CLR |= POWER_CONTROL_IC_MACHINE;	
	IO0CLR |= POWER_CONTROL_COIN_MACHINE;	
	IO0CLR |= POWER_CONTROL_GPS;			
	IO0CLR |= POWER_CONTROL_NOTE_MACHINE;	
	IO0CLR |= SYS_STATE_LEN;

	
#if (SPI0_MODE == 1)	
	/*	����ģ��SPI�ӻ�ģʽ
		IO0.4 -- SCK			����
		IO0.5 -- MOSI		����
		IO0.6 -- MISO		���
		IO0.7 -- SSL	����Ϊ�ж�
	*/
	PINSEL0 &= (~0x0000FF00);	//	��������SPI0��
	PINSEL0 |= 0x0000C000;	//	����SPI0 �ڹ���
	IO0DIR	&= (~SPI0_SCK);		//	�ӻ�����	
	IO0DIR	&= (~SPI0_MOSI);	//	�ӻ�����
	IO0DIR	|= SPI0_MISO;		//	�ӻ����
	EXTMODE |= (1<<2);			//	���ش���
	EXTPOLAR &= (~(1<<2));		//	�½��ش���
	EXTINT = 0x04;
#endif


	return 1;
	
}
uint8 bit_temp;

void Io2Exception(void) {
	uint32 temp,temp_old;				//	IO������
	uint16 rec_temp=0;					//	���ջ���
	uint8  send_temp=spi0_send_data;	//	���ͻ���
	//uint8  send_temp=0xAA;
	uint8  ctr_var=SPI0_COMM_CONFIG;		//	����������
	uint8  spi0_comm_bit_num = 0;		//	����λ��
	
	OS_ENTER_CRITICAL();

	temp = IO0PIN;
	temp_old = temp;
	if ((temp & SPI0_CS) == 0) {
		if (ctr_var & CPHA) {				//	��λ���ƣ������ڵڶ���ʱ�ӱ���
			if (ctr_var & CPOL) {			//	ʱ����֡����֮�䱣�ָߵ�ƽ
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	�½���
						//	�½����Ƴ�����
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	��λ�ȴ�
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					else if ((temp & SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	������
						//	�����ز�������
						if (spi0_comm_bit_num < 8) {			//	����δ�������
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	�յ�1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	�յ�0
							}
							else {							//	��λ�ȴ�
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	�յ�1
								else 					rec_temp &= (~0x01);		//	�յ�0
							}
						}
						spi0_comm_bit_num ++;
						
					}
					temp_old = temp;
				}
			}
			else {			//	ʱ����֡����֮�䱣�ֵ͵�ƽ
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	�½���
						//	�½��ز�������
						if (spi0_comm_bit_num < 8) {			//	����δ�������
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	�յ�1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	�յ�0
							}
							else {							//	��λ�ȴ�
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	�յ�1
								else 					rec_temp &= (~0x01);		//	�յ�0
							}
						}
						spi0_comm_bit_num ++;
					}
					else if ((temp & SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	������
						//	�������Ƴ�����
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	��λ�ȴ�
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					temp_old = temp;
				}
			}
		}
		else {									//	��λ���ƣ������ڵ�һ��ʱ�ӱ���
			//	�ڵ�һ��ʱ�Ӳ���ʱ��������Ҫ�ڱ�Ƭѡ����Ƴ�����
			if (ctr_var & LSBF) {		//	��λ�ȴ�
				if (send_temp & 0x01)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
				else					IO0CLR |= SPI0_MISO;
			}
			else {		//	��λ�ȴ�
				if (send_temp & 0x80)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
				else					IO0CLR |= SPI0_MISO;
			}
			if (ctr_var & CPOL) {			//	ʱ����֡����֮�䱣�ָߵ�ƽ
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	�½���
						//	�½��ز���������
						if (spi0_comm_bit_num < 8) {			//	����δ�������
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	�յ�1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	�յ�0
							}
							else {							//	��λ�ȴ�
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	�յ�1
								else 					rec_temp &= (~0x01);		//	�յ�0
							}
						}
						spi0_comm_bit_num ++;
					}
					else if (((temp & SPI0_SCK) == SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	������
						//	�������Ƴ�����
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	��λ�ȴ�
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;		//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					temp_old = temp;
				}
			}
			else {			//	ʱ����֡����֮�䱣�ֵ͵�ƽ
				while (((temp=IO0PIN) & SPI0_CS) == 0) {
					if ((temp_old & SPI0_SCK) && ((temp & SPI0_SCK) == 0)) {		//	�½���
						//	�½����Ƴ�����
						if (spi0_comm_bit_num < 8) {
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if ((send_temp>>spi0_comm_bit_num) & 0x01)	IO0SET |= SPI0_MISO;	//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
							else {		//	��λ�ȴ�
								if ((send_temp<<spi0_comm_bit_num) & 0x80)	IO0SET |= SPI0_MISO;	//	�Ƴ����λ����
								else										IO0CLR |= SPI0_MISO;
							}
						}
					}
					else if ((temp & SPI0_SCK) && ((temp_old & SPI0_SCK) == 0)) {		//	������
						//	�����ز�������
						if (spi0_comm_bit_num < 8) {			//	����δ�������
							if (ctr_var & LSBF) {		//	��λ�ȴ�
								if (temp & SPI0_MOSI) rec_temp |= (0x01<<spi0_comm_bit_num);			//	�յ�1
								else 					rec_temp &= (~(0x01<<spi0_comm_bit_num));		//	�յ�0
							}
							else {							//	��λ�ȴ�
								rec_temp <<= 1;
								if (temp & SPI0_MOSI) rec_temp |= 0x01;			//	�յ�1
								else 					rec_temp &= (~0x01);		//	�յ�0
							}
						}
						spi0_comm_bit_num ++;
					}
					temp_old = temp;
				}
			}
		}
		if (spi0_comm_bit_num == 8) {
			SpiDataHandle(rec_temp);	//	�շ����ݴ���
			spi0_rec_data = rec_temp;
			if (OSMboxPost(spi0_mbox,&spi0_rec_data) != OS_NO_ERR) {			//	�����û��ȡ�򶪵���ǰ����
				spi0_state |= ROVR;
			}
			//	�ػ�����
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


