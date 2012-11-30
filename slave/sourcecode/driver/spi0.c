#include "includes.h"

#define SPI0RATE			500000				//	5500000>SPI0RATE>50 000
#define SPI_DATA_BUFFER_LENGHT				100

uint8 spi0_send_data, spi0_rec_data;
uint8 spi0_state;
OS_EVENT *spi0_mbox;
uint32 spi0_tick;					//	SPI��ʱ��
unsigned char spi_data_temp[32];	//	SPI ���ݻ���
_data_pond_s data_pond[8];			//	���ܽӿ�
//	0--����豸����
//	1--�豸ֱ��ͨ�ſ���
//	7--·������


void Spi0Init(void)
{
	uint8 err;
#if (SPI0_MODE == 0)
	S0PCR  = (0 << 2) |				//	ÿ֡����8bit
	         (0 << 3) |				// CPHA = 0, ������SCK �ĵ�һ��ʱ���ز���
	         (1 << 4) |				// CPOL = 1, SCK Ϊ����Ч
	         (0 << 5) |				// MSTR = 0, SPI ���ڴ�ģʽ
	         (1 << 6) |				// LSBF = 1, SPI ���ݴ���LSB (λ0)����
	         (0 << 7);				// SPIE = 1, SPI �жϱ�ʹ��
	S0PSR |= 0x00;
	S0PDR = 0x00;
#else
	S0PCR &= (~(1 << 7));
#endif
	//S0PINT = 0x00;
	/*if (QueueCreate((void *)spisendbuffer,sizeof(spisendbuffer),NULL,NULL) != QUEUE_OK) {
		while(1);
	}
	if (QueueCreate((void *)spirecbuffer,sizeof(spirecbuffer),NULL,NULL) != QUEUE_OK) {
		while(1);
	}*/
	spi0_mbox = OSMboxCreate((void *)1);
	if (spi0_mbox == NULL)
	{
		while (1);
	}
	OSMboxPend(spi0_mbox, 0, &err);
	//memset(fun_data,NULL,sizeof(fun_data));
	data_pond[0].fun_data_addr = &device_control;
	data_pond[0].len           = DEVICE_CONTROL_DATA_LENGHT;
	data_pond[1].fun_data_addr = NULL;
	data_pond[1].len           = 0;
	data_pond[2].fun_data_addr = NULL;
	data_pond[2].len           = 0;
	data_pond[3].fun_data_addr = NULL;
	data_pond[3].len           = 0;
	data_pond[4].fun_data_addr = NULL;
	data_pond[4].len           = 0;
	data_pond[5].fun_data_addr = NULL;
	data_pond[5].len           = 0;
	data_pond[6].fun_data_addr = &config_ram;
	data_pond[6].len           = sizeof(_config_s);
	data_pond[7].fun_data_addr = &curr_line;
	data_pond[7].len           = sizeof(_line_mess_s);
}


#define CHIP_COMM_IDLE					0
#define	CHIP_COMM_ADDR					1
#define	CHIP_COMM_LEN					2
#define	CHIP_COMM_READ					3
#define	CHIP_COMM_WRITE					4
#define	CHIP_COMM_EXE					5
#define	CHIP_COMM_VERIFY				6
#define CHIP_COMM_READ_CRC				7
#define CHIP_COMM_WRITE_CRC				8
uint8 SpiDataHandle(uint8 rec_temp)
{

	//uint8 ;
	static uint16 op_addr, comm_num, bcc, res_bcc;
	static uint8 temp_8, op_len, chip_comm_state = CHIP_COMM_IDLE;

	if (spi0_tick > (OS_TICKS_PER_SEC / 200))
	{
		chip_comm_state = CHIP_COMM_IDLE;
	}
	switch (chip_comm_state)
	{
	case CHIP_COMM_IDLE:
		if (rec_temp == '<')
		{
			chip_comm_state = CHIP_COMM_ADDR;
			bcc = 0;
			temp_8 = 0;
			spi0_send_data = 0;
		}
		break;
	case CHIP_COMM_ADDR:			//	���ղ�����ַ
		//	��ַ�ṹ��	��15 λ( ���λ) ��ʾ��д;1--��
		//					��14-12 λ��ʾ������
		//					��11-00 λ��ʾ��ַ
		bcc = CRCByte(bcc, rec_temp);
		op_addr = (op_addr << 8) + rec_temp;
		if ((++temp_8) > 1)
		{
			chip_comm_state = CHIP_COMM_LEN;
			temp_8 = 0;
		}
		break;
	case CHIP_COMM_LEN:				//	���ճ���
		if ((rec_temp > 32) || (rec_temp == 0))  		//	ÿ֡��������ݳ��Ȳ��ܳ���32��
		{
			chip_comm_state = CHIP_COMM_IDLE;
			break;
		}
		bcc = CRCByte(bcc, rec_temp);
		op_len = rec_temp;

		if (op_addr & 0x8000)  			//	���λΪ1��ʾ��
		{
			chip_comm_state = CHIP_COMM_READ;
			spi0_send_data = ((uint8 *)data_pond[(op_addr >> 12) & 0x0007].fun_data_addr)[op_addr & 0x0fff];			//	׼�����͵�һ������
			bcc = CRCByte(bcc, spi0_send_data);
			comm_num = 1;
		}
		else  							//	���λΪ0��ʾд
		{
			chip_comm_state = CHIP_COMM_WRITE;
			comm_num = 0;
		}
		temp_8 = 0;
		break;
	case CHIP_COMM_READ:			//	������ȡ����
		if ((comm_num < op_len) && ((comm_num + op_addr & 0x0fff) < data_pond[(op_addr >> 12) & 0x0007].len))
		{
			spi0_send_data = ((uint8 *)data_pond[(op_addr >> 12) & 0x0007].fun_data_addr)[(op_addr + comm_num) & 0x0fff];	//	׼������
			bcc = CRCByte(bcc, spi0_send_data);
		}
		else if (comm_num < op_len)
		{
			spi0_send_data = 0;
			bcc = CRCByte(bcc, spi0_send_data);
		}
		else
		{
			spi0_send_data = (uint8)(bcc >> 8);
			chip_comm_state = CHIP_COMM_READ_CRC;
		}
		comm_num++;
		break;

	case CHIP_COMM_READ_CRC:
		spi0_send_data = (uint8)bcc;
		chip_comm_state = CHIP_COMM_IDLE;
		break;

	case CHIP_COMM_WRITE:			//	����д����
		if ((comm_num < (op_len - 1)) && ((comm_num + (op_addr & 0x0FFF)) < data_pond[(op_addr >> 12) & 0x0007].len))
		{
			spi_data_temp[comm_num] = rec_temp;
			bcc = CRCByte(bcc, spi_data_temp[comm_num]);
		}
		else if (comm_num < (op_len - 1))
		{
			spi_data_temp[comm_num] = rec_temp;
			bcc = CRCByte(bcc, spi_data_temp[comm_num]);
		}
		else if (comm_num == (op_len - 1))
		{
			spi_data_temp[comm_num] = rec_temp;
			bcc = CRCByte(bcc, spi_data_temp[comm_num]);
			spi0_send_data = (uint8)(bcc >> 8);
			res_bcc = 0;
			chip_comm_state = CHIP_COMM_WRITE_CRC;
		}
		comm_num++;
		break;

	case CHIP_COMM_WRITE_CRC:
		res_bcc = rec_temp;
		spi0_send_data = (uint8)bcc;
		chip_comm_state = CHIP_COMM_VERIFY;

		break;

	case CHIP_COMM_VERIFY:			//	У������
		res_bcc = ((res_bcc << 8) & 0xff00) + rec_temp;
		if (res_bcc == bcc)  		//	����У����ȷ
		{
			if (((op_addr & 0x0fff) >= data_pond[(op_addr >> 12) & 0x0007].len) || (op_len > 32))
			{
			}
			else if ((op_len + (op_addr & 0x0FFF)) >= data_pond[(op_addr >> 12) & 0x0007].len)
			{
				op_len = data_pond[(op_addr >> 12) & 0x0007].len - (op_addr & 0x0FFF);
				memcpy(&((uint8 *)data_pond[(op_addr >> 12) & 0x0007].fun_data_addr)[op_addr & 0x0FFF], spi_data_temp, op_len);
			}
			else
			{
				memcpy(&((uint8 *)data_pond[(op_addr >> 12) & 0x0007].fun_data_addr)[op_addr & 0x0FFF], spi_data_temp, op_len);
			}
		}
		chip_comm_state = CHIP_COMM_IDLE;
	case CHIP_COMM_EXE:
		break;
	}

	spi0_tick = 0;

	return chip_comm_state;
}





uint8 data_temp;

void Spi0_Exception(void)
{
	uint8 temp;
	OS_ENTER_CRITICAL();
	temp = S0PSR;
	spi0_rec_data = S0PDR;			//	ȡ����
	SpiDataHandle(spi0_rec_data);	//	��������
	S0PDR = spi0_send_data;			//	��������
	S0PINT = 0x01;
	VICVectAddr = 0;
	OS_EXIT_CRITICAL();
}






